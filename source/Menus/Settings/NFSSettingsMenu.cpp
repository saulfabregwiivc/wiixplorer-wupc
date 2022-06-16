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
#include "NFSSettingsMenu.h"
#include "Prompts/PromptWindows.h"
#include "Controls/Taskbar.h"
#include "DeviceControls/DeviceHandler.hpp"
#include "DeviceControls/RemountTask.h"
#include "network/networkops.h"
#include "Settings.h"

NFSSettingsMenu::NFSSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("NFS Settings"), r)
{
	SetupOptions();
}

NFSSettingsMenu::~NFSSettingsMenu()
{
}

void NFSSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Share:"));
	options.SetName(i++, tr("Host:"));
	options.SetName(i++, tr("Mountpoint:"));
	options.SetName(i++, tr("Reconnect NFS"));
	options.SetName(i++, tr("Disconnect NFS"));

	SetOptionValues();
}

void NFSSettingsMenu::SetOptionValues()
{
	int i = 0;

	options.SetValue(i++,tr("Mount %i"), Settings.CurrentNFSUser+1);
	options.SetValue(i++,"%s", Settings.NFSUser[Settings.CurrentNFSUser].Host);
	options.SetValue(i++,"%s", Settings.NFSUser[Settings.CurrentNFSUser].Mountpoint);
	options.SetValue(i++, " ");
	options.SetValue(i++, " ");
}

void NFSSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	char entered[300];
	int result = 0;

	switch (option)
	{
		case 0:
			Settings.CurrentNFSUser++;
			if(Settings.CurrentNFSUser >= MAXNFSUSERS)
				Settings.CurrentNFSUser = 0;
			break;
		case 1:
			snprintf(entered, sizeof(entered), "%s", Settings.NFSUser[Settings.CurrentNFSUser].Host);
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				snprintf(Settings.NFSUser[Settings.CurrentNFSUser].Host, sizeof(Settings.NFSUser[Settings.CurrentNFSUser].Host), "%s", entered);
			break;
		case 2:
			snprintf(entered, sizeof(entered), "%s", Settings.NFSUser[Settings.CurrentNFSUser].Mountpoint);
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				snprintf(Settings.NFSUser[Settings.CurrentNFSUser].Mountpoint, sizeof(Settings.NFSUser[Settings.CurrentNFSUser].Mountpoint), "%s", entered);
			break;
		case 3:
			result = WindowPrompt(tr("Do you want to reconnect this NFS?"),0,tr("OK"),tr("Cancel"));
			if(result)
			{
				RemountTask *mountTask = new RemountTask(tr("Remounting NFS client."), NFS1 + Settings.CurrentNFSUser);
				mountTask->SetAutoDelete(true);
				Taskbar::Instance()->AddTask(mountTask);
				ThreadedTaskHandler::Instance()->AddTask(mountTask);
			}
		break;

		case 4:
			result = WindowPrompt(tr("Do you want to disconnect this NFS?"),0,tr("OK"),tr("Cancel"));
			if(result)
				CloseNFS(Settings.CurrentNFSUser);
			break;
		default:
			break;
	}

	SetOptionValues();
}
