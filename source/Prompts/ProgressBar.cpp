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
#include "ProgressBar.h"
#include "Memory/Resources.h"
#include "Settings.h"

ProgressBar::ProgressBar()
	: GuiFrame(0, 0)
{
	progressbarOutline = Resources::GetImageData("progressbar_outline.png");

	width = progressbarOutline->GetWidth();
	height = progressbarOutline->GetHeight();

	fPixPerPercent = (float) width / 100.0f;

	progressbarOutlineImg = new GuiImage(progressbarOutline);

	GXColor ImgColor[4];
	ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressEmptyUL);
	ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressEmptyUR);
	ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressEmptyBR);
	ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressEmptyBL);

	progressbarEmptyImg = new GuiImage(width, height, (GXColor *) &ImgColor);

	ImgColor[0] = RGBATOGXCOLOR(Settings.ProgressUL);
	ImgColor[1] = RGBATOGXCOLOR(Settings.ProgressUR);
	ImgColor[2] = RGBATOGXCOLOR(Settings.ProgressBR);
	ImgColor[3] = RGBATOGXCOLOR(Settings.ProgressBL);

	progressbarImg = new GuiImage(fPixPerPercent, 36, (GXColor *) &ImgColor);

	int OutLineMiddle = progressbarOutline->GetHeight()/2;

	prTxt = new GuiText("0.00", 20, (GXColor){0, 0, 0, 255});
	prTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	prTxt->SetPosition(0.45f*width, OutLineMiddle-9);

	prsTxt = new GuiText("%", 20, (GXColor){0, 0, 0, 255});
	prsTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	prsTxt->SetPosition(0.6f*width, OutLineMiddle-9);

	Append(progressbarEmptyImg);
	Append(progressbarImg);
	Append(progressbarOutlineImg);
	Append(prsTxt);
	Append(prTxt);
}

ProgressBar::~ProgressBar()
{
	Resources::Remove(progressbarOutline);
	delete progressbarOutlineImg;
	delete progressbarEmptyImg;
	delete progressbarImg;
	delete prTxt;
	delete prsTxt;
}

void ProgressBar::SetPercent(float Percent)
{
	if(Percent < 0.0f)
	{
		prTxt->SetText("0.00");
		progressbarImg->SetSize(fPixPerPercent, 36);

	}
	else if(Percent < 100.0f)
	{
		prTxt->SetTextf("%0.2f", Percent);
		progressbarImg->SetSize((int) (Percent*fPixPerPercent), 36);
	}
	else
	{
		prTxt->SetText("100");
		progressbarImg->SetSize(100.0f * fPixPerPercent, 36);
	}
}
