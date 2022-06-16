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
#include "ImageSettingsMenu.h"
#include "Prompts/PromptWindows.h"
#include "ImageOperations/ImageWrite.h"
#include "Settings.h"

ImageSettingsMenu::ImageSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("Image Settings"), r)
{
	SetupOptions();
}

ImageSettingsMenu::~ImageSettingsMenu()
{
}

void ImageSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Slideshow Delay"));
	options.SetName(i++, tr("Screenshot Format"));
	options.SetName(i++, tr("Fade Speed"));

	SetOptionValues();
}

void ImageSettingsMenu::SetOptionValues()
{
	int i = 0;

	options.SetValue(i++, "%i", Settings.SlideshowDelay);

	if(Settings.ScreenshotFormat == IMAGE_PNG)
		options.SetValue(i++, "PNG");
	else if(Settings.ScreenshotFormat == IMAGE_JPEG)
		options.SetValue(i++, "JPEG");
	else if(Settings.ScreenshotFormat == IMAGE_GIF)
		options.SetValue(i++, "GIF");
	else if(Settings.ScreenshotFormat == IMAGE_TIFF)
		options.SetValue(i++, "TIFF");
	else if(Settings.ScreenshotFormat == IMAGE_BMP)
		options.SetValue(i++, "BMP");
	else if(Settings.ScreenshotFormat == IMAGE_GD)
		options.SetValue(i++, "GD");
	else if(Settings.ScreenshotFormat == IMAGE_GD2)
		options.SetValue(i++, "GD2");

	options.SetValue(i++, "%i", Settings.ImageFadeSpeed);

}

void ImageSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	char entered[300];

	switch (option)
	{
		case 0:
			snprintf(entered, sizeof(entered), "%i", Settings.SlideshowDelay);
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.SlideshowDelay = LIMIT(atoi(entered), 0, 999999);
			}
			break;
		case 1:
			Settings.ScreenshotFormat = (Settings.ScreenshotFormat+1) % 6;
			break;
		case 2:
			snprintf(entered, sizeof(entered), "%i", Settings.ImageFadeSpeed);
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.ImageFadeSpeed = LIMIT(atoi(entered), 1, 255);
			}
			break;
		default:
			break;
	}

	SetOptionValues();
}
