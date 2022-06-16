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
#ifndef GIFIMAGE_HPP_
#define GIFIMAGE_HPP_

#include <vector>
#include <gctypes.h>
#include "Tools/timer.h"

typedef struct
{
	u8 * image;
	int width;
	int height;
	int offsetx;
	int offsety;
	int Delay;
	int Disposal;
	bool Transparent;
} GifFrame;

class GifImage
{
	public:
		GifImage(const u8 * img, int imgSize);
		virtual ~GifImage();
		void LoadImage(const u8 * img, int imgSize);
		u8 * GetFrameImage(int pos);
		int GetWidth() { return MainWidth; };
		int GetHeight() { return MainHeight; };
		int GetFrameCount() { return Frames.size(); };
		void Draw(int x, int y, int z, int degrees, float scaleX,
				  float scaleY, int alpha, int minwidth, int maxwidth,
				  int minheight, int maxheight);
	protected:
		int MainWidth;
		int MainHeight;
		int last;
		int currentFrame;
		float lastTimer;
		Timer DelayTimer;
		std::vector<GifFrame> RedrawQueue;
		std::vector<GifFrame> Frames;
};

#endif
