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
#include "KeyboardWindow.h"
#include "Memory/Resources.h"

/**
 * Constructor for the GuiKeyboard class.
 */
KeyboardWindow::KeyboardWindow(wchar_t * t, u32 max)
{
	choice = -1;
	inText = t;
	selectable = true;
	kbtextstr.assign(t);
	kbtextmaxlen = max > 0 ? max : wcslen(t)+1;
	CurrentFirstLetter = 0;
	if(t)
	{
		CurrentFirstLetter = wcslen(t)-MAX_KEYBOARD_DISPLAY+1;
		if(CurrentFirstLetter < 0)
			CurrentFirstLetter = 0;
	}

	height += 100;

	btnOutline = Resources::GetImageData("button.png");
	btnOutlineOver = Resources::GetImageData("button_over.png");

	keyTextbox = Resources::GetImageData("keyboard_textbox.png");
	keyTextboxImg = new GuiImage(keyTextbox);
	keyTextboxImg->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	keyTextboxImg->SetPosition(0, 0);
	this->Append(keyTextboxImg);

	trigHeldA = new GuiTrigger;
	trigHeldA->SetHeldTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	kbText = new GuiText(GetDisplayText(), 20, (GXColor){0, 0, 0, 0xff});
	kbText->SetAlignment(ALIGN_LEFT | ALIGN_TOP);

	TextPointerBtn = new TextPointer(kbText, 0);
	TextPointerBtn->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	TextPointerBtn->SetPosition(0, 11);
	TextPointerBtn->SetHoldable(true);
	TextPointerBtn->SetTrigger(trigHeldA);
	TextPointerBtn->Held.connect(this, &KeyboardWindow::OnPointerHeld);
	this->Append(TextPointerBtn);

	TextPointerBtn->PositionChanged(0, 0, 0);
	TextPointerBtn->SetLetterPosition(MAX_KEYBOARD_DISPLAY-1);

	trigLeft = new GuiTrigger;
	trigLeft->SetButtonOnlyTrigger(-1, WiiControls.LeftButton | ClassicControls.LeftButton << 16, GCControls.LeftButton);
	trigRight = new GuiTrigger;
	trigRight->SetButtonOnlyTrigger(-1, WiiControls.RightButton | ClassicControls.RightButton << 16, GCControls.RightButton);

	GoRight = new GuiButton(1, 1);
	GoRight->SetSoundClick(keySoundClick);
	GoRight->SetTrigger(trigRight);
	GoRight->Clicked.connect(this, &KeyboardWindow::OnPositionMoved);
	this->Append(GoRight);

	GoLeft = new GuiButton(1, 1);
	GoLeft->SetSoundClick(keySoundClick);
	GoLeft->SetTrigger(trigLeft);
	GoLeft->Clicked.connect(this, &KeyboardWindow::OnPositionMoved);
	this->Append(GoLeft);

	keyClearText = new GuiText(tr("Clear"), 20, (GXColor){0, 0, 0, 0xff});
	keyClearImg = new GuiImage(keyMedium);
	keyClearOverImg = new GuiImage(keyMediumOver);
	keyClear = new GuiButton(keyMedium->GetWidth(), keyMedium->GetHeight());
	keyClear->SetImage(keyClearImg);
	keyClear->SetImageOver(keyClearOverImg);
	keyClear->SetSoundOver(keySoundOver);
	keyClear->SetSoundClick(keySoundClick);
	keyClear->SetLabel(keyClearText);
	keyClear->SetTrigger(trigA);
	keyClear->SetPosition(78, 4*42+80);
	keyClear->SetEffectGrow();
	keyClear->Clicked.connect(this, &KeyboardWindow::OnClearKeyPress);
	this->Append(keyClear);

	okBtnTxt = new GuiText(tr("OK"), 22, (GXColor){0, 0, 0, 255});
	okBtnImg = new GuiImage(btnOutline);
	okBtnImgOver = new GuiImage(btnOutlineOver);
	okBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	okBtn->SetAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
	okBtn->SetPosition(25, -25);
	okBtn->SetLabel(okBtnTxt);
	okBtn->SetImage(okBtnImg);
	okBtn->SetImageOver(okBtnImgOver);
	okBtn->SetSoundOver(keySoundOver);
	okBtn->SetTrigger(trigA);
	okBtn->SetEffectGrow();
	okBtn->Clicked.connect(this, &KeyboardWindow::OnButtonClick);
	this->Append(okBtn);

	cancelBtnTxt = new GuiText(tr("Cancel"), 22, (GXColor){0, 0, 0, 255});
	cancelBtnImg = new GuiImage(btnOutline);
	cancelBtnImgOver = new GuiImage(btnOutlineOver);
	cancelBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	cancelBtn->SetAlignment(ALIGN_RIGHT | ALIGN_BOTTOM);
	cancelBtn->SetPosition(-25, -25);
	cancelBtn->SetLabel(cancelBtnTxt);
	cancelBtn->SetImage(cancelBtnImg);
	cancelBtn->SetImageOver(cancelBtnImgOver);
	cancelBtn->SetSoundOver(keySoundOver);
	cancelBtn->SetTrigger(trigA);
	cancelBtn->SetEffectGrow();
	cancelBtn->Clicked.connect(this, &KeyboardWindow::OnButtonClick);
	this->Append(cancelBtn);

	this->keyPressed.connect(this, &KeyboardWindow::OnKeyPress);
}

/**
 * Destructor for the GuiKeyboard class.
 */
KeyboardWindow::~KeyboardWindow()
{
	RemoveAll();

	delete TextPointerBtn;
	delete GoRight;
	delete GoLeft;
	delete kbText;
	delete keyTextboxImg;
	delete keyClearText;
	delete keyClearImg;
	delete keyClearOverImg;
	delete keyClear;
	delete trigHeldA;
	delete trigLeft;
	delete trigRight;
	delete okBtn;
	delete cancelBtn;
	delete okBtnImg;
	delete okBtnImgOver;
	delete cancelBtnImg;
	delete cancelBtnImgOver;
	delete okBtnTxt;
	delete cancelBtnTxt;
	Resources::Remove(keyTextbox);
	Resources::Remove(btnOutline);
	Resources::Remove(btnOutlineOver);
}

void KeyboardWindow::OnButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	if(sender == okBtn)
	{
		choice = 1;
		wcsncpy(inText, this->GetString(), kbtextmaxlen);
		inText[kbtextmaxlen-1] = 0;
	}
	else if(sender == cancelBtn)
	{
		choice = 0;
	}

	ButtonClicked(choice, inText);
}

std::string KeyboardWindow::GetUTF8String() const
{
	return kbtextstr.toUTF8();
}

const wchar_t * KeyboardWindow::GetString()
{
	return kbtextstr.c_str();
}

void KeyboardWindow::AddChar(int pos, wchar_t Char)
{
	if(pos < 0 || pos > (int) kbtextstr.size())
		return;

	kbtextstr.insert(pos, 1, Char);
}

void KeyboardWindow::RemoveChar(int pos)
{
	if (pos < 0 || pos >= (int) kbtextstr.size())
		return;

	kbtextstr.erase(pos, 1);
}

void KeyboardWindow::MoveText(int n)
{
	POINT p = { 0, 0 };
	GuiButton *b = (n < 0) ? GoLeft : GoRight;

	n = labs(n);

	for(int i = 0; i < n; i++)
		OnPositionMoved(b, 0, p);
}

void KeyboardWindow::OnPointerHeld(GuiButton *sender UNUSED, int pointer, const POINT &p)
{
	TextPointerBtn->PositionChanged(pointer, p.x - TextPointerBtn->GetLeft(), p.y - TextPointerBtn->GetTop());
}

void KeyboardWindow::OnPositionMoved(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	sender->ResetState();

	if(sender == GoLeft)
	{
		int currentPointLetter = TextPointerBtn->GetCurrentLetter();
		currentPointLetter--;
		if(currentPointLetter < 0)
		{
			currentPointLetter = 0;
			CurrentFirstLetter--;
			if(CurrentFirstLetter < 0)
				CurrentFirstLetter = 0;
		}

		if(kbtextstr.length() < MAX_KEYBOARD_DISPLAY) {
			currentPointLetter += CurrentFirstLetter;
			CurrentFirstLetter = 0;
		}

		kbText->SetText(GetDisplayText());
		TextPointerBtn->UpdateWidth();
		TextPointerBtn->SetLetterPosition(currentPointLetter);
	}
	else if(sender == GoRight)
	{
		int currentPointLetter = TextPointerBtn->GetCurrentLetter();
		currentPointLetter++;
		int strlength = kbtextstr.length();

		if(currentPointLetter > strlength) {
			currentPointLetter = strlength;
		}
		else if(currentPointLetter > (MAX_KEYBOARD_DISPLAY-1))
		{
			currentPointLetter--;
			CurrentFirstLetter++;

			if(CurrentFirstLetter > (strlength-MAX_KEYBOARD_DISPLAY+1))
				CurrentFirstLetter = strlength-MAX_KEYBOARD_DISPLAY+1;
		}

		kbText->SetText(GetDisplayText());
		TextPointerBtn->UpdateWidth();
		TextPointerBtn->SetLetterPosition(currentPointLetter);
	}
}

void KeyboardWindow::OnClearKeyPress(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	if(sender == keyClear)
	{
		CurrentFirstLetter = 0;
		kbtextstr.clear();
		kbText->SetText(GetDisplayText());
		TextPointerBtn->UpdateWidth();
		TextPointerBtn->SetLetterPosition(0);
	}
}

void KeyboardWindow::OnKeyPress(wchar_t charCode)
{
	//! for usb keyboard switch '\r' to '\n'
	if(charCode == L'\r')
		charCode = L'\n';

	else if(((charCode >> 8) == 0xF2) && (charCode & 0xFF) < 0x80) {
		// this is usually a numpad
		charCode = charCode & 0xFF;
	}
	else if(charCode == KS_Left || charCode == KS_KP_Left) {
		//! control character LEFT
		POINT p = { 0, 0 };
		OnPositionMoved(GoLeft, 0, p);
	}
	else if(charCode == KS_Right || charCode == KS_KP_Right) {
		//! control character RIGHT
		POINT p = { 0, 0 };
		OnPositionMoved(GoRight, 0, p);
	}
	else if(charCode == KS_Home || charCode == KS_KP_Home) {
		//! control character HOME
		CurrentFirstLetter = 0;
		kbText->SetText(GetDisplayText());
		TextPointerBtn->UpdateWidth();
		TextPointerBtn->SetLetterPosition(0);
	}
	else if(charCode == KS_End || charCode == KS_KP_End) {
		//! control character END
		CurrentFirstLetter = kbtextstr.length()-MAX_KEYBOARD_DISPLAY+1;
		if(CurrentFirstLetter < 0)
			CurrentFirstLetter = 0;
		kbText->SetText(GetDisplayText());
		TextPointerBtn->UpdateWidth();
		TextPointerBtn->SetLetterPosition(0xFFFF);
	}
	else if(((charCode >> 8) == 0xF2) && (charCode & 0xFF) < 0x80) {
		// this is usually a numpad
		charCode = charCode & 0xFF;
	}
	else if(charCode >= 0xD800) {
		//! skip unknown characters
		return;
	}
	else if(charCode == 0x08)
	{
		RemoveChar(CurrentFirstLetter+TextPointerBtn->GetCurrentLetter()-1);
		MoveText(-1);
		// show what we are deleting
		int currentPointLetter = TextPointerBtn->GetCurrentLetter();
		while(CurrentFirstLetter > 0 && currentPointLetter < 5) {
			CurrentFirstLetter--;
			currentPointLetter++;
		}
		kbText->SetText(GetDisplayText());
		TextPointerBtn->UpdateWidth();
		TextPointerBtn->SetLetterPosition(currentPointLetter);
	}
	else if(charCode == 0x7F)
	{
		RemoveChar(CurrentFirstLetter+TextPointerBtn->GetCurrentLetter());
		int currentPointLetter = TextPointerBtn->GetCurrentLetter();
		if(kbtextstr.length() < MAX_KEYBOARD_DISPLAY) {
			currentPointLetter += CurrentFirstLetter;
			CurrentFirstLetter = 0;
		}
		kbText->SetText(GetDisplayText());
		TextPointerBtn->UpdateWidth();
		TextPointerBtn->SetLetterPosition(TextPointerBtn->GetCurrentLetter());
	}
	else if(kbtextstr.length() < kbtextmaxlen)
	{
		AddChar(CurrentFirstLetter+TextPointerBtn->GetCurrentLetter(), charCode);
		MoveText(1);
	}
}

const wchar_t * KeyboardWindow::GetDisplayText()
{
	int len = kbtextstr.size();

	if(len < MAX_KEYBOARD_DISPLAY)
		return kbtextstr.c_str();

	int n = 0;
	int startPos = CurrentFirstLetter;
	if(startPos < 0)
		startPos = 0;

	for(int i = startPos; n < (MAX_KEYBOARD_DISPLAY - 1) && i < len; ++i)
	{
		displayTxt[n] = kbtextstr.at(i);
		++n;
	}

	displayTxt[n] = 0;

	return displayTxt;
}
