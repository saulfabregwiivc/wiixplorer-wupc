/****************************************************************************
 * Copyright (C) 2009-2013 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include "gui_keyboard.h"
#include "Memory/Resources.h"

/**
 * Constructor for the GuiKeyboardAlone class.
 */
GuiKeyboard::GuiKeyboard(void)
{
	//! Start the external keyboard scan thread
	ExternalKeyboard::Instance()->ResumeScanThread();
	ExternalKeyboard::Instance()->clearQueue();

	width = 14*42+40;
	height = 5*42+80;
	shift = false;
	caps = false;
	UpdateKeys = false;
	DefaultKeys = false;
	selectable = true;
	ShiftChan = -1;
	DeleteDelay = 0;
	memset(&keyboardEvent, 0, sizeof(keyboardEvent));

	memset(keys, 0, sizeof(keys));

	wcsncpy(keys[0].ch,		wfmt("%s", tr("`1234567890-=")), MAXKEYS);
	wcsncpy(keys[0].chShift,wfmt("%s", tr("~!@#$%^&*()_+")), MAXKEYS);
	wcsncpy(keys[1].ch,		wfmt("%s", tr("qwertyuiop[]\\")), MAXKEYS);
	wcsncpy(keys[1].chShift,wfmt("%s", tr("QWERTYUIOP{}|")), MAXKEYS);
	wcsncpy(keys[2].ch,		wfmt("%s", tr("asdfghjkl;'")), MAXKEYS);
	wcsncpy(keys[2].chShift,wfmt("%s", tr("ASDFGHJKL:\"")), MAXKEYS);
	wcsncpy(keys[3].ch,		wfmt("%s", tr("zxcvbnm,./")), MAXKEYS);
	wcsncpy(keys[3].chShift,wfmt("%s", tr("ZXCVBNM<>?")), MAXKEYS);

	int KeyboardPosition = 0;

	key = Resources::GetImageData("keyboard_key.png");
	keyOver = Resources::GetImageData("keyboard_key_over.png");
	keyMedium = Resources::GetImageData("keyboard_mediumkey.png");
	keyMediumOver = Resources::GetImageData("keyboard_mediumkey_over.png");
	keyLarge = Resources::GetImageData("keyboard_largekey.png");
	keyLargeOver = Resources::GetImageData("keyboard_largekey_over.png");

	keySoundOver = Resources::GetSound("button_over.wav");
	keySoundClick = Resources::GetSound("button_click.wav");

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	keyBackImg = new GuiImage(keyMedium);
	keyBackOverImg = new GuiImage(keyMediumOver);
	keyBackText = new GuiText(tr("Back"), 20, (GXColor){0, 0, 0, 0xff});
	keyBack = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyBack->SetImage(keyBackImg);
	keyBack->SetImageOver(keyBackOverImg);
	keyBack->SetLabel(keyBackText);
	keyBack->SetSoundOver(keySoundOver);
	keyBack->SetSoundClick(keySoundClick);
	keyBack->SetTrigger(trigA);
	keyBack->SetPosition(12*42+40+KeyboardPosition, 0*42+80);
	keyBack->SetEffectGrow();
	keyBack->Clicked.connect(this, &GuiKeyboard::OnSpecialKeyPress);
	this->Append(keyBack);

	keyTabImg = new GuiImage(keyMedium);
	keyTabOverImg = new GuiImage(keyMediumOver);
	keyTabText = new GuiText(tr("Tab"), 20, (GXColor){0, 0, 0, 0xff});
	keyTab = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyTab->SetImage(keyTabImg);
	keyTab->SetImageOver(keyTabOverImg);
	keyTab->SetLabel(keyTabText);
	keyTab->SetSoundOver(keySoundOver);
	keyTab->SetSoundClick(keySoundClick);
	keyTab->SetTrigger(trigA);
	keyTab->SetPosition(0+KeyboardPosition, 1*42+80);
	keyTab->SetEffectGrow();
	keyTab->Clicked.connect(this, &GuiKeyboard::OnSpecialKeyPress);
	this->Append(keyTab);

	keyCapsImg = new GuiImage(keyMedium);
	keyCapsOverImg = new GuiImage(keyMediumOver);
	keyCapsText = new GuiText(tr("Caps"), 20, (GXColor){0, 0, 0, 0xff});
	keyCaps = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyCaps->SetImage(keyCapsImg);
	keyCaps->SetImageOver(keyCapsOverImg);
	keyCaps->SetLabel(keyCapsText);
	keyCaps->SetSoundOver(keySoundOver);
	keyCaps->SetSoundClick(keySoundClick);
	keyCaps->SetTrigger(trigA);
	keyCaps->SetPosition(0+KeyboardPosition, 2*42+80);
	keyCaps->SetEffectGrow();
	keyCaps->Clicked.connect(this, &GuiKeyboard::OnSpecialKeyPress);
	this->Append(keyCaps);

	keyShiftImg = new GuiImage(keyMedium);
	keyShiftOverImg = new GuiImage(keyMediumOver);
	keyShiftText = new GuiText(tr("Shift"), 20, (GXColor){0, 0, 0, 0xff});
	keyShift = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyShift->SetImage(keyShiftImg);
	keyShift->SetImageOver(keyShiftOverImg);
	keyShift->SetLabel(keyShiftText);
	keyShift->SetSoundOver(keySoundOver);
	keyShift->SetSoundClick(keySoundClick);
	keyShift->SetTrigger(trigA);
	keyShift->SetPosition(0+KeyboardPosition, 3*42+80);
	keyShift->SetEffectGrow();
	keyShift->Clicked.connect(this, &GuiKeyboard::OnSpecialKeyPress);
	this->Append(keyShift);

	keyLangImg = new GuiImage(keyMedium);
	keyLangOverImg = new GuiImage(keyMediumOver);
	keyLangText = new GuiText(tr("Lang."), 20, (GXColor){0, 0, 0, 0xff});
	keyLang = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyLang->SetImage(keyLangImg);
	keyLang->SetImageOver(keyLangOverImg);
	keyLang->SetLabel(keyLangText);
	keyLang->SetSoundOver(keySoundOver);
	keyLang->SetSoundClick(keySoundClick);
	keyLang->SetTrigger(trigA);
	keyLang->SetPosition(0+KeyboardPosition, 4*42+80);
	keyLang->SetEffectGrow();
	keyLang->Clicked.connect(this, &GuiKeyboard::OnSpecialKeyPress);
	this->Append(keyLang);

	keyEnterText = new GuiText(tr("Enter"), 18, (GXColor){0, 0, 0, 0xff});
	keyEnterImg = new GuiImage(keyMedium);
	keyEnterOverImg = new GuiImage(keyMediumOver);
	keyEnter = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyEnter->SetImage(keyEnterImg);
	keyEnter->SetImageOver(keyEnterOverImg);
	keyEnter->SetSoundOver(keySoundOver);
	keyEnter->SetSoundClick(keySoundClick);
	keyEnter->SetLabel(keyEnterText);
	keyEnter->SetTrigger(trigA);
	keyEnter->SetPosition(12*42+37+KeyboardPosition, 2*42+80);
	keyEnter->SetEffectGrow();
	keyEnter->Clicked.connect(this, &GuiKeyboard::OnSpecialKeyPress);
	this->Append(keyEnter);

	keySpaceImg = new GuiImage(keyLarge);
	keySpaceOverImg = new GuiImage(keyLargeOver);
	keySpace = new GuiButton(keyLarge->GetWidth(), keyLarge->GetHeight());
	keySpace->SetImage(keySpaceImg);
	keySpace->SetImageOver(keySpaceOverImg);
	keySpace->SetSoundOver(keySoundOver);
	keySpace->SetSoundClick(keySoundClick);
	keySpace->SetTrigger(trigA);
	keySpace->SetPosition(0, 4*42+80);
	keySpace->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	keySpace->SetEffectGrow();
	keySpace->Clicked.connect(this, &GuiKeyboard::OnSpecialKeyPress);
	this->Append(keySpace);

	int Pos = 0;
	wchar_t txt[2] = { 0, 0 };

	for(int i = 0; i < MAXROWS; i++)
	{
		for(int j = 0; j < MAXKEYS; j++)
		{
			txt[0] = keys[i].ch[j];
			keyImg[i][j] = new GuiImage(key);
			keyImgOver[i][j] = new GuiImage(keyOver);
			keyTxt[i][j] = new GuiText(txt, 20, (GXColor){0, 0, 0, 0xff});
			keyTxt[i][j]->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
			keyTxt[i][j]->SetPosition(0, -10);
			keyBtn[i][j] = new GuiButton(key->GetWidth(), key->GetHeight());
			keyBtn[i][j]->SetImage(keyImg[i][j]);
			keyBtn[i][j]->SetImageOver(keyImgOver[i][j]);
			keyBtn[i][j]->SetSoundOver(keySoundOver);
			keyBtn[i][j]->SetSoundClick(keySoundClick);
			keyBtn[i][j]->SetTrigger(trigA);
			keyBtn[i][j]->SetLabel(keyTxt[i][j]);
			if(i == 1)
				Pos = 78;
			else if(i > 0)
				Pos = (i+2)*20;
			keyBtn[i][j]->SetPosition(j*42+Pos+KeyboardPosition, i*42+80);
			keyBtn[i][j]->SetEffectGrow();
			keyBtn[i][j]->Clicked.connect(this, &GuiKeyboard::OnNormalKeyPress);

			if(keys[i].ch[j] != 0)
				this->Append(keyBtn[i][j]);
		}
	}
}

/**
 * Destructor for the GuiKeyboardAlone class.
 */
GuiKeyboard::~GuiKeyboard()
{
	//! Stop the external keyboard scan thread
	ExternalKeyboard::Instance()->SuspendScanThread();

	delete keyCapsText;
	delete keyCapsImg;
	delete keyCapsOverImg;
	delete keyCaps;
	delete keyShiftText;
	delete keyShiftImg;
	delete keyShiftOverImg;
	delete keyShift;
	delete keyBackText;
	delete keyBackImg;
	delete keyBackOverImg;
	delete keyBack;
	delete keySpaceImg;
	delete keySpaceOverImg;
	delete keySpace;
	delete keyEnterText;
	delete keyEnterImg;
	delete keyEnterOverImg;
	delete keyEnter;
	delete keyLangText;
	delete keyLangImg;
	delete keyLangOverImg;
	delete keyLang;
	delete keyTabText;
	delete keyTabImg;
	delete keyTabOverImg;
	delete keyTab;
	delete trigA;
	Resources::Remove(key);
	Resources::Remove(keyOver);
	Resources::Remove(keyMedium);
	Resources::Remove(keyMediumOver);
	Resources::Remove(keyLarge);
	Resources::Remove(keyLargeOver);
	Resources::Remove(keySoundOver);
	Resources::Remove(keySoundClick);

	for(int i = 0; i < MAXROWS; i++)
	{
		for(int j = 0; j < MAXKEYS; j++)
		{
			delete keyImg[i][j];
			delete keyImgOver[i][j];
			delete keyTxt[i][j];
			delete keyBtn[i][j];
		}
	}
}

void GuiKeyboard::SwitchKeyLanguage()
{
	DefaultKeys = !DefaultKeys;

	memset(keys, 0, sizeof(keys));

	if(DefaultKeys)
	{
		wcsncpy(keys[0].ch,		L"`1234567890-=", MAXKEYS);
		wcsncpy(keys[0].chShift,L"~!@#$%^&*()_+", MAXKEYS);
		wcsncpy(keys[1].ch,		L"qwertyuiop[]\\", MAXKEYS);
		wcsncpy(keys[1].chShift,L"QWERTYUIOP{}|", MAXKEYS);
		wcsncpy(keys[2].ch,		L"asdfghjkl;'", MAXKEYS);
		wcsncpy(keys[2].chShift,L"ASDFGHJKL:\"", MAXKEYS);
		wcsncpy(keys[3].ch,		L"zxcvbnm,./", MAXKEYS);
		wcsncpy(keys[3].chShift,L"ZXCVBNM<>?", MAXKEYS);
	}
	else
	{
		wcsncpy(keys[0].ch,		wfmt("%s", tr("`1234567890-=")), MAXKEYS);
		wcsncpy(keys[0].chShift,wfmt("%s", tr("~!@#$%^&*()_+")), MAXKEYS);
		wcsncpy(keys[1].ch,		wfmt("%s", tr("qwertyuiop[]\\")), MAXKEYS);
		wcsncpy(keys[1].chShift,wfmt("%s", tr("QWERTYUIOP{}|")), MAXKEYS);
		wcsncpy(keys[2].ch,		wfmt("%s", tr("asdfghjkl;'")), MAXKEYS);
		wcsncpy(keys[2].chShift,wfmt("%s", tr("ASDFGHJKL:\"")), MAXKEYS);
		wcsncpy(keys[3].ch,		wfmt("%s", tr("zxcvbnm,./")), MAXKEYS);
		wcsncpy(keys[3].chShift,wfmt("%s", tr("ZXCVBNM<>?")), MAXKEYS);
	}
}

void GuiKeyboard::OnSpecialKeyPress(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	if(sender == keySpace)
	{
		keyPressed(L' ');
	}
	else if(sender == keyBack)
	{
		keyPressed(0x08);
	}
	else if(sender == keyEnter)
	{
		keyPressed(L'\n');
	}
	else if(sender == keyShift)
	{
		shift = !shift;
		UpdateKeys = true;
	}
	else if(sender == keyCaps)
	{
		caps = !caps;
		UpdateKeys = true;
	}
	else if(sender == keyTab)
	{
		keyPressed(L'\t');
	}
	else if(sender == keyLang)
	{
		SwitchKeyLanguage();
		UpdateKeys = true;
	}
}

void GuiKeyboard::OnNormalKeyPress(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	for(int i = 0; i < MAXROWS; i++)
	{
		for(int j = 0; j < MAXKEYS; j++)
		{
			if(sender == keyBtn[i][j])
			{
				if(shift || caps)
				{
					keyPressed(keys[i].chShift[j]);
				}
				else
				{
					keyPressed(keys[i].ch[j]);
				}

				if(shift)
				{
					shift = false;
					UpdateKeys = true;
				}
			}
		}
	}
}

void GuiKeyboard::Update(GuiTrigger * t)
{
	GuiFrame::Update(t);

	++DeleteDelay;

	if(t->chan == 0) {
		// Update only once every frame (50-60 times per second)
		bool bKeyChangeEvent = ExternalKeyboard::Instance()->getEvent(keyboardEvent);
		if(bKeyChangeEvent) {
			if(keyboardEvent.type == KEYBOARD_PRESSED) {
				keyHeldDelay.reset();
			}
			else {
				// key released -> reset symbol
				keyboardEvent.symbol = 0;
			}
		}
		
		// if the key was not released it -> add it again as "new" event
		if(	   (keyboardEvent.symbol != 0)
			&& (   bKeyChangeEvent
				|| (keyHeldDelay.elapsedMilliSecs() > 500 && DeleteDelay > Settings.KeyboardDeleteDelay)))	// delay hold key
		{
			wchar_t charCode = keyboardEvent.symbol;
			if(charCode != 0) {
				keyPressed(charCode);
				DeleteDelay = 0;
			}
		}
	}

	if(((t->wpad.btns_h & WiiControls.KeyBackspaceButton) ||
		(t->wpad.btns_h & (ClassicControls.KeyBackspaceButton << 16)) ||
		(t->pad.btns_h & GCControls.KeyBackspaceButton)) &&
		DeleteDelay > (u32) Settings.KeyboardDeleteDelay)
	{
		keyPressed(0x08);
		DeleteDelay = 0;
	}

	if((t->wpad.btns_h & WiiControls.KeyShiftButton) ||
	   (t->wpad.btns_h & (ClassicControls.KeyShiftButton << 16)) ||
	   (t->pad.btns_h & GCControls.KeyShiftButton))
	{
		caps = true;

		if(ShiftChan < 0)
			UpdateKeys = true;

		ShiftChan = t->chan;
	}
	else
	{
		if(t->chan == ShiftChan)
		{
			caps = false;
			shift = false;
			UpdateKeys = true;
			ShiftChan = -1;
		}
	}

	if(UpdateKeys)
	{
		wchar_t txt[2] = { 0, 0 };

		for(int i = 0; i < MAXROWS; i++)
		{
			for(int j = 0; j < MAXKEYS; j++)
			{
				if(keys[i].ch[j] != 0)
				{
					this->Append(keyBtn[i][j]);

					if(shift || caps)
						txt[0] = keys[i].chShift[j];
					else
						txt[0] = keys[i].ch[j];

					keyTxt[i][j]->SetText(txt);
				}
				else
				{
					this->Remove(keyBtn[i][j]);
				}
			}
		}
		UpdateKeys = false;
	}
}
