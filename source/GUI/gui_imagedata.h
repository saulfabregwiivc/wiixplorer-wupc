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
#ifndef GUI_IMAGEDATA_H_
#define GUI_IMAGEDATA_H_

#include <gctypes.h>
#include <gd.h>
#include "ImageOperations/GifImage.hpp"

class GuiImageData
{
	public:
		//!Constructor
		GuiImageData();
		//!\param img Image data
		//!\param imgSize The image size
		GuiImageData(const u8 * img, int imgSize);
		//!Destructor
		virtual ~GuiImageData();
		//!Load image from buffer
		//!\param img Image data
		//!\param imgSize The image size
		void LoadImage(const u8 * img, int imgSize);
		//!Gets a pointer to the image data
		//!\return pointer to image data
		u8 * GetImage() { return data ? data : AnimGif ? AnimGif->GetFrameImage(0) : NULL; };
		//!Gets the image width
		//!\return image width
		int GetWidth() { return width; };
		//!Gets the image height
		//!\return image height
		int GetHeight() { return height; };
		//!Gets the texture format
		u8 GetTextureFormat() { return format; };
		//!Gets animated gif if present
		GifImage * GetAnimGif() { return AnimGif; };
	protected:
		void LoadTPL(const u8 *img, int imgSize);

		u8 * data; //!< Image data
		int height; //!< Height of image
		int width; //!< Width of image
		u8 format; //!< Texture format
		GifImage * AnimGif;
};

#endif
