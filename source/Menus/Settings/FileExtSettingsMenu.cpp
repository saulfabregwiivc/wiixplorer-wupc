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
#include "FileExtSettingsMenu.h"
#include "Prompts/PromptWindows.h"
#include "Settings.h"

FileExtSettingsMenu::FileExtSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("File Extensions Settings"), r)
{
	SetupOptions();
}

FileExtSettingsMenu::~FileExtSettingsMenu()
{
}

void FileExtSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Video Files"));
	options.SetName(i++, tr("Audio Files"));
	options.SetName(i++, tr("Image Files"));
	options.SetName(i++, tr("Archive Files"));
	options.SetName(i++, tr("Homebrew Files"));
	options.SetName(i++, tr("Font Files"));
	options.SetName(i++, tr("Language Files"));
	options.SetName(i++, tr("Wii Binary Files"));
	options.SetName(i++, tr("PDF Files"));
	options.SetName(i++, tr("WiiXplorer Movies"));

	SetOptionValues();
}

void FileExtSettingsMenu::SetOptionValues()
{
	int i = 0;

	options.SetValue(i++, Settings.FileExtensions.GetVideo());
	options.SetValue(i++, Settings.FileExtensions.GetAudio());
	options.SetValue(i++, Settings.FileExtensions.GetImage());
	options.SetValue(i++, Settings.FileExtensions.GetArchive());
	options.SetValue(i++, Settings.FileExtensions.GetHomebrew());
	options.SetValue(i++, Settings.FileExtensions.GetFont());
	options.SetValue(i++, Settings.FileExtensions.GetLanguageFiles());
	options.SetValue(i++, Settings.FileExtensions.GetWiiBinary());
	options.SetValue(i++, Settings.FileExtensions.GetPDF());
	options.SetValue(i++, Settings.FileExtensions.GetWiiXplorerMovies());
}

void FileExtSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	char entered[300];

	switch (option)
	{
		case 0:
			snprintf(entered, sizeof(entered), "%s", Settings.FileExtensions.GetVideo());
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.FileExtensions.SetVideo(entered);
			}
			break;
		case 1:
			snprintf(entered, sizeof(entered), "%s", Settings.FileExtensions.GetAudio());
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.FileExtensions.SetAudio(entered);
			}
			break;
		case 2:
			snprintf(entered, sizeof(entered), "%s", Settings.FileExtensions.GetImage());
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.FileExtensions.SetImage(entered);
			}
			break;
		case 3:
			snprintf(entered, sizeof(entered), "%s", Settings.FileExtensions.GetArchive());
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.FileExtensions.SetArchive(entered);
			}
			break;
		case 4:
			snprintf(entered, sizeof(entered), "%s", Settings.FileExtensions.GetHomebrew());
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.FileExtensions.SetHomebrew(entered);
			}
			break;
		case 5:
			snprintf(entered, sizeof(entered), "%s", Settings.FileExtensions.GetFont());
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.FileExtensions.SetFont(entered);
			}
			break;
		case 6:
			snprintf(entered, sizeof(entered), "%s", Settings.FileExtensions.GetLanguageFiles());
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.FileExtensions.SetLanguageFiles(entered);
			}
			break;
		case 7:
			snprintf(entered, sizeof(entered), "%s", Settings.FileExtensions.GetWiiBinary());
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.FileExtensions.SetWiiBinary(entered);
			}
			break;
		case 8:
			snprintf(entered, sizeof(entered), "%s", Settings.FileExtensions.GetPDF());
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.FileExtensions.SetPDF(entered);
			}
			break;
		case 9:
			snprintf(entered, sizeof(entered), "%s", Settings.FileExtensions.GetWiiXplorerMovies());
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.FileExtensions.SetWiiXplorerMovies(entered);
			}
			break;
		default:
			break;
	}

	SetOptionValues();
}
