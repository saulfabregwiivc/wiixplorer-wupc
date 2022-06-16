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
#include "GeneralSettingsMenu.h"
#include "Prompts/PromptWindows.h"
#include "LanguageSettingsMenu.h"
#include "Controls/Application.h"
#include "Controls/ThreadedTaskHandler.hpp"
#include "System/IosLoader.h"
#include "Settings.h"

static inline int NextPriority(int prio)
{
	switch(prio)
	{
		case 0:
			return 30;
		case 30:
			return 70;
		case 70:
			return 90;
		case 90:
			return 120;
		case 120:
			return 0;
		default:
			return 30;
	}
}

static inline const char * PrioritySynonym(int prio)
{
	switch(prio)
	{
		case 0:
			return tr("Idle");
		case 30:
			return tr("Low");
		case 70:
			return tr("Normal");
		case 90:
			return tr("High");
		case 120:
			return tr("Highest");
		default:
			return NULL;
	}
}

static inline const char * CompressionSynonym(int comp)
{
	switch(comp)
	{
		case -1:
			return tr("(Default)");
		case 0:
			return tr("(Store)");
		case 1:
			return tr("(Best speed)");
		case 9:
			return tr("(Best compression)");
		default:
			return "";
	}
}

GeneralSettingsMenu::GeneralSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("General Settings"), r)
{
	SetupOptions();
}

GeneralSettingsMenu::~GeneralSettingsMenu()
{
	ThreadedTaskHandler::Instance()->setThreadPriority(Settings.CopyThreadPrio);
}

void GeneralSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Language"));
	options.SetName(i++, tr("Clock Mode"));
	options.SetName(i++, tr("PDF Processing Zoom"));
	options.SetName(i++, tr("Keyboard Delete Delay"));
	options.SetName(i++, tr("Rumble"));
	options.SetName(i++, tr("Scrolling Speed"));
	options.SetName(i++, tr("Tooltip Delay"));
	options.SetName(i++, tr("Zip Compression Mode"));
	options.SetName(i++, tr("Copy Thread Priority"));
	options.SetName(i++, tr("Copy Thread Background Priority"));
	options.SetName(i++, tr("Show Partition Formatter"));
	options.SetName(i++, tr("Use Both USB Ports"));
	options.SetName(i++, tr("Override Priiloader"));

	SetOptionValues();
}

void GeneralSettingsMenu::SetOptionValues()
{
	int i = 0;

	if(strextcmp(Settings.LanguagePath, "lang", '.') != 0)
	{
		options.SetValue(i++, tr("Default"));
	}
	else
	{
		char * language = strrchr(Settings.LanguagePath, '/')+1;
		options.SetValue(i++, "%s", language);
	}

	if (Settings.ClockMode == 1)
		options.SetValue(i++, tr("12H"));
	else
		options.SetValue(i++, tr("24H"));

	options.SetValue(i++, "%0.2f", Settings.PDFLoadZoom);

	options.SetValue(i++, "%i", Settings.KeyboardDeleteDelay);

	options.SetValue(i++, "%s", Settings.Rumble ? tr("ON") : tr("OFF"));

	options.SetValue(i++, "%i", Settings.ScrollSpeed);

	options.SetValue(i++, "%i ms", Settings.TooltipDelay);

	options.SetValue(i++, "%i %s", Settings.CompressionLevel, CompressionSynonym(Settings.CompressionLevel));

	options.SetValue(i++, PrioritySynonym(Settings.CopyThreadPrio));

	options.SetValue(i++, PrioritySynonym(Settings.CopyThreadBackPrio));

	options.SetValue(i++, "%s", Settings.ShowFormatter ? tr("ON") : tr("OFF"));

	iosinfo_t *info = IosLoader::GetIOSInfo(IOS_GetVersion());
	if(info && (info->version >= 9)) {
		options.SetValue(i++, "%s", Settings.USBPort ? tr("ON") : tr("OFF"));
	}
	else {
		options.SetValue(i++, tr("requires d2x cIOS v9+"));
	}

	options.SetValue(i++, "%s", Settings.OverridePriiloader ? tr("ON") : tr("OFF"));
}

void GeneralSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	char entered[300];

	switch (option)
	{
		case 0:
		{
			this->hide();
			LanguageSettingsMenu *menu = new LanguageSettingsMenu(this);
			Application::Instance()->Append(menu);
			break;
		}
		case 1:
		{
			Settings.ClockMode = (Settings.ClockMode+1) % 2;
			break;
		}
		case 2:
		{
			snprintf(entered, sizeof(entered), "%0.2f", Settings.PDFLoadZoom);
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.PDFLoadZoom = atof(entered);
				WindowPrompt(tr("Warning:"), tr("This option could mess up the pdf view."), tr("OK"));
			}
			break;
		}
		case 3:
		{
			snprintf(entered, sizeof(entered), "%i", Settings.KeyboardDeleteDelay);
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.KeyboardDeleteDelay = atoi(entered);
			}
			break;
		}
		case 4:
		{
			Settings.Rumble = (Settings.Rumble+1) % 2;
			break;
		}
		case 5:
		{
			Settings.ScrollSpeed = (Settings.ScrollSpeed+1) % 21;
			break;
		}
		case 6:
		{
			snprintf(entered, sizeof(entered), "%i", Settings.TooltipDelay);
			if(OnScreenKeyboard(entered, sizeof(entered)))
				Settings.TooltipDelay = atoi(entered);
			break;
		}
		case 7:
		{
			Settings.CompressionLevel++;
			if(Settings.CompressionLevel > 9)
				Settings.CompressionLevel = -1;
			break;
		}
		case 8:
		{
			Settings.CopyThreadPrio = NextPriority(Settings.CopyThreadPrio);
			break;
		}
		case 9:
		{
			Settings.CopyThreadBackPrio = NextPriority(Settings.CopyThreadBackPrio);
			break;
		}
		case 10:
		{
			Settings.ShowFormatter = (Settings.ShowFormatter+1) % 2;
			break;
		}
		case 11:
		{
			iosinfo_t *info = IosLoader::GetIOSInfo(IOS_GetVersion());
			if(info && (info->version >= 9)) {
				Settings.USBPort = ((Settings.USBPort+1) % 2);
			}
			else {
				Settings.USBPort = OFF;
			}
			break;
		}
		case 12:
		{
			Settings.OverridePriiloader = (Settings.OverridePriiloader+1) % 2;
			break;
		}
		default:
			break;
	}

	SetOptionValues();
}
