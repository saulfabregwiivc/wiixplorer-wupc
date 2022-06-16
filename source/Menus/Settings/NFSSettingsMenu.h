/****************************************************************************
 * Copyright (C) 2009-2011 Dimok, r-win 
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
#ifndef NFS_SETTINGS_MENU_H_
#define NFS_SETTINGS_MENU_H_

#include "SettingsMenu.h"

class NFSSettingsMenu : public SettingsMenu
{
	public:
		NFSSettingsMenu(GuiFrame *returnElement);
		virtual ~NFSSettingsMenu();
	protected:
		void SetupOptions();
		void SetOptionValues();
		virtual void OnOptionClick(GuiOptionBrowser *sender, int option);
};



#endif
