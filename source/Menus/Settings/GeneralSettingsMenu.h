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
#ifndef GENERAL_SETTINGS_MENU_H_
#define GENERAL_SETTINGS_MENU_H_

#include "SettingsMenu.h"

class GeneralSettingsMenu : public SettingsMenu
{
	public:
		GeneralSettingsMenu(GuiFrame *returnElement);
		virtual ~GeneralSettingsMenu();
	protected:
		void SetupOptions();
		void SetOptionValues();
		virtual void OnOptionClick(GuiOptionBrowser *sender, int option);
};



#endif
