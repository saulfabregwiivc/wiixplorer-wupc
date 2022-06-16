/****************************************************************************
 * Copyright (C) 2009-2011 Dimok
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
#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include "GUI/gui.h"
#include "GUI/gui_window.h"
#include "GUI/gui_longtext.hpp"
#include "GUI/gui_keyboard.h"
#include "Controls/Scrollbar.hpp"
#include "Controls/HorizontalScrollbar.hpp"
#include "TextPointer.h"

//!Display a list of files
class TextEditor : public GuiWindow, public sigslot::has_slots<>
{
	public:
		TextEditor(const std::string &filepath);
		virtual ~TextEditor();
		static void LoadFile(const char *filepath);
		void SetText(const wchar_t *intext);
		void WriteTextFile(const std::string &path);
		void Update(GuiTrigger * t);
	protected:
		void OnListChange(int selItem, int selIndex);
		void OnHorScrollChange(int selItem);
		void OnButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnPointerHeld(GuiButton *sender, int pointer, const POINT &p);
		void OnKeyboardEffectFinished(GuiElement *e);
		void OnKeyboardKeyPressed(wchar_t charCode);
		void OnFinishedFileLoad(u8 *buffer, u32 bufferSize);

		bool ExitEditor;
		bool FileEdited;
		int linestodraw;
		int displayLineWidth;
		int textStartWidth;
		u32 filesize;
		std::string filepath;

		Scrollbar * scrollbar;
		HorizontalScrollbar * horScrollbar;
		GuiFrameImage *textBgImg;
		GuiKeyboard *keyboard;

		/** Buttons **/
		GuiButton * maximizeBtn;
		GuiButton * minimizeBtn;
		GuiButton * closeBtn;
		GuiButton * PlusBtn;
		TextPointer * TextPointerBtn;

		/** Images **/
		GuiImage * closeImg;
		GuiImage * closeImgOver;
		GuiImage * maximizeImg;
		GuiImage * minimizeImg;

		/** ImageDatas **/
		GuiImageData * closeImgData;
		GuiImageData * closeImgOverData;
		GuiImageData * maximizeImgData;
		GuiImageData * minimizeImgData;

		/** Sounds **/
		GuiSound * btnSoundOver;
		GuiSound * btnSoundClick;

		/** Triggers **/
		GuiTrigger * trigA;
		GuiTrigger * trigHeldA;
		GuiTrigger * trigPlus;
		GuiTrigger * trigB;

		/** Texts **/
		GuiText * filenameTxt;
		GuiLongText * MainFileTxt;
};

#endif
