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
#include "FTPServerSettingsMenu.h"
#include "Prompts/PromptWindows.h"
#include "network/networkops.h"
#include "Settings.h"

FTPServerSettingsMenu::FTPServerSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("FTP Server Settings"), r)
{
	SetupOptions();
}

FTPServerSettingsMenu::~FTPServerSettingsMenu()
{
}

void FTPServerSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Auto Start:"));
	options.SetName(i++, tr("Password:"));
	options.SetName(i++, tr("FTP Port:"));

	SetOptionValues();
}

void FTPServerSettingsMenu::SetOptionValues()
{
	int i = 0;

	if(Settings.FTPServer.AutoStart)
		options.SetValue(i++, tr("ON"));
	else
		options.SetValue(i++, tr("OFF"));

	if (strcmp(Settings.FTPServer.Password, "") != 0)
		options.SetValue(i++,"********");
	else
		options.SetValue(i++," ");

	options.SetValue(i++,"%i", Settings.FTPServer.Port);

}

void FTPServerSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	char entered[300];
	int result = 0;

	switch (option)
	{
		case 0:
			Settings.FTPServer.AutoStart = (Settings.FTPServer.AutoStart+1) % 2;
			break;
		case 1:
			entered[0] = 0;
			result = OnScreenKeyboard(entered, 149);
			if(result)
				snprintf(Settings.FTPServer.Password, sizeof(Settings.FTPServer.Password), "%s", entered);
			break;
		case 2:
			snprintf(entered, sizeof(entered), "%d", Settings.FTPServer.Port);
			result = OnScreenKeyboard(entered, 149);
			if(result)
				Settings.FTPServer.Port = (u16) atoi(entered);
			break;
		default:
			break;
	}

	SetOptionValues();
}
