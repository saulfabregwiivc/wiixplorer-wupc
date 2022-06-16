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
#include "NetworkSettingsMenu.h"
#include "FTPServerSettingsMenu.h"
#include "FTPClientSettingsMenu.h"
#include "SMBSettingsMenu.h"
#include "NFSSettingsMenu.h"
#include "Controls/Application.h"
#include "Controls/Taskbar.h"
#include "Controls/ThreadedTaskHandler.hpp"
#include "Prompts/PromptWindows.h"
#include "Prompts/ProgressWindow.h"
#include "network/UpdateTask.h"
#include "Settings.h"

NetworkSettingsMenu::NetworkSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("Network Settings"), r)
{
	updateBtnTxt = new GuiText(tr("Update App"), 22, (GXColor){0, 0, 0, 255});
	updateBtnImg = new GuiImage(btnOutline);
	updateBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	updateBtn->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	updateBtn->SetPosition(100+btnOutline->GetWidth()/2, -65);
	updateBtn->SetLabel(updateBtnTxt);
	updateBtn->SetImage(updateBtnImg);
	updateBtn->SetSoundOver(btnSoundOver);
	updateBtn->SetTrigger(trigA);
	updateBtn->SetEffectGrow();
	updateBtn->Clicked.connect(this, &NetworkSettingsMenu::OnUpdateButtonClick);
	Append(updateBtn);

	SetupOptions();
}

NetworkSettingsMenu::~NetworkSettingsMenu()
{
	Remove(updateBtn);
	delete updateBtnTxt;
	delete updateBtnImg;
	delete updateBtn;
}

void NetworkSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Auto Connect"));
	options.SetName(i++, tr("Update Meta.xml"));
	options.SetName(i++, tr("Update Icon.png"));
	options.SetName(i++, tr("SMB Settings"));
	options.SetName(i++, tr("FTP Client Settings"));
	options.SetName(i++, tr("FTP Server Settings"));
	options.SetName(i++, tr("NFS Settings"));

	SetOptionValues();
}

void NetworkSettingsMenu::SetOptionValues()
{
	int i = 0;

	if(Settings.AutoConnect == 1)options.SetValue(i++, tr("ON"));
	else if(Settings.AutoConnect == 0) options.SetValue(i++, tr("OFF"));

	if(Settings.UpdateMetaxml == 1) options.SetValue(i++, tr("ON"));
	else if(Settings.UpdateMetaxml == 0) options.SetValue(i++, tr("OFF"));

	if(Settings.UpdateIconpng == 1) options.SetValue(i++, tr("ON"));
	else if(Settings.UpdateIconpng == 0) options.SetValue(i++, tr("OFF"));

	options.SetValue(i++, " ");

	options.SetValue(i++, " ");

	options.SetValue(i++, " ");

	options.SetValue(i++, " ");

}

void NetworkSettingsMenu::OnUpdateButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	if(!NetworkInitPrompt())
		return;

	if(ProgressWindow::Instance()->IsRunning())
	{
		ThrowMsg(tr("Error:"), tr("A task is in progress. Can't run update check right now."));
		return;
	}

	UpdateTask *task = new UpdateTask(true, false, false);
	task->SetAutoDelete(true);
	Taskbar::Instance()->AddTask(task);
	ThreadedTaskHandler::Instance()->AddTask(task);
}

void NetworkSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	switch (option)
	{
		case 0:
			Settings.AutoConnect++;
			if(Settings.AutoConnect > 1)
				Settings.AutoConnect = 0;
			break;
		case 1:
			Settings.UpdateMetaxml++;
			if(Settings.UpdateMetaxml > 1)
				Settings.UpdateMetaxml = 0;
			break;
		case 2:
			Settings.UpdateIconpng++;
			if(Settings.UpdateIconpng > 1)
				Settings.UpdateIconpng = 0;
			break;
		case 3:
		{
			this->hide();

			SMBSettingsMenu *menu = new SMBSettingsMenu(this);
			Application::Instance()->Append(menu);
			break;
		}
		case 4:
		{
			this->hide();

			FTPClientSettingsMenu *menu = new FTPClientSettingsMenu(this);
			Application::Instance()->Append(menu);
			break;
		}
		case 5:
		{
			this->hide();

			FTPServerSettingsMenu *menu = new FTPServerSettingsMenu(this);
			Application::Instance()->Append(menu);
			break;
		}
		case 6:
		{
			this->hide();

			NFSSettingsMenu *menu = new NFSSettingsMenu(this);
			Application::Instance()->Append(menu);
			break;
		}
		default:
			break;
	}

	SetOptionValues();
}
