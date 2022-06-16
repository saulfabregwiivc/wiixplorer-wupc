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
#ifndef _IMAGE_CONVERTER_H_
#define _IMAGE_CONVERTER_H_

#include "stdafx.h"
#include "ImageWrite.h"

enum
{
	NONE,
	FLIP_VERTICAL,
	FLIP_HORIZONTAL,
	FLIP_BOTH
};

class ImageConverter
{
	public:
		ImageConverter(const char * filepath);
		ImageConverter(const u8 * imgBuf, int imgSize);
		virtual ~ImageConverter();
		virtual bool LoadImage(const char * filepath);
		virtual bool LoadImage(const u8 * img, int imgSize);
		bool Convert();
		bool RotateImage(int angle);
		bool ResizeImage(int newwidth, int newheight);
		void FlipHorizontal();
		void FlipVertical();
		void FlipBoth();
		void SetOutPath(const char * filepath);
		gdImagePtr GetImagePtr();
		void ResetOptions();

		bool WriteImage(const char * filepath);

	protected:
		void Setup();
		void ClearMemory();

		gdImagePtr gdImage;
		u8 InputType;
		std::string ImagePath;
		std::string OutPath;

		/** Options **/
		int OutputWidth;
		int OutputHeight;
		int Angle;
		u8 FlipMode;
		u8 Interlace;
		u8 AlphaBlending;
		u8 SaveAlpha;
		u8 OutputType;
		u8 JPEG_Quality;
		u8 BMP_Compression;
		u8 GD2_Compression;
};

#endif
