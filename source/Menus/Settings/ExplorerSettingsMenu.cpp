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
#include "ExplorerSettingsMenu.h"
#include "Settings.h"

ExplorerSettingsMenu::ExplorerSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("Explorer Settings"), r)
{
	SetupOptions();
}

ExplorerSettingsMenu::~ExplorerSettingsMenu()
{
}

void ExplorerSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Browser Mode"));
	options.SetName(i++, tr("Hide System Files/Folders"));

	SetOptionValues();
}

void ExplorerSettingsMenu::SetOptionValues()
{
	int i = 0;

	if (Settings.BrowserMode == ICONBROWSER)
		options.SetValue(i++, tr("Icon Mode"));
	else
		options.SetValue(i++, tr("List Mode"));

	if(Settings.HideSystemFiles)
		options.SetValue(i++, tr("ON"));
	else
		options.SetValue(i++, tr("OFF"));
}

void ExplorerSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	switch (option)
	{
		case 0:
			Settings.BrowserMode = (Settings.BrowserMode+1) % 2;
			break;
		case 1:
			Settings.HideSystemFiles = (Settings.HideSystemFiles+1) % 2;
			break;
		default:
			break;
	}

	SetOptionValues();
}
