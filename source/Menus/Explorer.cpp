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
#include "Explorer.h"
#include "ArchiveOperations/ArchiveBrowser.h"
#include "FileOperations/ListFileBrowser.hpp"
#include "FileOperations/IconFileBrowser.hpp"
#include "FileOperations/fileops.h"
#include "FileStartUp/FileStartUp.h"
#include "DeviceControls/RemountTask.h"
#include "Controls/Application.h"
#include "Controls/Clipboard.h"
#include "Controls/Taskbar.h"
#include "Memory/Resources.h"
#include "FileOperations/ProcessChoice.h"
#include "Prompts/CreditWindow.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/ThrobberWindow.h"
#include "Prompts/ProgressWindow.h"
#include "DiskOperations/di2.h"
#include "Settings.h"
#include "sys.h"

Explorer::Explorer(GuiFrame *p, const char *path)
	: GuiFrame(0, 0, p)
{
	this->Init();
	if(path)
		this->LoadPath(path);
}

Explorer::~Explorer()
{
	RemoveAll();

	Resources::Remove(btnSoundClick);
	Resources::Remove(btnSoundOver);
	Resources::Remove(creditsImgData);
	Resources::Remove(Background);
	Resources::Remove(Address);
	Resources::Remove(Refresh);
	Resources::Remove(sdstorage);
	Resources::Remove(usbstorage);
	Resources::Remove(usbstorage_blue);
	Resources::Remove(networkstorage);
	Resources::Remove(ftpstorage);
	Resources::Remove(nand_ImgData);
	Resources::Remove(dvd_ImgData);

	delete BackgroundImg;
	delete creditsImg;
	delete deviceImg;
	delete AdressbarImg;
	delete RefreshImg;

	delete AdressText;

	delete RefreshBtn;
	delete CreditsBtn;
	delete deviceSwitchBtn;
	delete Adressbar;
	delete clickmenuBtn;
	delete BackInDirBtn;

	delete trigA;
	delete trigPlus;
	delete trigBackInDir;

	if(fileBrowser != curBrowser)
		delete fileBrowser;

	delete curBrowser;
	delete guiBrowser;
}

void Explorer::Init()
{
	explorerTasks = 0;
	guiBrowser = NULL;
	curBrowser = NULL;

	fileBrowser = new FileBrowser();
	curBrowser = fileBrowser;

	SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	SetPosition(0, 50);

	trigA = new SimpleGuiTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigPlus = new GuiTrigger();
	trigPlus->SetButtonOnlyTrigger(-1, WiiControls.ContextMenuButton | ClassicControls.ContextMenuButton << 16, GCControls.ContextMenuButton);
	trigBackInDir = new GuiTrigger();
	trigBackInDir->SetButtonOnlyTrigger(-1, WiiControls.UpInDirectory | ClassicControls.UpInDirectory << 16, GCControls.UpInDirectory);

	btnSoundClick = Resources::GetSound("button_click.wav");
	btnSoundOver = Resources::GetSound("button_over.wav");

	creditsImgData = Resources::GetImageData("WiiXplorer.png");
	Background = Resources::GetImageData("bg_browser.png");
	Address = Resources::GetImageData("addressbar_textbox.png");
	Refresh = Resources::GetImageData("refresh2.png");

	sdstorage = Resources::GetImageData("sdstorage.png");
	usbstorage = Resources::GetImageData("usbstorage.png");
	usbstorage_blue = Resources::GetImageData("usbstorage_blue.png");
	networkstorage = Resources::GetImageData("networkstorage.png");
	ftpstorage = Resources::GetImageData("ftpstorage.png");
	nand_ImgData = Resources::GetImageData("nandstorage.png");
	dvd_ImgData = Resources::GetImageData("dvdstorage.png");

	width = Background->GetWidth();
	height = Background->GetHeight();
	BackgroundImg = new GuiImage(Background);

	guiBrowserType = Settings.BrowserMode;

	if(guiBrowserType == ICONBROWSER)
	{
		guiBrowser = new IconFileBrowser(curBrowser, width, 252);
	}
	else
	{
		guiBrowser = new ListFileBrowser(curBrowser, width, 252);
	}

	guiBrowser->SetPosition(0, 53);
	guiBrowser->Clicked.connect(this, &Explorer::OnBrowserChanges);

	creditsImg = new GuiImage(creditsImgData);
	CreditsBtn = new GuiButton(creditsImgData->GetWidth(), creditsImgData->GetHeight());
	CreditsBtn->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	CreditsBtn->SetPosition(guiBrowser->GetLeft()+235, guiBrowser->GetTop()+262);
	CreditsBtn->SetImage(creditsImg);
	CreditsBtn->SetSoundClick(btnSoundClick);
	CreditsBtn->SetSoundOver(btnSoundOver);
	CreditsBtn->SetTrigger(trigA);
	CreditsBtn->SetEffectGrow();
	CreditsBtn->Clicked.connect(this, &Explorer::OnCreditsButtonClick);

	deviceImg = new GuiImage(sdstorage);

	deviceSwitchBtn = new GuiButton(deviceImg->GetWidth(), deviceImg->GetHeight());
	deviceSwitchBtn->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	deviceSwitchBtn->SetPosition(guiBrowser->GetLeft()+20, guiBrowser->GetTop()-38);
	deviceSwitchBtn->SetImage(deviceImg);
	deviceSwitchBtn->SetSoundClick(btnSoundClick);
	deviceSwitchBtn->SetSoundOver(btnSoundOver);
	deviceSwitchBtn->SetTrigger(trigA);
	deviceSwitchBtn->SetEffectGrow();
	deviceSwitchBtn->Clicked.connect(this, &Explorer::OnDeviceButtonClick);

	AdressText = new GuiText((char *) NULL, 20, (GXColor) {0, 0, 0, 255});
	AdressText->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	AdressText->SetPosition(18, 0);
	AdressText->SetMaxWidth(Address->GetWidth()-45-Refresh->GetWidth(), SCROLL_HORIZONTAL);
	AdressbarImg = new GuiImage(Address);
	Adressbar = new GuiButton(Address->GetWidth()-Refresh->GetWidth()-5, Address->GetHeight());
	Adressbar->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Adressbar->SetPosition(guiBrowser->GetLeft()+62, guiBrowser->GetTop()-38);
	Adressbar->SetImage(AdressbarImg);
	Adressbar->SetLabel(AdressText);
	Adressbar->SetSoundClick(btnSoundClick);
	Adressbar->SetSoundOver(btnSoundOver);
	Adressbar->SetRumble(false);
	Adressbar->SetTrigger(trigA);
	Adressbar->Clicked.connect(this, &Explorer::OnButtonClick);

	RefreshImg = new GuiImage(Refresh);
	RefreshImg->SetScale(0.8);
	RefreshBtn = new GuiButton(Refresh->GetWidth(), Refresh->GetHeight());
	RefreshBtn->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	RefreshBtn->SetPosition(Adressbar->GetLeft()+Adressbar->GetWidth()-5, Adressbar->GetTop()+4);
	RefreshBtn->SetImage(RefreshImg);
	RefreshBtn->SetSoundClick(btnSoundClick);
	RefreshBtn->SetSoundOver(btnSoundOver);
	RefreshBtn->SetTrigger(trigA);
	RefreshBtn->SetEffectGrow();
	RefreshBtn->Clicked.connect(this, &Explorer::OnButtonClick);

	clickmenuBtn = new GuiButton(guiBrowser->GetWidth(), guiBrowser->GetHeight());
	clickmenuBtn->SetPosition(guiBrowser->GetLeft(), guiBrowser->GetTop());
	clickmenuBtn->SetTrigger(trigPlus);
	clickmenuBtn->Clicked.connect(this, &Explorer::OnContextButtonClick);

	BackInDirBtn = new GuiButton(0, 0);
	BackInDirBtn->SetTrigger(trigBackInDir);
	BackInDirBtn->Clicked.connect(this, &Explorer::BackInDirectory);

	//! catch each finished effect event
	EffectFinished.connect(this, &Explorer::OnEffectFinish);

	Append(BackgroundImg);
	Append(clickmenuBtn);
	Append(CreditsBtn);
	Append(Adressbar);
	Append(RefreshBtn);
	Append(deviceSwitchBtn);
	Append(BackInDirBtn);
	Append(guiBrowser);

	show();
}

void Explorer::show()
{
	SetEffect(EFFECT_FADE, 50);

	if(parentElement)
		((GuiFrame *) parentElement)->Append(this);

	if(guiBrowserType != Settings.BrowserMode)
	{
		Remove(guiBrowser);
		delete guiBrowser;
		guiBrowserType = Settings.BrowserMode;

		if(guiBrowserType == ICONBROWSER)
			guiBrowser = new IconFileBrowser(curBrowser, width, 252);
		else
			guiBrowser = new ListFileBrowser(curBrowser, width, 252);
		guiBrowser->SetPosition(0, 53);
		guiBrowser->Clicked.connect(this, &Explorer::OnBrowserChanges);
		Append(guiBrowser);
	}
}

void Explorer::hide()
{
	if(!Application::isClosing())
		SetEffect(EFFECT_FADE, -50);
	else if(parentElement)
		((GuiFrame *) parentElement)->Remove(this);

}

void Explorer::OnEffectFinish(GuiElement *e UNUSED)
{
	//! on hide effect remove the explorer from the application
	if(GetEffect() == EFFECT_FADE && effectAmount < 0)
	{
		if(parentElement)
			((GuiFrame *) parentElement)->Remove(this);

	}
}

int Explorer::LoadPath(const char * path)
{
	int filecount = curBrowser->BrowsePath(path);
	if(filecount < 0)
	{
		//int choice = WindowPrompt(tr("Error:"), fmt("%s %s", tr("Unable to load path:"), path), tr("Retry"), tr("Close"));
		//if(choice)
			//return LoadPath(path);
	}

	curBrowser->ResetMarker();
	AdressText->SetText(curBrowser->GetCurrentPath());
	Settings.LastUsedPath.assign(curBrowser->GetCurrentPath());
	SetDeviceImage();
	return filecount;
}

void Explorer::SetDeviceImage()
{
	const char * currentroot = fileBrowser->GetRootDir();
	if(strncmp(currentroot, DeviceName[SD], 2) == 0)
	{
		deviceImg->SetImage(sdstorage);
	}
	else if(strncmp(currentroot, DeviceName[USB1], 3) == 0)
	{
		const char * FSName = DeviceHandler::PathToFSName(curBrowser->GetCurrentPath());

		if(FSName && strncmp(FSName, "NTFS", 4) != 0)
			deviceImg->SetImage(usbstorage);
		else
			deviceImg->SetImage(usbstorage_blue);
	}
	else if(strncmp(currentroot, DeviceName[SMB1], 3) == 0)
	{
		deviceImg->SetImage(networkstorage);
	}
	else if(strncmp(currentroot, DeviceName[FTP1], 3) == 0)
	{
		deviceImg->SetImage(ftpstorage);
	}
	else if(strncmp(currentroot, DeviceName[NFS1], 3) == 0)
	{
		deviceImg->SetImage(networkstorage);
	}
	else if(strncmp(currentroot, DeviceName[NAND], 4) == 0)
	{
		deviceImg->SetImage(nand_ImgData);
	}
	else if(strncmp(currentroot, DeviceName[DVD], 3) == 0)
	{
		deviceImg->SetImage(dvd_ImgData);
	}
}

void Explorer::OnBrowserChanges(int index UNUSED)
{
	// check corresponding browser entry
	if(curBrowser->IsCurrentDir())
	{
		int result = curBrowser->ChangeDirectory();
		if(result >= 0)
		{
			guiBrowser->SetSelected(0);
			curBrowser->SetPageIndex(0);
			curBrowser->ResetMarker();
			guiBrowser->Refresh();
			AdressText->SetText(curBrowser->GetCurrentPath());
			if(fileBrowser == curBrowser)
				Settings.LastUsedPath.assign(curBrowser->GetCurrentPath());
		}
		else if(result == CLOSE_ARCHIVE && fileBrowser != curBrowser)
		{
			delete curBrowser;
			curBrowser = fileBrowser;
			guiBrowser->SetBrowser(curBrowser);
			guiBrowser->Refresh();
			AdressText->SetTextf("%s", curBrowser->GetCurrentPath());
		}
		else
		{
			ShowError(tr("Can't browse that path."));
		}
	}
	else
	{
		int result = 0;

		SetState(STATE_DISABLED);

		result = curBrowser->ExecuteFile(curBrowser->GetCurrentSelectedFilepath());

		SetState(STATE_DEFAULT);

		if(result == ARCHIVE)
		{
			if(fileBrowser != curBrowser)
				delete curBrowser;

			curBrowser = new ArchiveBrowser(curBrowser->GetCurrentSelectedFilepath());
			guiBrowser->SetBrowser(curBrowser);
			AdressText->SetText(curBrowser->GetCurrentPath());
		}
		else if(result == REFRESH_BROWSER || result == RELOAD_BROWSER)
		{
			guiBrowser->Refresh();
		}
	}
}

void Explorer::OnRightClick(PopUpMenu *menu, int item)
{
	Application::Instance()->Remove(menu);
	Application::Instance()->UnsetUpdateOnly(menu);

	guiBrowser->SetState(STATE_DISABLED);

	if(item >= 0 && curBrowser != fileBrowser) //! Archive
	{
		ProcessArcChoice(item, fileBrowser->GetCurrentPath());
		guiBrowser->Refresh();
	}
	else if(item >= 0)  //! Real file browser
	{
		ProcessChoice(item);
	}

	guiBrowser->SetState(STATE_DEFAULT);

	Application::Instance()->PushForDelete(menu);

}

void Explorer::OnDeviceSelect(DeviceMenu *deviceMenu, int device)
{
	deviceMenu->SetEffect(EFFECT_FADE, -30);

	if(device == DVD)
	{
		char *read_buffer = (char*) memalign(32, 2048);
		if(!read_buffer) return;

		if(DI2_ReadDVD(read_buffer, 1, 0) != 0)
		{
			RemountTask *mountTask = new RemountTask(tr("Mounting DVD"), DVD);
			Taskbar::Instance()->AddTask(mountTask);
			ThreadedTaskHandler::Instance()->AddTask(mountTask);
		}
		free(read_buffer);
	}

	if(device >= SD && device < MAXDEVICES)
	{
		if(curBrowser != fileBrowser)
		{
			delete curBrowser;
			curBrowser = fileBrowser;
			guiBrowser->SetBrowser(curBrowser);
		}
		LoadPath(fmt("%s:/", DeviceName[device]));
		guiBrowser->SetSelected(0);
		guiBrowser->Refresh();
		AdressText->SetText(curBrowser->GetCurrentPath());
		Settings.LastUsedPath.assign(fileBrowser->GetCurrentPath());
	}

	Application::Instance()->PushForDelete(deviceMenu);
}

void Explorer::OnCreditsButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	CreditWindow * Credits = new CreditWindow(this);
	Credits->DimBackground(true);
	Application::Instance()->SetUpdateOnly(Credits);
}

void Explorer::OnDeviceButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	DeviceMenu *deviceMenu = new DeviceMenu(deviceSwitchBtn->GetLeft()-5-this->GetLeft(), deviceSwitchBtn->GetTop()+deviceSwitchBtn->GetHeight()-this->GetTop());
	deviceMenu->DeviceSelected.connect(this, &Explorer::OnDeviceSelect);
	Application::Instance()->SetUpdateOnly(deviceMenu);
	this->Append(deviceMenu);
}

void Explorer::OnContextButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p)
{
	PopUpMenu *RightClick = NULL;
	if(curBrowser != fileBrowser) //! Archive
	{
		RightClick = new PopUpMenu(p.x, p.y);
		RightClick->AddItem(tr("Paste"));
		RightClick->AddItem(tr("Extract"));
		RightClick->AddItem(tr("Extract All"));
		RightClick->AddItem(tr("Properties"));
		RightClick->Finish();
	}
	else //! Real file browser
	{
		RightClick = new PopUpMenu(p.x, p.y);
		RightClick->AddItem(tr("Cut"));
		RightClick->AddItem(tr("Copy"));
		RightClick->AddItem(tr("Paste"));
		RightClick->AddItem(tr("Rename"));
		RightClick->AddItem(tr("Delete"));
		RightClick->AddItem(tr("New Folder"));
		RightClick->AddItem(tr("Add to zip"));
		RightClick->AddItem(tr("MD5 Check"));
		RightClick->AddItem(tr("Properties"));
		RightClick->Finish();
	}
	RightClick->ItemClicked.connect(this, &Explorer::OnRightClick);
	Application::Instance()->SetUpdateOnly(RightClick);
	Application::Instance()->Append(RightClick);
}

void Explorer::OnButtonClick(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	if(sender == RefreshBtn)
	{
		guiBrowser->Refresh();
	}

	else if(sender == Adressbar)
	{
		char entered[1024];
		strcpy(entered, curBrowser->GetCurrentPath());
		if(OnScreenKeyboard(entered, sizeof(entered)))
		{
			LoadPath(entered);
		}
	}
}

void Explorer::BackInDirectory(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	curBrowser->BackInDirectory();
	guiBrowser->SetSelected(0);
	curBrowser->SetPageIndex(0);
	curBrowser->ResetMarker();
	guiBrowser->Refresh();
	AdressText->SetText(curBrowser->GetCurrentPath());

	sender->ResetState();
}

void Explorer::OnFinishedTask(Task *t)
{
	//! Stop progress window on last task
	if(--explorerTasks == 0) {
		StopProgress();
		ResetReplaceChoice();
	}
	guiBrowser->Refresh();
	Application::Instance()->PushForDelete(t);
}
