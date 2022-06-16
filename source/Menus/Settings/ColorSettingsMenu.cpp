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
#include "ColorSettingsMenu.h"
#include "Controls/Application.h"
#include "Prompts/ColorSetPrompt.h"
#include "Settings.h"

ColorSettingsMenu::ColorSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("Color Settings"), r)
{
	SetupOptions();
}

ColorSettingsMenu::~ColorSettingsMenu()
{
}


void ColorSettingsMenu::SetupOptions()
{
	int i = 0;
	options.SetName(i++, tr("Background Upper/Left"));
	options.SetName(i++, tr("Background Upper/Right"));
	options.SetName(i++, tr("Background Bottom/Right"));
	options.SetName(i++, tr("Background Bottom/Left"));
	options.SetName(i++, tr("ProgressBar Upper/Left"));
	options.SetName(i++, tr("ProgressBar Upper/Right"));
	options.SetName(i++, tr("ProgressBar Bottom/Right"));
	options.SetName(i++, tr("ProgressBar Bottom/Left"));
	options.SetName(i++, tr("ProgressBar Empty Upper/Left"));
	options.SetName(i++, tr("ProgressBar Empty Upper/Right"));
	options.SetName(i++, tr("ProgressBar Empty Bottom/Right"));
	options.SetName(i++, tr("ProgressBar Empty Bottom/Left"));

	SetOptionValues();
}

void ColorSettingsMenu::SetOptionValues()
{
	int i = 0;

	options.SetValue(i++, tr("R: %i G: %i B: %i"), Application::Instance()->GetBGColorPtr()[0].r,
								   	   	   	   	   Application::Instance()->GetBGColorPtr()[0].g,
								   	   	   	   	   Application::Instance()->GetBGColorPtr()[0].b);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), Application::Instance()->GetBGColorPtr()[1].r,
												   Application::Instance()->GetBGColorPtr()[1].g,
												   Application::Instance()->GetBGColorPtr()[1].b);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), Application::Instance()->GetBGColorPtr()[2].r,
												   Application::Instance()->GetBGColorPtr()[2].g,
												   Application::Instance()->GetBGColorPtr()[2].b);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), Application::Instance()->GetBGColorPtr()[3].r,
												   Application::Instance()->GetBGColorPtr()[3].g,
												   Application::Instance()->GetBGColorPtr()[3].b);
	GXColor ImgColor[4];
	ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressUL);
	ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressUR);
	ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressBR);
	ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressBL);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), ImgColor[0].r, ImgColor[0].g, ImgColor[0].b);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), ImgColor[1].r, ImgColor[1].g, ImgColor[1].b);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), ImgColor[2].r, ImgColor[2].g, ImgColor[2].b);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), ImgColor[3].r, ImgColor[3].g, ImgColor[3].b);

	ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressEmptyUL);
	ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressEmptyUR);
	ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressEmptyBR);
	ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressEmptyBL);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), ImgColor[0].r, ImgColor[0].g, ImgColor[0].b);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), ImgColor[1].r, ImgColor[1].g, ImgColor[1].b);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), ImgColor[2].r, ImgColor[2].g, ImgColor[2].b);

	options.SetValue(i++, tr("R: %i G: %i B: %i"), ImgColor[3].r, ImgColor[3].g, ImgColor[3].b);
}

void ColorSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	GXColor ImgColor[4];

	switch (option)
	{
		case 0:
			ColorSetPrompt::Show(tr("Background Upper/Left"), Application::Instance()->GetBGColorPtr(), 0);
			Settings.BackgroundUL = GXCOLORTORGBA(Application::Instance()->GetBGColorPtr()[0]);
			break;
		case 1:
			ColorSetPrompt::Show(tr("Background Upper/Right"), Application::Instance()->GetBGColorPtr(), 1);
			Settings.BackgroundUR = GXCOLORTORGBA(Application::Instance()->GetBGColorPtr()[1]);
			break;
		case 2:
			ColorSetPrompt::Show(tr("Background Bottom/Right"), Application::Instance()->GetBGColorPtr(), 2);
			Settings.BackgroundBR = GXCOLORTORGBA(Application::Instance()->GetBGColorPtr()[2]);
			break;
		case 3:
			ColorSetPrompt::Show(tr("Background Bottom/Left"), Application::Instance()->GetBGColorPtr(), 3);
			Settings.BackgroundBL = GXCOLORTORGBA(Application::Instance()->GetBGColorPtr()[3]);
			break;
		case 4:
			ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressUL);
			ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressUR);
			ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressBR);
			ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressBL);
			ColorSetPrompt::Show(tr("ProgressBar Upper/Left"), (GXColor *) &ImgColor, 0);
			Settings.ProgressUL = GXCOLORTORGBA(ImgColor[0]);
			break;
		case 5:
			ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressUL);
			ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressUR);
			ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressBR);
			ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressBL);
			ColorSetPrompt::Show(tr("ProgressBar Upper/Right"), (GXColor *) &ImgColor, 1);
			Settings.ProgressUR = GXCOLORTORGBA(ImgColor[1]);
			break;
		case 6:
			ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressUL);
			ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressUR);
			ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressBR);
			ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressBL);
			ColorSetPrompt::Show(tr("ProgressBar Bottom/Right"), (GXColor *) &ImgColor, 2);
			Settings.ProgressBR = GXCOLORTORGBA(ImgColor[2]);
			break;
		case 7:
			ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressUL);
			ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressUR);
			ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressBR);
			ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressBL);
			ColorSetPrompt::Show(tr("ProgressBar Bottom/Left"), (GXColor *) &ImgColor, 3);
			Settings.ProgressBL = GXCOLORTORGBA(ImgColor[3]);
			break;
		case 8:
			ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressEmptyUL);
			ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressEmptyUR);
			ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressEmptyBR);
			ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressEmptyBL);
			ColorSetPrompt::Show(tr("ProgressBar Upper/Left"), (GXColor *) &ImgColor, 0);
			Settings.ProgressEmptyUL = GXCOLORTORGBA(ImgColor[0]);
			break;
		case 9:
			ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressEmptyUL);
			ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressEmptyUR);
			ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressEmptyBR);
			ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressEmptyBL);
			ColorSetPrompt::Show(tr("ProgressBar Upper/Right"), (GXColor *) &ImgColor, 1);
			Settings.ProgressEmptyUR = GXCOLORTORGBA(ImgColor[1]);
			break;
		case 10:
			ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressEmptyUL);
			ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressEmptyUR);
			ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressEmptyBR);
			ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressEmptyBL);
			ColorSetPrompt::Show(tr("ProgressBar Bottom/Right"), (GXColor *) &ImgColor, 2);
			Settings.ProgressEmptyBR = GXCOLORTORGBA(ImgColor[2]);
			break;
		case 11:
			ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressEmptyUL);
			ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressEmptyUR);
			ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressEmptyBR);
			ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressEmptyBL);
			ColorSetPrompt::Show(tr("ProgressBar Bottom/Left"), (GXColor *) &ImgColor, 3);
			Settings.ProgressEmptyBL = GXCOLORTORGBA(ImgColor[3]);
			break;
		default:
			break;
	}

	SetOptionValues();
}
