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
#ifndef EXTERNALKEYBOARD_H_
#define EXTERNALKEYBOARD_H_

#include <queue>
#include <gctypes.h>
#include <wiikeyboard/keyboard.h>
#include <wiikeyboard/usbkeyboard.h>
#include "Controls/CMutex.h"
#include "Controls/CThread.h"
#include "Tools/wsksymvar.h"

#define MAXHELD 8

extern const struct wscons_keydesc ukbd_keydesctab[];

class ExternalKeyboard : public CThread
{
public:
	static ExternalKeyboard *Instance(void) { if(!instance) instance = new ExternalKeyboard(); return instance; }
	static void DestroyInstance(void) { if(instance) delete instance; instance = NULL; }

	void ResumeScanThread(void) { bSuspendScan = false; resumeThread(); }
	void SuspendScanThread(void) { bSuspendScan = true; }

	bool getEvent(keyboard_event &event) {
		mutex.lock();
		if(eventQueue.empty()) {
			mutex.unlock();
			return false;
		}
		event = eventQueue.front();
		eventQueue.pop();
		mutex.unlock();
		return true;
	}

	void clearQueue(void) { mutex.lock(); eventQueue = std::queue<keyboard_event>(); mutex.unlock(); }
private:
	ExternalKeyboard();
	virtual ~ExternalKeyboard();

	static ExternalKeyboard *instance;

	void addEvent(keyboard_event &event) {
		mutex.lock();
		eventQueue.push(event);
		mutex.unlock();
	}

	void LoadKeyboardLayout(void);
	void executeThread(void);

	static void keyboardEventCb(USBKeyboard_event event);
	void update_modifier(u_int type, int toggle, int mask);
	void keyboardEvent(USBKeyboard_event event);
	void ScanForNewDevice(void);

	CMutex mutex;
	std::queue<keyboard_event> eventQueue;

	bool bExitRequested;
	bool bSuspendScan;
	int iNewDeviceScanTimer;
	u16 _modifiers;
	int _composelen;

	struct {
		u8 keycode;
		u16 symbol;
	} _held[MAXHELD];

	struct wskbd_mapdata _ukbd_keymapdata;
	keysym_t _composebuf[2];
	int _sc_maplen;
	struct wscons_keymap *_sc_map;
};

#endif /* EXTERNALKEYBOARD_H_ */
