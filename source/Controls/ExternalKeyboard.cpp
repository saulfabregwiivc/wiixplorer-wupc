/*-------------------------------------------------------------
keyboard.c -- keyboard event system

Copyright (C) 2008, 2009
DAVY Guillaume davyg2@gmail.com
Brian Johnson brijohn@gmail.com
dhewg

Convert to C++ and scan thread suspend/resume modification by Dimok

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.
-------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "ExternalKeyboard.h"

#define MAX_QUEUE_SIZE		255

extern "C" {
	keysym_t ksym_upcase(keysym_t);
}

ExternalKeyboard *ExternalKeyboard::instance = NULL;

ExternalKeyboard::ExternalKeyboard()
{
	bExitRequested = false;
	bSuspendScan = true;
	iNewDeviceScanTimer = 0;

	_ukbd_keymapdata.keydesc = ukbd_keydesctab;
	_ukbd_keymapdata.layout = KB_NONE;
	_sc_maplen = 0;
	_sc_map = 0;
	_composebuf[0] = 0;
	_composebuf[1] = 0;
	_composelen = 0;
	_modifiers = 0;

	if (USB_Initialize() != IPC_OK)
		return;

	if (USBKeyboard_Initialize() == IPC_OK) {
		LoadKeyboardLayout();
		ScanForNewDevice();
	}
}

ExternalKeyboard::~ExternalKeyboard()
{
	bExitRequested = true;
	bSuspendScan = false;

	USBKeyboard_Close();
	USBKeyboard_Deinitialize();

	if (_sc_map) {
		free(_sc_map);
		_sc_map = NULL;
		_sc_maplen = 0;
	}
}

void ExternalKeyboard::executeThread(void)
{
	while(!bExitRequested)
	{
		if(bSuspendScan)
			suspendThread();

		if(!bExitRequested)
		{
			// scan for new attached keyboards
			if (++iNewDeviceScanTimer > 300) {
				if (!USBKeyboard_IsConnected())
					ScanForNewDevice();
				iNewDeviceScanTimer = 0;
			}

			USBKeyboard_Scan();
			usleep(10000);
		}
	}
}

void ExternalKeyboard::update_modifier(u_int type, int toggle, int mask)
{
	if (toggle) {
		if (type == KEYBOARD_PRESSED)
			_modifiers ^= mask;
	} else {
		if (type == KEYBOARD_RELEASED)
			_modifiers &= ~mask;
		else
			_modifiers |= mask;
	}
}

void ExternalKeyboard::keyboardEventCb(USBKeyboard_event kevent)
{
	instance->keyboardEvent(kevent);
}

void ExternalKeyboard::keyboardEvent(USBKeyboard_event kevent)
{
	if(eventQueue.size() >= MAX_QUEUE_SIZE)
		return;

	keyboard_event event;
	struct wscons_keymap kp;
	keysym_t *group;
	int gindex;
	keysym_t ksym;
	int i;

	switch (kevent.type) {
	case USBKEYBOARD_DISCONNECTED:
		event.type = KEYBOARD_DISCONNECTED;
		event.modifiers = 0;
		event.keycode = 0;
		event.symbol = 0;

		addEvent(event);

		return;

	case USBKEYBOARD_PRESSED:
		event.type = KEYBOARD_PRESSED;
		break;

	case USBKEYBOARD_RELEASED:
		event.type = KEYBOARD_RELEASED;
		break;

	default:
		return;
	}

	event.keycode = kevent.keyCode;

	wskbd_get_mapentry(&_ukbd_keymapdata, event.keycode, &kp);

	/* Now update modifiers */
	switch (kp.group1[0]) {
	case KS_Shift_L:
		update_modifier(event.type, 0, MOD_SHIFT_L);
		break;

	case KS_Shift_R:
		update_modifier(event.type, 0, MOD_SHIFT_R);
		break;

	case KS_Shift_Lock:
		update_modifier(event.type, 1, MOD_SHIFTLOCK);
		break;

	case KS_Caps_Lock:
		update_modifier(event.type, 1, MOD_CAPSLOCK);
		USBKeyboard_SetLed(USBKEYBOARD_LEDCAPS,
							MOD_ONESET(_modifiers, MOD_CAPSLOCK));
		break;

	case KS_Control_L:
		update_modifier(event.type, 0, MOD_CONTROL_L);
		break;

	case KS_Control_R:
		update_modifier(event.type, 0, MOD_CONTROL_R);
		break;

	case KS_Alt_L:
		update_modifier(event.type, 0, MOD_META_L);
		break;

	case KS_Alt_R:
		update_modifier(event.type, 0, MOD_META_R);
		break;

	case KS_Mode_switch:
		update_modifier(event.type, 0, MOD_MODESHIFT);
		break;

	case KS_Mode_Lock:
		update_modifier(event.type, 1, MOD_MODELOCK);
		break;

	case KS_Num_Lock:
		update_modifier(event.type, 1, MOD_NUMLOCK);
		USBKeyboard_SetLed(USBKEYBOARD_LEDNUM,
							MOD_ONESET(_modifiers, MOD_NUMLOCK));
		break;

	case KS_Hold_Screen:
		update_modifier(event.type, 1, MOD_HOLDSCREEN);
		USBKeyboard_SetLed(USBKEYBOARD_LEDSCROLL,
							MOD_ONESET(_modifiers, MOD_HOLDSCREEN));
		break;
	}

	/* Get the keysym */
	if ((_modifiers & (MOD_MODESHIFT|MOD_MODELOCK)) && !MOD_ONESET(_modifiers, MOD_ANYCONTROL))
		group = &kp.group2[0];
	else
		group = &kp.group1[0];

	if ((_modifiers & MOD_NUMLOCK) &&
		KS_GROUP(group[1]) == KS_GROUP_Keypad) {
		gindex = !MOD_ONESET(_modifiers, MOD_ANYSHIFT);
		ksym = group[gindex];
	} else {
		/* CAPS alone should only affect letter keys */
		if ((_modifiers & (MOD_CAPSLOCK | MOD_ANYSHIFT)) ==
			MOD_CAPSLOCK) {
			gindex = 0;
			ksym = ksym_upcase(group[0]);
		} else {
			gindex = MOD_ONESET(_modifiers, MOD_ANYSHIFT);
			ksym = group[gindex];
		}
	}

	/* Process compose sequence and dead accents */
	switch (KS_GROUP(ksym)) {
	case KS_GROUP_Mod:
		if (ksym == KS_Multi_key) {
			update_modifier(KEYBOARD_PRESSED, 0, MOD_COMPOSE);
			_composelen = 2;
		}
		break;

	case KS_GROUP_Dead:
		if (event.type != KEYBOARD_PRESSED)
			return;

		if (_composelen == 0) {
			update_modifier(KEYBOARD_PRESSED, 0, MOD_COMPOSE);
			_composelen = 1;
			_composebuf[0] = ksym;

			return;
		}
		break;
	}

	if ((event.type == KEYBOARD_PRESSED) && (_composelen > 0)) {
		/*
		 * If the compose key also serves as AltGr (i.e. set to both
		 * KS_Multi_key and KS_Mode_switch), and would provide a valid,
		 * distinct combination as AltGr, leave compose mode.
		 */
		if (_composelen == 2 && group == &kp.group2[0]) {
			if (kp.group1[gindex] != kp.group2[gindex])
				_composelen = 0;
		}

		if (_composelen != 0) {
			_composebuf[2 - _composelen] = ksym;
			if (--_composelen == 0) {
				ksym = wskbd_compose_value(_composebuf);
				update_modifier(KEYBOARD_RELEASED, 0, MOD_COMPOSE);
			} else {
				return;
			}
		}
	}

	// store up to MAXHELD pressed events to match the symbol for release
	switch (KS_GROUP(ksym)) {
	case KS_GROUP_Ascii:
	case KS_GROUP_Keypad:
	case KS_GROUP_Function:
		if (event.type == KEYBOARD_PRESSED) {
			for (i = 0; i < MAXHELD; ++i) {
				if (_held[i].keycode == 0) {
					_held[i].keycode = event.keycode;
					_held[i].symbol = ksym;

					break;
				}
			}
		} else {
			for (i = 0; i < MAXHELD; ++i) {
				if (_held[i].keycode == event.keycode) {
					ksym = _held[i].symbol;
					_held[i].keycode = 0;
					_held[i].symbol = 0;

					break;
				}
			}
		}

		break;
	}

	event.symbol = ksym;
	event.modifiers = _modifiers;

	addEvent(event);
}

void ExternalKeyboard::ScanForNewDevice(void)
{
	keyboard_event event;

	s32 ret = USBKeyboard_Open(&ExternalKeyboard::keyboardEventCb);
	if (ret < 0)
		return;

	_modifiers = 0;
	_composelen = 0;
	memset(_held, 0, sizeof(_held));

	USBKeyboard_SetLed(USBKEYBOARD_LEDNUM, true);
	USBKeyboard_SetLed(USBKEYBOARD_LEDCAPS, true);
	USBKeyboard_SetLed(USBKEYBOARD_LEDSCROLL, true);
	usleep(200 * 1000);
	USBKeyboard_SetLed(USBKEYBOARD_LEDNUM, false);
	USBKeyboard_SetLed(USBKEYBOARD_LEDCAPS, false);
	USBKeyboard_SetLed(USBKEYBOARD_LEDSCROLL, false);

	event.type = KEYBOARD_CONNECTED;
	event.modifiers = 0;
	event.keycode = 0;

	addEvent(event);
}

struct nameint {
	int value;
	const char *name;
};

static struct nameint kbdenc_tab[] = {
	KB_ENCTAB
};

static struct nameint kbdvar_tab[] = {
	KB_VARTAB
};

static kbd_t _get_keymap_by_name(const char *identifier) {
	char name[64];
	u8 i, j;
	kbd_t encoding, variant;

	kbd_t res = KB_NONE;

	if (!identifier || (strlen(identifier) < 2))
		return res;

	i = 0;
	for (i = 0; ukbd_keydesctab[i].name != 0; ++i) {
		if (ukbd_keydesctab[i].name & KB_HANDLEDBYWSKBD)
			continue;

		encoding = KB_ENCODING(ukbd_keydesctab[i].name);
		variant = KB_VARIANT(ukbd_keydesctab[i].name);

		name[0] = 0;
		for (j = 0; j < sizeof(kbdenc_tab) / sizeof(struct nameint); ++j)
			if (encoding == (kbd_t)kbdenc_tab[j].value) {
				strcpy(name, kbdenc_tab[j].name);
				break;
			}

		if (strlen(name) < 1)
			continue;

		for (j = 0; j < sizeof(kbdvar_tab) / sizeof(struct nameint); ++j)
			if (variant & kbdvar_tab[j].value) {
				strcat(name, "-");
				strcat(name,  kbdvar_tab[j].name);
			}

		if (!strcmp(identifier, name)) {
			res = ukbd_keydesctab[i].name;
			break;
		}
	}

	return res;
}

void ExternalKeyboard::LoadKeyboardLayout(void)
{
	int fd;
	struct stat st;
	size_t i;
	char keymap[64];

	if (_sc_map) {
		free(_sc_map);
		_sc_map = NULL;
		_sc_maplen = 0;
	}

	_ukbd_keymapdata.layout = KB_NONE;

	if (_ukbd_keymapdata.layout == KB_NONE) {
		keymap[0] = 0;
		fd = open("/wiikbd.map", O_RDONLY);
		if ((fd > 0) && !fstat(fd, &st)) {
			if ((st.st_size > 0) && (st.st_size < 64) &&
				(st.st_size == read(fd, keymap, st.st_size))) {
				keymap[63] = 0;
				for (i = 0; i < 64; ++i) {
					if ((keymap[i] != '-') && (isalpha((int)keymap[i]) == 0)) {
						keymap[i] = 0;
						break;
					}
				}
			}
			close(fd);
		}
		_ukbd_keymapdata.layout = _get_keymap_by_name(keymap);
	}

	if (_ukbd_keymapdata.layout == KB_NONE) {
		switch (CONF_GetLanguage()) {
		case CONF_LANG_GERMAN:
			_ukbd_keymapdata.layout = KB_DE;
			break;

		case CONF_LANG_JAPANESE:
			_ukbd_keymapdata.layout = KB_JP;
			break;

		case CONF_LANG_FRENCH:
			_ukbd_keymapdata.layout = KB_FR;
			break;

		case CONF_LANG_SPANISH:
			_ukbd_keymapdata.layout = KB_ES;
			break;

		case CONF_LANG_ITALIAN:
			_ukbd_keymapdata.layout = KB_IT;
			break;

		case CONF_LANG_DUTCH:
			_ukbd_keymapdata.layout = KB_NL;
			break;

		case CONF_LANG_SIMP_CHINESE:
		case CONF_LANG_TRAD_CHINESE:
		case CONF_LANG_KOREAN:
		default:
			_ukbd_keymapdata.layout = KB_US;
			break;
		}
	}

	if (wskbd_load_keymap(&_ukbd_keymapdata, &_sc_map, &_sc_maplen) < 0) {
		_ukbd_keymapdata.layout = KB_NONE;
	}
}
