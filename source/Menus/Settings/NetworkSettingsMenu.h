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
#ifndef NETWORK_SETTINGS_MENU_H_
#define NETWORK_SETTINGS_MENU_H_

#include "SettingsMenu.h"

class NetworkSettingsMenu : public SettingsMenu
{
	public:
		NetworkSettingsMenu(GuiFrame *returnElement);
		virtual ~NetworkSettingsMenu();
	protected:
		void SetupOptions();
		void SetOptionValues();
		void OnOptionClick(GuiOptionBrowser *sender, int option);
		void OnUpdateButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED);

		GuiText *updateBtnTxt;
		GuiImage *updateBtnImg;
		GuiButton *updateBtn;
};



#endif
