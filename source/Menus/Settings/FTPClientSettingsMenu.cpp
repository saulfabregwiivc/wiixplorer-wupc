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
#include "FTPClientSettingsMenu.h"
#include "Prompts/PromptWindows.h"
#include "Controls/Taskbar.h"
#include "DeviceControls/DeviceHandler.hpp"
#include "DeviceControls/RemountTask.h"
#include "network/networkops.h"
#include "Settings.h"

FTPClientSettingsMenu::FTPClientSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("FTP Client Settings"), r)
{
	SetupOptions();
}

FTPClientSettingsMenu::~FTPClientSettingsMenu()
{
}

void FTPClientSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Client:"));
	options.SetName(i++, tr("Host:"));
	options.SetName(i++, tr("Username:"));
	options.SetName(i++, tr("Password:"));
	options.SetName(i++, tr("FTP Port:"));
	options.SetName(i++, tr("FTP Path:"));
	options.SetName(i++, tr("Passive Mode:"));
	options.SetName(i++, tr("Reconnect FTP"));
	options.SetName(i++, tr("Disconnect Client"));

	SetOptionValues();
}

void FTPClientSettingsMenu::SetOptionValues()
{
	int i = 0;

	options.SetValue(i++,tr("Client %i"), Settings.CurrentFTPUser+1);

	options.SetValue(i++,"%s", Settings.FTPUser[Settings.CurrentFTPUser].Host);

	options.SetValue(i++,"%s", Settings.FTPUser[Settings.CurrentFTPUser].User);

	if (strcmp(Settings.FTPUser[Settings.CurrentFTPUser].Password, "") != 0)
		options.SetValue(i++,"********");
	else
		options.SetValue(i++," ");

	options.SetValue(i++,"%i", Settings.FTPUser[Settings.CurrentFTPUser].Port);

	options.SetValue(i++,"%s", Settings.FTPUser[Settings.CurrentFTPUser].FTPPath);

	if (Settings.FTPUser[Settings.CurrentFTPUser].Passive == 1)
		options.SetValue(i++,tr("ON"));
	else
		options.SetValue(i++,tr("OFF"));

	options.SetValue(i++," ");

	options.SetValue(i++," ");

}

void FTPClientSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	char entered[300];
	int result = 0;

	switch (option)
	{
		case 0:
			Settings.CurrentFTPUser++;
			if(Settings.CurrentFTPUser >= MAXFTPUSERS)
				Settings.CurrentFTPUser = 0;
			break;
		case 1:
			snprintf(entered, sizeof(entered), "%s", Settings.FTPUser[Settings.CurrentFTPUser].Host);
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				snprintf(Settings.FTPUser[Settings.CurrentFTPUser].Host, sizeof(Settings.FTPUser[Settings.CurrentFTPUser].Host), "%s", entered);
			break;
		case 2:
			snprintf(entered, sizeof(entered), "%s", Settings.FTPUser[Settings.CurrentFTPUser].User);
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				snprintf(Settings.FTPUser[Settings.CurrentFTPUser].User, sizeof(Settings.FTPUser[Settings.CurrentFTPUser].User), "%s", entered);
			break;
		case 3:
			entered[0] = 0;
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				snprintf(Settings.FTPUser[Settings.CurrentFTPUser].Password, sizeof(Settings.FTPUser[Settings.CurrentFTPUser].Password), "%s", entered);
			break;
		case 4:
			snprintf(entered, sizeof(entered), "%i", Settings.FTPUser[Settings.CurrentFTPUser].Port);
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				Settings.FTPUser[Settings.CurrentFTPUser].Port = (u16) atoi(entered);
			break;
		case 5:
			snprintf(entered, sizeof(entered), "%s", Settings.FTPUser[Settings.CurrentFTPUser].FTPPath);
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				snprintf(Settings.FTPUser[Settings.CurrentFTPUser].FTPPath, sizeof(Settings.FTPUser[Settings.CurrentFTPUser].FTPPath), "%s", entered);
			break;
		case 6:
			Settings.FTPUser[Settings.CurrentFTPUser].Passive++;
			if(Settings.FTPUser[Settings.CurrentFTPUser].Passive > 1)
				Settings.FTPUser[Settings.CurrentFTPUser].Passive = 0;
			break;
		case 7:
			result = WindowPrompt(tr("Do you want to reconnect to the FTP client?"),0,tr("OK"),tr("Cancel"));
			if(result)
			{
				RemountTask *mountTask = new RemountTask(tr("Remounting FTP client."), FTP1 + Settings.CurrentFTPUser);
				mountTask->SetAutoDelete(true);
				Taskbar::Instance()->AddTask(mountTask);
				ThreadedTaskHandler::Instance()->AddTask(mountTask);
			}
			break;
		case 8:
			result = WindowPrompt(tr("Do you want to disconnect the FTP client?"),0,tr("OK"),tr("Cancel"));
			if(result)
				CloseFTP(Settings.CurrentFTPUser);
			break;
		default:
			break;
	}

	SetOptionValues();
}
