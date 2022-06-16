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
#ifndef _KEYBOARDWINDOW_H_
#define _KEYBOARDWINDOW_H_

#include "GUI/gui_keyboard.h"
#include "TextOperations/TextPointer.h"
#include "TextOperations/wstring.hpp"

#define MAX_KEYBOARD_DISPLAY	40

//!On-screen keyboard
class KeyboardWindow : public GuiKeyboard
{
	public:
		KeyboardWindow(wchar_t * t, u32 max);
		virtual ~KeyboardWindow();
		int GetChoice(void) { return choice; }
		const wchar_t * GetString();
		std::string GetUTF8String() const;
		void AddChar(int pos, wchar_t Char);
		void RemoveChar(int pos);
		sigslot::signal2<int, wchar_t *> ButtonClicked;
	protected:
		void MoveText(int n);
		const wchar_t * GetDisplayText();
		void OnPointerHeld(GuiButton *sender, int pointer, const POINT &p);
		void OnPositionMoved(GuiButton *sender, int pointer, const POINT &p);
		void OnClearKeyPress(GuiButton *sender, int pointer, const POINT &p);
		void OnButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnKeyPress(wchar_t charCode);

		int CurrentFirstLetter;
		wString kbtextstr;
		wchar_t displayTxt[MAX_KEYBOARD_DISPLAY];
		u32 kbtextmaxlen;
		int choice;
		wchar_t *inText;

		TextPointer * TextPointerBtn;
		GuiButton * GoLeft;
		GuiButton * GoRight;
		GuiText * kbText;
		GuiImage * keyTextboxImg;
		GuiText * keyClearText;
		GuiImage * keyClearImg;
		GuiImage * keyClearOverImg;
		GuiButton * keyClear;
		GuiImageData * keyTextbox;
		GuiImageData * btnOutline;
		GuiImageData * btnOutlineOver;
		GuiTrigger * trigHeldA;
		GuiTrigger * trigLeft;
		GuiTrigger * trigRight;
		GuiButton * okBtn;
		GuiButton * cancelBtn;
		GuiImage * okBtnImg;
		GuiImage * okBtnImgOver;
		GuiImage * cancelBtnImg;
		GuiImage * cancelBtnImgOver;
		GuiText * okBtnTxt;
		GuiText * cancelBtnTxt;
};

#endif
