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
#ifndef GUI_ARROWOPTION_H_
#define GUI_ARROWOPTION_H_

#include "GUI/gui_frame.h"
#include "GUI/gui_button.h"
#include "GUI/gui_sound.h"

using namespace std;

class GuiArrowOption : public GuiFrame, public sigslot::has_slots<>
{
	public:
		GuiArrowOption();
		virtual ~GuiArrowOption();
		void AddOption(const char * name, int PositionX, int PositionY);
		void SetOptionName(int i, const char * text);
		void SetOptionValue(int i, const char * text);
		int GetOptionCount();
		void RemoveOption(int i);
		GuiButton * GetButton(int i);
		GuiButton * GetButtonLeft(int i);
		GuiButton * GetButtonRight(int i);
		void ClearList();

		sigslot::signal3<GuiElement *, int, const POINT &> ClickedLeft;
		sigslot::signal3<GuiElement *, int, const POINT &> ClickedRight;
		sigslot::signal3<GuiElement *, int, const POINT &> ClickedButton;
	protected:
		void OnButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnRightButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnLeftButtonClick(GuiButton *sender, int pointer, const POINT &p);

		GuiSound * btnClick;
		GuiSound * btnSoundOver;
		GuiImageData * ArrowImgData;

		vector<GuiText *> OptionsName;
		vector<GuiText *> OptionsText;
		vector<GuiButton *> OptionsBtn;
		vector<GuiButton *> OptionsBtnRight;
		vector<GuiButton *> OptionsBtnLeft;
		vector<GuiImage *> OptionsImgLeft;
		vector<GuiImage *> OptionsImgRight;

		SimpleGuiTrigger * trigA;
};

#endif
