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
#ifndef SETTINGS_MENU_H_
#define SETTINGS_MENU_H_

#include "GUI/gui.h"
#include "GUI/gui_optionbrowser.h"

class SettingsMenu : public GuiFrame, public sigslot::has_slots<>
{
	public:
		SettingsMenu(const char * title, GuiFrame *returnElement);
		virtual ~SettingsMenu();
		virtual void show();
		virtual void hide();
	protected:
		virtual void OnBackButtonClick(GuiButton *sender, int pointer, const POINT &p);
		virtual void OnOptionClick(GuiOptionBrowser *sender UNUSED, int option UNUSED) = 0;
		virtual void CloseMenu();
		virtual void SetupOptions() = 0;
		virtual void SetOptionValues() = 0;
		void OnEffectFinish(GuiElement *e UNUSED);

		GuiFrame *returnFrame;

		GuiSound * btnSoundClick;
		GuiSound * btnSoundOver;

		GuiImageData * btnOutline;
		GuiImageData * btnOutlineOver;

		SimpleGuiTrigger * trigA;
		GuiTrigger * trigB;

		GuiText * titleTxt;
		GuiText * backBtnTxt;
		GuiImage * backBtnImg;
		GuiButton * backBtn;

		GuiOptionBrowser * optionBrowser;

		OptionList options;
};



#endif
