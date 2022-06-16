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
 * for WiiXplorer 2010
 ***************************************************************************/
#include <stdio.h>
#include <gctypes.h>
#include <malloc.h>
#include <gd.h>
#include "VideoOperations/video.h"
#include "ImageOperations/ImageWrite.h"
#include "FileOperations/fileops.h"
#include "TextureConverter.h"
#include "stdafx.h"

extern "C" bool Screenshot(const char * outpath, int format)
{
	int width = 0;
	int height = 0;

	u8 * frameBuf = Video_GetFrame(&width, &height);
	if(!frameBuf)
		return false;

	gdImagePtr gdImg = 0;

	YCbYCrToGD(frameBuf, width, height, &gdImg);

	free(frameBuf);
	frameBuf = NULL;

	if(gdImg == 0)
		return false;

	if(screenwidth != width || screenheight != height)
	{
		gdImagePtr dst = gdImageCreateTrueColor(screenwidth, screenheight);
		gdImageCopyResized(dst, gdImg, 0, 0, 0, 0, screenwidth, screenheight, width, height);

		gdImageDestroy(gdImg);
		gdImg = dst;
	}

	WriteGDImage(outpath, gdImg, format, 0);

	gdImageDestroy(gdImg);

	return 0;
}

bool Screenshot()
{
	time_t rawtime;
	time(&rawtime);
	struct tm * curtime = localtime(&rawtime);

	char Extension[6];

	switch(Settings.ScreenshotFormat)
	{
		default:
		case IMAGE_PNG:
			sprintf(Extension, ".png");
			break;
		case IMAGE_JPEG:
			sprintf(Extension, ".jpg");
			break;
		case IMAGE_GIF:
			sprintf(Extension, ".gif");
			break;
		case IMAGE_TIFF:
			sprintf(Extension, ".tif");
			break;
		case IMAGE_BMP:
			sprintf(Extension, ".bmp");
			break;
		case IMAGE_GD:
		case IMAGE_GD2:
			sprintf(Extension, ".gd");
			break;
	}

	char text[100];
	strftime (text, sizeof(text), "WiiXplorer_%H%M%S", curtime);

	char filepath[400];
	snprintf(filepath, sizeof(filepath), "%s/%s%s", Settings.ScreenshotPath, text, Extension);
	CreateSubfolder(Settings.ScreenshotPath);

	return Screenshot(filepath, Settings.ScreenshotFormat);
}
