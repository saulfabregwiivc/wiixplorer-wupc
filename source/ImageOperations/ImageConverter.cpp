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
#include "FileOperations/fileops.h"
#include "ImageConverter.hpp"
#include "TplImage.h"

#define IMAGE_TPL   7
#define IMAGE_TGA   8

ImageConverter::ImageConverter(const char * filepath)
{
	InputType = IMAGE_PNG;
	gdImage = 0;

	ResetOptions();
	LoadImage(filepath);
}

ImageConverter::ImageConverter(const u8 * imgBuf, int imgSize)
{
	InputType = IMAGE_PNG;
	gdImage = 0;

	ResetOptions();
	LoadImage(imgBuf, imgSize);
}

ImageConverter::~ImageConverter()
{
	ClearMemory();
}

bool ImageConverter::LoadImage(const char * filepath)
{
	ClearMemory();

	if(!filepath)
		return false;

	ImagePath = filepath;

	SetOutPath(filepath);

	u8 * buffer = NULL;
	u32 filesize = 0;

	LoadFileToMem(filepath, &buffer, &filesize);

	if(!buffer)
		return false;

	bool result = LoadImage(buffer, filesize);
	free(buffer);

	return result;
}

bool ImageConverter::LoadImage(const u8 * img, int imgSize)
{
	if (img[0] == 0xFF && img[1] == 0xD8)
	{
		gdImage = gdImageCreateFromJpegPtr(imgSize, (u8*) img);
		InputType = IMAGE_JPEG;
	}
	else if ((img[0] == 0x49 && img[1] == 0x49) || (img[0] == 0x4D && img[1] == 0x4D))
	{
		gdImage = gdImageCreateFromTiffPtr(imgSize, (u8*) img);
		InputType = IMAGE_TIFF;
	}
	else if (img[0] == 'B' && img[1] == 'M')
	{
		gdImage = gdImageCreateFromBmpPtr(imgSize, (u8*) img);
		InputType = IMAGE_BMP;
	}
	else if (img[0] == 'G' && img[1] == 'I' && img[2] == 'F')
	{
		gdImage = gdImageCreateFromGifPtr(imgSize, (u8*) img);
		InputType = IMAGE_GIF;
	}
	else if (img[0] == 0x89 && img[1] == 'P' && img[2] == 'N' && img[3] == 'G')
	{
		gdImage = gdImageCreateFromPngPtr(imgSize, (u8*) img);
		InputType = IMAGE_PNG;
	}
	else if ((img[0] == 0xFF && img[1] == 0xFF) || (img[0] == 0xFF && img[1] == 0xFE))
	{
		gdImage = gdImageCreateFromGdPtr(imgSize, (u8*) img);
		InputType = IMAGE_GD;
	}
	else if (img[0] == 0x67 && img[1] == 0x64 && img[2] == 0x32 && img[3] == 0x00)
	{
		gdImage = gdImageCreateFromGd2Ptr(imgSize, (u8*) img);
		InputType = IMAGE_GD2;
	}
	else if (img[0] == 0x00 && img[1] == 0x20 && img[2] == 0xAF && img[3] == 0x30)
	{
		TplImage TplFile(img, imgSize);

		gdImage = TplFile.ConvertToGD(0);
		InputType = IMAGE_TPL;
	}
	//!This must be last since it can also intefere with outher formats
	else if(img[0] == 0x00)
	{
		gdImage = gdImageCreateFromTgaPtr(imgSize, (u8*) img);
		InputType = IMAGE_TGA;
	}

	if(gdImage == 0)
	{
		ThrowMsg(tr("Could not load image."), 0);
		return false;
	}

	OutputWidth = gdImageSX(gdImage);
	OutputHeight = gdImageSY(gdImage);

	return true;
}

gdImagePtr ImageConverter::GetImagePtr()
{
	return gdImage;
}

bool ImageConverter::Convert()
{
	bool result = false;

	result = RotateImage(Angle);
	if(!result)
		return false;

	result = ResizeImage(OutputWidth, OutputHeight);
	if(!result)
		return false;

	if(FlipMode == FLIP_VERTICAL)
		FlipVertical();

	if(FlipMode == FLIP_HORIZONTAL)
		FlipHorizontal();

	if(FlipMode == FLIP_BOTH)
		FlipBoth();

	return WriteImage(OutPath.c_str());
}

bool ImageConverter::ResizeImage(int newwidth, int newheight)
{
	if(gdImage == 0)
		return false;

	int imgwidth = gdImageSX(gdImage);
	int imgheight = gdImageSY(gdImage);

	if(imgwidth != newwidth || newheight != newheight)
	{
		gdImagePtr dst = gdImageCreateTrueColor(newwidth, newheight);
		if(dst == 0)
		{
			ThrowMsg(tr("Not enough memory to resize."), 0);
			return false;
		}

		gdImageInterlace(dst, Interlace);
		gdImageAlphaBlending(dst, AlphaBlending);
		gdImageSaveAlpha(dst, SaveAlpha);

		gdImageCopyResized(dst, gdImage, 0, 0, 0, 0, newwidth, newheight, imgwidth, imgheight);
		gdImageDestroy(gdImage);
		gdImage = dst;
	}

	return true;
}

bool ImageConverter::RotateImage(int angle)
{
	if(gdImage == 0)
		return false;

	int imgwidth = gdImageSX(gdImage);
	int imgheight = gdImageSY(gdImage);

	if(angle != 0)
	{
		gdImagePtr dst = gdImageCreateTrueColor(OutputWidth, OutputHeight);
		if(dst == 0)
		{
			ThrowMsg(tr("Not enough memory to rotate."), 0);
			return false;
		}

		gdImageInterlace(dst, Interlace);
		gdImageAlphaBlending(dst, AlphaBlending);
		gdImageSaveAlpha(dst, SaveAlpha);

		gdImageCopyRotated (dst, gdImage, (double) OutputWidth/2.0, (double) OutputHeight/2.0, 0, 0, imgwidth, imgheight, angle);
		gdImageDestroy(gdImage);
		gdImage = dst;
	}

	return true;
}

void ImageConverter::FlipHorizontal()
{
	if(gdImage == 0)
		return;

	gdImageFlipHorizontal(gdImage);
}

void ImageConverter::FlipVertical()
{
	if(gdImage == 0)
		return;

	gdImageFlipVertical(gdImage);
}

void ImageConverter::FlipBoth()
{
	if(gdImage == 0)
		return;

	gdImageFlipBoth(gdImage);
}

bool ImageConverter::WriteImage(const char * filepath)
{
	if(gdImage == 0)
		return false;

	u8 compression = 0;

	if(OutputType == IMAGE_JPEG)
	{
		compression = 100-JPEG_Quality;
	}
	else if(OutputType == IMAGE_BMP)
	{
		compression = BMP_Compression;
	}
	else if(OutputType == IMAGE_GD2)
	{
		compression = GD2_Compression;
	}

	return WriteGDImage(filepath, gdImage, OutputType, compression);
}

void ImageConverter::ClearMemory()
{
	if(gdImage != 0)
		gdImageDestroy(gdImage);

	gdImage = 0;
}

void ImageConverter::ResetOptions()
{
	OutputType = IMAGE_PNG;
	FlipMode = NONE;
	Interlace = 0;
	AlphaBlending = 0;
	SaveAlpha = 1;
	JPEG_Quality = 100;
	BMP_Compression = 1;
	GD2_Compression = 1;
	Angle = 0;
	OutputWidth = 0;
	OutputHeight = 0;

	if(gdImage != 0)
	{
		OutputWidth = gdImageSX(gdImage);
		OutputHeight = gdImageSY(gdImage);
	}

	SetOutPath(NULL);
}

void ImageConverter::SetOutPath(const char * filepath)
{
	if(filepath)
		OutPath = filepath;

	else if(!ImagePath.empty() && OutPath.empty())
		OutPath = ImagePath;

	if(OutPath.empty())
		return;

	char Extension[6];
	memset(Extension, 0, sizeof(Extension));

	switch(OutputType)
	{
		case IMAGE_PNG:
			snprintf(Extension, sizeof(Extension), ".png");
			break;
		case IMAGE_JPEG:
			snprintf(Extension, sizeof(Extension), ".jpg");
			break;
		case IMAGE_GIF:
			snprintf(Extension, sizeof(Extension), ".gif");
			break;
		case IMAGE_TIFF:
			snprintf(Extension, sizeof(Extension), ".tif");
			break;
		case IMAGE_BMP:
			snprintf(Extension, sizeof(Extension), ".bmp");
			break;
		case IMAGE_GD:
		case IMAGE_GD2:
			snprintf(Extension, sizeof(Extension), ".gd");
			break;
		default:
			return;
	}

	size_t extPos = OutPath.rfind('.');

	if(extPos == std::string::npos)
		return;

	OutPath.erase(extPos);
	OutPath += Extension;
}
