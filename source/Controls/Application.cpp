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
#include "Application.h"
#include "Menus/Explorer.h"
#include "DiskOperations/di2.h"
#include "SoundOperations/MusicPlayer.h"
#include "Memory/Resources.h"
#include "Controls/Taskbar.h"
#include "network/networkops.h"
#include "DeviceControls/DeviceHandler.hpp"
#include "TextOperations/FontSystem.h"
#include "FTPOperations/FTPServer.h"
#include "Prompts/HomeMenu.h"
#include "Prompts/ProgressWindow.h"
#include "Prompts/ThrobberWindow.h"
#include "SoundOperations/SoundHandler.hpp"
#include "Settings.h"
#include "VideoOperations/video.h"
#include "ThreadedTaskHandler.hpp"
#include "System/IosLoader.h"
#include "Memory/mem2.h"
#include "Settings.h"
#include "audio.h"
#include "input.h"
#include "sys.h"

Application *Application::instance = NULL;
bool Application::exitApplication = false;
bool Application::bReset = false;
bool Application::bShutdown = false;

Application::Application()
	: GuiFrame(screenwidth, screenheight) // screenwidth and height are defined in Video.h
{
	GXColor ImgColor[4];
	ImgColor[0] = RGBATOGXCOLOR(Settings.BackgroundUL);
	ImgColor[1] = RGBATOGXCOLOR(Settings.BackgroundUR);
	ImgColor[2] = RGBATOGXCOLOR(Settings.BackgroundBR);
	ImgColor[3] = RGBATOGXCOLOR(Settings.BackgroundBL);

	bgImg = new GuiImage(screenwidth, screenheight, &ImgColor[0]);

	//! Setup WiiMote Pointers
	pointer[0] = new WiiPointer("player1_point.png");
	pointer[1] = new WiiPointer("player2_point.png");
	pointer[2] = new WiiPointer("player3_point.png");
	pointer[3] = new WiiPointer("player4_point.png");

	//! initially all input updates are allowed
	bGuiInputUpdate = true;
}

Application::~Application()
{
	RemoveAll();
	delete bgImg;
	delete btnHome;

	for (int i = 0; i < 4; i++)
		delete pointer[i];

	//! remove still outstanding deletes without any dependencies
	for(u32 i = 0; i < deleteList.size(); i++)
		delete deleteList[i];
}

void Application::quit()
{
	// Fade out...
	for(int i = 0; i <= 255; i += 15)
	{
		Draw();
		GXColor fadeoutColor = (GXColor){0, 0, 0, i};
		Menu_DrawRectangle(0, 0, 100.0f, screenwidth, screenheight, &fadeoutColor, false, true);
		Menu_Render();
	}

	exitApplication = true;
}

void Application::hide()
{
	RemoveAll();
}

void Application::show()
{
	Append(bgImg);
	Append(MusicPlayer::Instance());
	Append(ProgressWindow::Instance());
	//! Append taskbar instance
	Append(Taskbar::Instance());
}

void Application::exec()
{
	while(!exitApplication)
	{
		updateEvents();
	}

	ExitApp();
}

void Application::updateEvents()
{
	if(exitApplication)
		return;

	if(bShutdown) {
		Sys_Shutdown();
	}
	else if(bReset) {
		Sys_Reboot();
	}

	//! first update the inputs
	UpdatePads();

	//! update WiiMote pointer position and activity first
	for (int i = 0; i < 4; i++)
		pointer[i]->Update(&userInput[i]);

	//! update the gui elements with new inputs
	if(bGuiInputUpdate)
	{
		for (int i = 0; i < 4; i++)
		{
			if(!updateOnlyElement.empty())
				updateOnlyElement.back()->Update(&userInput[i]);
			else
				Update(&userInput[i]);

			//! always update the home menu, everywhere
			btnHome->Update(&userInput[i]);
		}
	}

	//! render everything
	Draw();

	//! render wii mote pointer always last and on top
	for (int i = 3; i >= 0; i--)
		pointer[i]->Draw();

	//! render to screen
	Menu_Render();

	//! delete elements that were queued for delete after the rendering is done
	if(!deleteList.empty())
		ProcessDeleteQueue();
	//! execute tasks that require main thread execution
	if(!postUpdateTasks.empty())
		ProcessPostUpdateTasks();
}

void Application::SetGrabPointer(int i)
{
	if (i < 0 || i > 3)
		return;

	char imageTxt[20];
	snprintf(imageTxt, sizeof(imageTxt), "player%i_grab.png", i + 1);

	pointer[i]->SetImage(imageTxt);
}

void Application::ResetPointer(int i)
{
	if (i < 0 || i > 3)
		return;

	char imageTxt[20];
	snprintf(imageTxt, sizeof(imageTxt), "player%i_point.png", i + 1);

	pointer[i]->SetImage(imageTxt);
}

void Application::ProcessDeleteQueue(void)
{
	m_mutex.lock();
	for(u32 i = 0; i < deleteList.size(); ++i)
	{
		GuiElement *parent = deleteList[i]->GetParent();
		//! Only remove elements when they are currently not animated
		//! otherwise wait till animation is finished
		if(!parent || !deleteList[i]->IsAnimated())
		{
			//! since i am lazy and don't remove them usually, this is where i remove it
			if(parent)
			{
				GuiFrame *parentFrame = dynamic_cast<GuiFrame *>(parent);
				if(parentFrame)
					parentFrame->Remove(deleteList[i]);
			}

			UnsetUpdateOnly(deleteList[i]);
			delete deleteList[i];
			deleteList.erase(deleteList.begin() + i);
			i--;
		}
	}
	m_mutex.unlock();
}

void Application::PushForDelete(GuiElement *e)
{
	if(!e)
		return;

	//! elements that are queued for delete are only rendered and not updated with buttons
	e->SetState(STATE_DISABLED);

	m_mutex.lock();
	deleteList.push_back(e);
	m_mutex.unlock();
}

void Application::ProcessPostUpdateTasks(void)
{
	while(!postUpdateTasks.empty())
	{
		m_mutex.lock();
		ThreadedTask *task = postUpdateTasks.front();
		postUpdateTasks.pop();
		m_mutex.unlock();

		task->Execute();
		delete task;
	}
}

/***********************************************************
 * Main application initialization.
 *
 * Called only once at the beginning of the application.
 ***********************************************************/
void Application::init(void)
{
	InitGecko();		// Initialize stdout/stderr and gecko output
	MEM2_init(52);		// Initialize 52 MB (max is 53469152 bytes though)
	MagicPatches(1);	// We all love magic
	Settings.EntraceIOS = (u8) IOS_GetVersion();	// remember entrace IOS

	//! TODO: clean and split this up

	//!********************************************
	//! Main init routine
	//!********************************************
	//! setup default font for initialization process
	SetupDefaultFont(NULL);
	//! setup background image and class size as they might be changed in video init
	this->SetSize(screenwidth, screenheight);
	bgImg->SetSize(screenwidth, screenheight);
	//! Temporary prompt window to notify the user about the loading process
	ThrobberWindow window("Initializing...", "Mounting devices");
	window.SetParent(this);
	bool effectStop = false;
	while(!effectStop)
	{
		//! render last frame when effect is 0 too
		effectStop = window.GetEffect() == 0;
		//! render prompt
		bgImg->Draw();
		window.Draw();
		Menu_Render();
	}

	//! render prompt
	bgImg->Draw();
	window.Draw();
	Menu_Render();

	//! Initalise the SD/USB devices
	DeviceHandler::Instance()->MountSD();
	DeviceHandler::Instance()->MountAllUSB();

	//! try loading the settings files
	bool bSettingsLoaded = Settings.Load();

	if(Settings.EntraceIOS != Settings.BootIOS)
	{
		//! notify user that we are about to reload IOS
		window.SetMessage(fmt("Reloading into IOS %i", Settings.BootIOS));
		//! render prompt
		bgImg->Draw();
		window.Draw();
		Menu_Render();

		//! load IOS
		IosLoader::ReloadAppIos(Settings.BootIOS);

		//! if settings were not loaded before, reloading here again.
		if(!bSettingsLoaded)
			Settings.Load();
	}

	//! notify user about new action
	window.SetMessage("Loading resources");
	//! render prompt
	bgImg->Draw();
	window.Draw();
	Menu_Render();

	//! init rest of the devices
	SetupPads();		// Initialize input
	InitAudio();		// Initialize audio
	Sys_Init();			// Initialize shutdown/reset buttons
	DI2_Init();			//Init DVD Driver
	ISFS_Initialize();	// ISFS init
	DeviceHandler::Instance()->MountGCA();
	DeviceHandler::Instance()->MountGCB();
	DeviceHandler::Instance()->MountNAND();
	Settings.LoadLanguage(Settings.LanguagePath);
	SetupPDFFontPath(Settings.UpdatePath);
	SetupDefaultFont(Settings.CustomFontPath);

	//! Set UTF 8 locale
	setlocale(LC_CTYPE, "C-UTF-8");
	setlocale(LC_MESSAGES, "C-UTF-8");

	//! Set main thread prio very high as it is the render thread
	LWP_SetThreadPriority(LWP_GetSelf(), 121);

	//! Initialize sound handler thread
	SoundHandler::Instance();

	//! Initialize network thread if selected
	if(Settings.AutoConnect)
	{
		InitNetworkThread();
		ResumeNetworkThread();
	}

	//! FTP Server thread
	if(Settings.FTPServer.AutoStart)
		FTPServer::Instance()->StartupFTP();

	//! Initialize the task thread
	ThreadedTaskHandler::Instance()->setThreadPriority(Settings.CopyThreadPrio);

	//!********************************************
	//! class internal things
	//!********************************************
	//! resetup BG color
	GXColor *bgColor = GetBGColorPtr();
	bgColor[0] = RGBATOGXCOLOR(Settings.BackgroundUL);
	bgColor[1] = RGBATOGXCOLOR(Settings.BackgroundUR);
	bgColor[2] = RGBATOGXCOLOR(Settings.BackgroundBR);
	bgColor[3] = RGBATOGXCOLOR(Settings.BackgroundBL);

	//! Setup the music player
	MusicPlayer::Instance()->SetVolume(Settings.MusicVolume);
	MusicPlayer::Instance()->SetLoop(Settings.BGMLoopMode);
	MusicPlayer::Instance()->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	MusicPlayer::Instance()->SetPosition(30, 230);

	//! Fade window out
	window.SetEffect(EFFECT_FADE, -20);
	effectStop = false;
	while(!effectStop)
	{
		//! render last frame when effect is 0 too
		effectStop = window.GetEffect() == 0;
		//! render prompt
		bgImg->Draw();
		window.Draw();
		Menu_Render();
	}
	//! remove parant as otherwise prompt window would slide out
	window.SetParent(NULL);

	//! setup the home menu button
	trigHome.SetButtonOnlyTrigger(-1, WiiControls.HomeButton | ClassicControls.HomeButton << 16, GCControls.HomeButton);

	btnHome = new GuiButton(0, 0);
	btnHome->SetTrigger(&trigHome);
	btnHome->Clicked.connect(this, &Application::OnHomeButtonClick);
}

void Application::OnHomeButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p3 UNUSED)
{
	// disable home menu button clicks while we are inside the home menu
	btnHome->SetClickable(false);

	HomeMenu *homeMenu = new HomeMenu();
	homeMenu->DimBackground(true);
	homeMenu->Closing.connect(this, &Application::OnHomeMenuClosing);
	this->SetUpdateOnly(homeMenu);
	this->Append(homeMenu);
}

void Application::OnHomeMenuClosing(GuiFrame *menu UNUSED)
{
	btnHome->SetClickable(true);
}
