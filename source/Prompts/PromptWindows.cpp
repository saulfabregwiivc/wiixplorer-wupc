/***************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 * PromptWindows.cpp
 *
 * All promptwindows
 * for WiiXplorer 2010
 ***************************************************************************/
#include "GUI/gui_keyboard.h"
#include "network/networkops.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/KeyboardWindow.h"
#include "Controls/Taskbar.h"
#include "FileOperations/fileops.h"
#include "FileOperations/filebrowser.h"
#include "Controls/Application.h"
#include "Tools/StringTools.h"
#include "TextOperations/wstring.hpp"
#include "sys.h"
#include "svnrev.h"

/**************************************************************************
* OnScreenKeyboard for unicode string
***************************************************************************/
int OnScreenKeyboard(wchar_t * var, u16 maxlen)
{
	int save = 0;
	KeyboardWindow *keyboard = new KeyboardWindow(var, maxlen);
	keyboard->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	keyboard->DimBackground(true);

	Application::Instance()->Append(keyboard);
	Application::Instance()->SetUpdateOnly(keyboard);

	while((save = keyboard->GetChoice()) < 0)
	{
		Application::Instance()->updateEvents();
	}

	Application::Instance()->PushForDelete(keyboard);
	Application::Instance()->updateEvents();

	return save;
}

/**************************************************************************
* OnScreenKeyboard overload for UTF-8 strings
***************************************************************************/
int OnScreenKeyboard(char * var, u16 maxlen)
{
	wchar_t *wtext = new wchar_t[maxlen+2];
	wtext[0] = 0;

	if(strlen(var) > maxlen)
		var[maxlen] = 0;

	char2wchar_t(var, wtext);

	int save = OnScreenKeyboard(wtext, maxlen);

	if(save)
	{
		wString Converter(wtext);
		snprintf(var, maxlen, Converter.toUTF8().c_str());
	}

	delete [] wtext;

	return save;
}

/****************************************************************************
* NetworkInitPrompt
***************************************************************************/
bool NetworkInitPrompt()
{
	if(IsNetworkInit())
		return true;

	int choice = -1;

	PromptWindow * Prompt = new PromptWindow(tr("Network initialising..."), tr("Please wait..."), tr("Cancel"));
	Prompt->DimBackground(true);
	Application::Instance()->Append(Prompt);
	Application::Instance()->SetUpdateOnly(Prompt);

	if(!Settings.AutoConnect)
	{
		InitNetworkThread();
		ResumeNetworkThread();
	}

	while((choice = Prompt->GetChoice()) < 0 && !IsNetworkInit())
	{
		Application::Instance()->updateEvents();
	}

	Prompt->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	Application::Instance()->PushForDelete(Prompt);
	Application::Instance()->updateEvents();

	if(!IsNetworkInit())
	{
		ShowError(tr("No network connection."));
		return false;
	}

	return true;
}

/****************************************************************************
* WindowPrompt
*
* Shortcut function
***************************************************************************/
int WindowPrompt(const char *title, const char *msg,
				 const char *btn1Label, const char *btn2Label,
				 const char *btn3Label, const char *btn4Label)
{
	int choice = -1;

	PromptWindow * Prompt = new PromptWindow(title, msg, btn1Label, btn2Label, btn3Label, btn4Label);
	Prompt->DimBackground(true);
	Application::Instance()->Append(Prompt);
	Application::Instance()->SetUpdateOnly(Prompt);

	while((choice = Prompt->GetChoice()) < 0)
	{
		Application::Instance()->updateEvents();
	}

	Prompt->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	Application::Instance()->PushForDelete(Prompt);
	//! TODO: fixme
	//! since blocking function continue on Update() loop where they stop
	//! the last pressed button on the prompt window will be used in the
	//! follow up updates, prevent that by reupdating the pads with one event
	Application::Instance()->updateEvents();

	return choice;
}
