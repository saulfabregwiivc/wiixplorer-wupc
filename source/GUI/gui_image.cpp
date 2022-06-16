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
#include "gui_image.h"
#include "VideoOperations/video.h"
/**
 * Constructor for the GuiImage class.
 */
GuiImage::GuiImage()
{
	image = NULL;
	width = 0;
	height = 0;
	imageangle = 0;
	tileHorizontal = -1;
	tileVertical = -1;
	stripe = 0;
	format = GX_TF_RGBA8;
	color[0] = (GXColor){0, 0, 0, 255};
	AnimGif = NULL;
	imgType = IMAGE_DATA;
}

GuiImage::GuiImage(GuiImageData * img)
{
	image = NULL;
	width = 0;
	height = 0;
	AnimGif = NULL;
	format = GX_TF_RGBA8;
	if(img)
	{
		image = img->GetImage();
		width = img->GetWidth();
		height = img->GetHeight();
		format = img->GetTextureFormat();
		AnimGif = img->GetAnimGif();
		if(AnimGif)
		{
			image = NULL;
			width = AnimGif->GetWidth();
			height = AnimGif->GetHeight();
		}
	}
	imageangle = 0;
	tileHorizontal = -1;
	tileVertical = -1;
	stripe = 0;
	color[0] = (GXColor){0, 0, 0, 255};
	imgType = IMAGE_DATA;
}

GuiImage::GuiImage(u8 * img, int w, int h)
{
	image = img;
	width = w;
	height = h;
	imageangle = 0;
	tileHorizontal = -1;
	tileVertical = -1;
	stripe = 0;
	format = GX_TF_RGBA8;
	color[0] = (GXColor){0, 0, 0, 255};
	AnimGif = NULL;
	imgType = IMAGE_TEXTURE;
}

GuiImage::GuiImage(int w, int h, GXColor c)
{
	image = NULL;
	width = w;
	height = h;
	imageangle = 0;
	tileHorizontal = -1;
	tileVertical = -1;
	stripe = 0;
	format = GX_TF_RGBA8;
	imgType = IMAGE_COLOR;
	AnimGif = NULL;
	color[0] = c;
}

GuiImage::GuiImage(int w, int h, GXColor * c)
{
	image = NULL;
	width = w;
	height = h;
	imageangle = 0;
	tileHorizontal = -1;
	tileVertical = -1;
	stripe = 0;
	format = GX_TF_RGBA8;
	imgType = IMAGE_MULTICOLOR;
	AnimGif = NULL;

	for(int i = 0; i < 4; i++)
		color[i] = c[i];

	color[4] = (GXColor){0, 0, 0, 0};
}

/**
 * Destructor for the GuiImage class.
 */
GuiImage::~GuiImage()
{
}

u8 * GuiImage::GetImage()
{
	return image ? image : AnimGif ? AnimGif->GetFrameImage(0) : NULL;
}

void GuiImage::SetImage(GuiImageData * img)
{
	image = NULL;
	width = 0;
	height = 0;
	format = GX_TF_RGBA8;
	if(img)
	{
		width = img->GetWidth();
		height = img->GetHeight();
		format = img->GetTextureFormat();
		AnimGif = img->GetAnimGif();
		image = img->GetImage();

		if(AnimGif)
		{
			image = NULL;
			width = AnimGif->GetWidth();
			height = AnimGif->GetHeight();
		}
	}
	imgType = IMAGE_DATA;
}

void GuiImage::SetImage(u8 * img, int w, int h, int f)
{
	width = w;
	height = h;
	format = f;

	AnimGif = NULL;
	image = img;
	imgType = IMAGE_TEXTURE;
}

float GuiImage::GetAngle()
{
	return imageangle;
}

void GuiImage::SetAngle(float a)
{
	imageangle = a;
}

void GuiImage::SetTileHorizontal(int t)
{
	tileHorizontal = t;
}

void GuiImage::SetTileVertical(int t)
{
	tileVertical = t;
}

GXColor GuiImage::GetPixel(int x, int y)
{
	if(!image || this->GetWidth() <= 0 || x < 0 || y < 0)
		return (GXColor){0, 0, 0, 0};

	u32 offset = (((y >> 2)<<4)*this->GetWidth()) + ((x >> 2)<<6) + (((y%4 << 2) + x%4 ) << 1);
	GXColor color;
	color.a = *(image+offset);
	color.r = *(image+offset+1);
	color.g = *(image+offset+32);
	color.b = *(image+offset+33);
	return color;
}

void GuiImage::SetPixel(int x, int y, GXColor color)
{
	if(!image || this->GetWidth() <= 0 || x < 0 || y < 0)
		return;

	u32 offset = (((y >> 2)<<4)*this->GetWidth()) + ((x >> 2)<<6) + (((y%4 << 2) + x%4 ) << 1);
	*(image+offset) = color.a;
	*(image+offset+1) = color.r;
	*(image+offset+32) = color.g;
	*(image+offset+33) = color.b;
}

void GuiImage::SetStripe(int s)
{
	stripe = s;
}

void GuiImage::SetImageColor(GXColor * c, int colorCount)
{
	for(int i = 0; i < colorCount; i++)
		color[i] = c[i];
}

void GuiImage::SetSize(int w, int h)
{
	if(imgType != IMAGE_COLOR && imgType != IMAGE_MULTICOLOR)
		return;

	width = w;
	height = h;
}

void GuiImage::ColorStripe(int shift)
{
	int x, y;
	GXColor color;
	int alt = 0;

	for(y=0; y < this->GetHeight(); y++)
	{
		if(y % 3 == 0)
			alt ^= 1;

		for(x=0; x < this->GetWidth(); x++)
		{
			color = GetPixel(x, y);

			if(alt)
			{
				if(color.r < 255-shift)
					color.r += shift;
				else
					color.r = 255;
				if(color.g < 255-shift)
					color.g += shift;
				else
					color.g = 255;
				if(color.b < 255-shift)
					color.b += shift;
				else
					color.b = 255;

				color.a = 255;
			}
			else
			{
				if(color.r > shift)
					color.r -= shift;
				else
					color.r = 0;
				if(color.g > shift)
					color.g -= shift;
				else
					color.g = 0;
				if(color.b > shift)
					color.b -= shift;
				else
					color.b = 0;

				color.a = 255;
			}
			SetPixel(x, y, color);
		}
	}
}

/**
 * Draw the button on screen
 */
void GuiImage::Draw()
{
	if(!this->IsVisible() || tileVertical == 0 || tileHorizontal == 0)
		return;

	bool bUnCut = (cutBoundsRect.x1() == 0xBADBABE) || (cutBoundsRect.y1() == 0xBADBABE) ||
				  (cutBoundsRect.x2() == 0xBADBABE) || (cutBoundsRect.y2() == 0xBADBABE);

	int currLeft = this->GetLeft();
	int currTop = this->GetTop();
	int currZ = this->GetZPosition();
	float currScaleX = this->GetScaleX();
	float currScaleY = this->GetScaleY();
	u8 currAlpha = this->GetAlpha();

	if(image && tileHorizontal > 0 && tileVertical > 0)
	{
		for(int n=0; n<tileVertical; n++)
			for(int i=0; i<tileHorizontal; i++)
			{
				if(bUnCut)
					Menu_DrawImg(image, width, height, format, currLeft+width*i, currTop+width*n, currZ, imageangle, currScaleX, currScaleY, currAlpha);
				else
					Menu_DrawImgCut(image, width, height, format, currLeft+width*i, currTop+width*n, currZ, imageangle, currScaleX, currScaleY, currAlpha, cutBoundsRect.x1(), cutBoundsRect.x2(), cutBoundsRect.y1(), cutBoundsRect.y2());
			}
	}
	else if(image && tileHorizontal > 0)
	{
		for(int i=0; i<tileHorizontal; i++)
		{
			int widthTile = (imageangle == 90 || imageangle == 270) ? height : width;
			if(bUnCut)
				Menu_DrawImg(image, width, height, format, currLeft+widthTile*i, currTop, currZ, imageangle, currScaleX, currScaleY, currAlpha);
			else
				Menu_DrawImgCut(image, width, height, format, currLeft+widthTile*i, currTop, currZ, imageangle, currScaleX, currScaleY, currAlpha, cutBoundsRect.x1(), cutBoundsRect.x2(), cutBoundsRect.y1(), cutBoundsRect.y2());
		}
	}
	else if(image && tileVertical > 0)
	{
		for(int i=0; i<tileVertical; i++)
		{
			if(bUnCut)
				Menu_DrawImg(image, width, height, format, currLeft, currTop+height*i, currZ, imageangle, currScaleX, currScaleY, currAlpha);
			else
				Menu_DrawImgCut(image, width, height, format, currLeft, currTop+height*i, currZ, imageangle, currScaleX, currScaleY, currAlpha, cutBoundsRect.x1(), cutBoundsRect.x2(), cutBoundsRect.y1(), cutBoundsRect.y2());
		}
	}
	else if(imgType == IMAGE_COLOR)
	{
		Menu_DrawRectangle(currLeft,currTop, currZ, width, height, &color[0], false, true);
	}
	else if(imgType == IMAGE_MULTICOLOR)
	{
		Menu_DrawRectangle(currLeft,currTop, currZ, width, height, &color[0], true, true);
	}
	else if(AnimGif)
	{
		AnimGif->Draw(currLeft, currTop, currZ, imageangle, currScaleX, currScaleY,
					  GetAlpha(), cutBoundsRect.x1(), cutBoundsRect.x2(), cutBoundsRect.y1(), cutBoundsRect.y2());
	}
	else if(image)
	{
		if(bUnCut)
			Menu_DrawImg(image, width, height, format, currLeft, currTop, currZ, imageangle, currScaleX, currScaleY, currAlpha);
		else
			Menu_DrawImgCut(image, width, height, format, currLeft, currTop, currZ, imageangle, currScaleX, currScaleY, currAlpha, cutBoundsRect.x1(), cutBoundsRect.x2(), cutBoundsRect.y1(), cutBoundsRect.y2());
	}

	if(stripe > 0)
	{
		GXColor stripeColor = (GXColor) {0, 0, 0, stripe};
		for(int y=0; y < height; y+=6)
			Menu_DrawRectangle(currLeft, currLeft+y, currZ, width, 3, &stripeColor, false, true);
	}

	this->UpdateEffects();
}
