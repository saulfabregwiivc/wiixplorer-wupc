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
#include "SMBSettingsMenu.h"
#include "Prompts/PromptWindows.h"
#include "Controls/Taskbar.h"
#include "DeviceControls/DeviceHandler.hpp"
#include "DeviceControls/RemountTask.h"
#include "network/networkops.h"
#include "Settings.h"

SMBSettingsMenu::SMBSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("SMB Settings"), r)
{
	SetupOptions();
}

SMBSettingsMenu::~SMBSettingsMenu()
{
}

void SMBSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Share:"));
	options.SetName(i++, tr("Host:"));
	options.SetName(i++, tr("Username:"));
	options.SetName(i++, tr("Password:"));
	options.SetName(i++, tr("SMB Name:"));
	options.SetName(i++, tr("Reconnect SMB"));
	options.SetName(i++, tr("Disconnect SMB"));

	SetOptionValues();
}

void SMBSettingsMenu::SetOptionValues()
{
	int i = 0;

	options.SetValue(i++,tr("Share %i"), Settings.CurrentSMBUser+1);

	options.SetValue(i++,"%s", Settings.SMBUser[Settings.CurrentSMBUser].Host);

	options.SetValue(i++,"%s", Settings.SMBUser[Settings.CurrentSMBUser].User);

	if (strcmp(Settings.SMBUser[Settings.CurrentSMBUser].Password, "") != 0)
		options.SetValue(i++,"********");
	else
		options.SetValue(i++," ");

	options.SetValue(i++,"%s", Settings.SMBUser[Settings.CurrentSMBUser].SMBName);

	options.SetValue(i++," ");

	options.SetValue(i++," ");

}

void SMBSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	char entered[300];
	int result = 0;

	switch (option)
	{
		case 0:
			Settings.CurrentSMBUser++;
			if(Settings.CurrentSMBUser >= MAXSMBUSERS)
				Settings.CurrentSMBUser = 0;
			break;
		case 1:
			snprintf(entered, sizeof(entered), "%s", Settings.SMBUser[Settings.CurrentSMBUser].Host);
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				snprintf(Settings.SMBUser[Settings.CurrentSMBUser].Host, sizeof(Settings.SMBUser[Settings.CurrentSMBUser].Host), "%s", entered);
			break;
		case 2:
			snprintf(entered, sizeof(entered), "%s", Settings.SMBUser[Settings.CurrentSMBUser].User);
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				snprintf(Settings.SMBUser[Settings.CurrentSMBUser].User, sizeof(Settings.SMBUser[Settings.CurrentSMBUser].User), "%s", entered);
			break;
		case 3:
			entered[0] = 0;
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				snprintf(Settings.SMBUser[Settings.CurrentSMBUser].Password, sizeof(Settings.SMBUser[Settings.CurrentSMBUser].Password), "%s", entered);
			break;
		case 4:
			snprintf(entered, sizeof(entered), "%s", Settings.SMBUser[Settings.CurrentSMBUser].SMBName);
			result = OnScreenKeyboard(entered, sizeof(entered));
			if(result)
				snprintf(Settings.SMBUser[Settings.CurrentSMBUser].SMBName, sizeof(Settings.SMBUser[Settings.CurrentSMBUser].SMBName), "%s", entered);
			break;
		case 5:
			result = WindowPrompt(tr("Do you want to reconnect this SMB?"),0,tr("OK"),tr("Cancel"));
			if(result)
			{
				RemountTask *mountTask = new RemountTask(tr("Remounting SMB client."), SMB1 + Settings.CurrentSMBUser);
				mountTask->SetAutoDelete(true);
				Taskbar::Instance()->AddTask(mountTask);
				ThreadedTaskHandler::Instance()->AddTask(mountTask);
			}
			break;
		case 6:
			result = WindowPrompt(tr("Do you want to disconnect this SMB?"),0,tr("OK"),tr("Cancel"));
			if(result)
				CloseSMBShare(Settings.CurrentSMBUser);
			break;
		default:
			break;
	}

	SetOptionValues();
}
