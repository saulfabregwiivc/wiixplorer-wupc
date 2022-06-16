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
#ifndef GUI_KEYBOARD_H_
#define GUI_KEYBOARD_H_

#include "gui_frame.h"
#include "gui_button.h"
#include "gui_text.h"
#include "gui_image.h"
#include "TextOperations/TextPointer.h"
#include "TextOperations/wstring.hpp"
#include "Controls/ExternalKeyboard.h"

#define MAXKEYS					14
#define MAXROWS					4

//!On-screen keyboard
class GuiKeyboard : public GuiFrame, public sigslot::has_slots<>
{
	public:
		GuiKeyboard();
		virtual ~GuiKeyboard();
		void Update(GuiTrigger * t);

		sigslot::signal1<wchar_t> keyPressed;
	protected:
		void SwitchKeyLanguage();
		void OnSpecialKeyPress(GuiButton *sender, int pointer, const POINT &p);
		void OnNormalKeyPress(GuiButton *sender, int pointer, const POINT &p);

		typedef struct _keyrowtype
		{
			wchar_t ch[MAXKEYS];
			wchar_t chShift[MAXKEYS];
		} KeyboardRow;

		int DeleteDelay;
		Timer keyHeldDelay;
		keyboard_event keyboardEvent;
		KeyboardRow keys[MAXROWS];
		int ShiftChan;
		bool shift;
		bool caps;
		bool UpdateKeys;
		bool DefaultKeys;

		GuiText * keyCapsText;
		GuiImage * keyCapsImg;
		GuiImage * keyCapsOverImg;
		GuiButton * keyCaps;
		GuiText * keyShiftText;
		GuiImage * keyShiftImg;
		GuiImage * keyShiftOverImg;
		GuiButton * keyShift;
		GuiText * keyBackText;
		GuiImage * keyBackImg;
		GuiImage * keyBackOverImg;
		GuiButton * keyBack;
		GuiImage * keySpaceImg;
		GuiImage * keySpaceOverImg;
		GuiButton * keySpace;
		GuiText * keyEnterText;
		GuiImage * keyEnterImg;
		GuiImage * keyEnterOverImg;
		GuiButton * keyEnter;
		GuiText * keyTabText;
		GuiImage * keyTabImg;
		GuiImage * keyTabOverImg;
		GuiButton * keyTab;
		GuiText * keyLangText;
		GuiImage * keyLangImg;
		GuiImage * keyLangOverImg;
		GuiButton * keyLang;
		GuiButton * keyBtn[MAXROWS][MAXKEYS];
		GuiImage * keyImg[MAXROWS][MAXKEYS];
		GuiImage * keyImgOver[MAXROWS][MAXKEYS];
		GuiText * keyTxt[MAXROWS][MAXKEYS];
		GuiImageData * key;
		GuiImageData * keyOver;
		GuiImageData * keyMedium;
		GuiImageData * keyMediumOver;
		GuiImageData * keyLarge;
		GuiImageData * keyLargeOver;
		GuiSound * keySoundOver;
		GuiSound * keySoundClick;
		GuiTrigger * trigA;
};

#endif
