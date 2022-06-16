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
#ifndef _PROMPTWINDOW_H_
#define _PROMPTWINDOW_H_

#include "GUI/gui_image.h"
#include "GUI/gui_imagedata.h"
#include "GUI/gui_button.h"
#include "GUI/gui_text.h"
#include "GUI/gui_trigger.h"
#include "GUI/gui_frame.h"

class PromptWindow : public GuiFrame, public sigslot::has_slots<>
{
	public:
		PromptWindow(const char *title, const char *msg = NULL, const char *btn1Label = NULL,
						const char *btn2Label = NULL, const char *btn3Label = NULL,
						const char *btn4Label = NULL);
		virtual ~PromptWindow();
		int GetChoice();
		void SetTitle(const char *title);
		void SetMessage(const char *msg);
		void SetAutoClose(bool a) { AutoClose = a; }
		sigslot::signal2<PromptWindow *, int> ButtonClicked;
	protected:
		void OnButtonClick(GuiButton *sender, int pointer, const POINT &p);

		int choice;
		bool AutoClose;

		GuiImage * btn1Img;
		GuiImage * btn2Img;
		GuiImage * btn3Img;
		GuiImage * btn4Img;
		GuiImage * bgWindowImg;

		GuiImageData * btnOutline;
		GuiImageData * bgWindow;

		GuiSound * btnClick;
		GuiSound * btnSoundOver;

		GuiText * titleTxt;
		GuiText * msgTxt;
		GuiText * btn1Txt;
		GuiText * btn2Txt;
		GuiText * btn3Txt;
		GuiText * btn4Txt;

		GuiButton * btn1;
		GuiButton * btn2;
		GuiButton * btn3;
		GuiButton * btn4;

		GuiTrigger * trigA;
		GuiTrigger * trigB;
};

#endif
