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
#include "PictureButton.h"
#include "Memory/Resources.h"

PictureButton::PictureButton(const char *img, const char *imgOver, GuiSound * sndClick, GuiSound *sndOver)
	: GuiButton(0, 0)
{
	Init(img, imgOver, sndClick, sndOver);
}

PictureButton::PictureButton(GuiImageData *img)
	: GuiButton(img->GetWidth(), img->GetHeight())
{
	Init(img, NULL, NULL, NULL);
}

PictureButton::PictureButton(GuiImageData *img, GuiImageData *imgOver)
	: GuiButton(img->GetWidth(), img->GetHeight())
{
	Init(img, imgOver, NULL, NULL);
}

PictureButton::PictureButton(GuiImageData *img, GuiImageData *imgOver, GuiSound * sndClick)
	: GuiButton(img->GetWidth(), img->GetHeight())
{
	Init(img, imgOver, sndClick, NULL);
}

PictureButton::PictureButton(GuiImageData *img, GuiImageData *imgOver, GuiSound * sndClick, GuiSound * sndOver)
	: GuiButton(img->GetWidth(), img->GetHeight())
{
	Init(img, imgOver, sndClick, sndOver);
}

PictureButton::~PictureButton()
{
	if(image)
		Resources::Remove(imgData);
	if(imageOver)
		Resources::Remove(imgOverData);

	image = NULL;
	imageOver = NULL;

}

void PictureButton::Init(const char *img, const char *imgOver, GuiSound * sndClick, GuiSound * sndOver)
{
	imgData = Resources::GetImageData(img);
	imgOverData = Resources::GetImageData(imgOver);

	width = imgData->GetWidth();
	height = imgData->GetHeight();

	Init(imgData, imgOverData, sndClick, sndOver);
}

void PictureButton::Init(GuiImageData *img, GuiImageData *imgOver, GuiSound * sndClick, GuiSound * sndOver)
{
	image = new GuiImage(img);
	SetImage(image);

	if (imgOver != NULL)
	{
		imageOver = new GuiImage(imgOver);
		SetImageOver(imageOver);

		SetEffectGrow();
	}

	if (sndClick != NULL)
	{
		SetSoundClick(sndClick);
	}
	if (sndOver != NULL)
	{
		SetSoundOver(sndOver);
	}
}
