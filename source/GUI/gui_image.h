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
#ifndef GUI_IMAGE_H_
#define GUI_IMAGE_H_

#include "gui_element.h"
#include "gui_imagedata.h"

enum
{
	IMAGE_TEXTURE,
	IMAGE_COLOR,
	IMAGE_MULTICOLOR,
	IMAGE_DATA
};

//!Display, manage, and manipulate images in the GUI
class GuiImage : public GuiElement
{
	public:
		//!Constructor
		GuiImage();
		//!\overload
		//!\param img Pointer to GuiImageData element
		GuiImage(GuiImageData * img);
		//!\overload
		//!Sets up a new image from the image data specified
		//!\param img
		//!\param w Image width
		//!\param h Image height
		GuiImage(u8 * img, int w, int h);
		//!\overload
		//!Creates an image filled with the specified color
		//!\param w Image width
		//!\param h Image height
		//!\param c Image color
		GuiImage(int w, int h, GXColor c);
		//!\overload
		//!Creates an image filled with multiple colors
		//!\param w Image width
		//!\param h Image height
		//!\param c Image color
		GuiImage(int w, int h, GXColor * c);
		//!Destructor
		virtual ~GuiImage();
		//!Gets the image rotation angle for drawing
		float GetAngle();
		//!Sets the image rotation angle for drawing
		//!\param a Angle (in degrees)
		void SetAngle(float a);
		//!Sets the number of times to draw the image horizontally
		//!\param t Number of times to draw the image
		void SetTileHorizontal(int t);
		//!Sets the number of times to draw the image vertically
		//!\param t Number of times to draw the image
		void SetTileVertical(int t);
		//!Constantly called to draw the image
		void Draw();
		//!Gets the image data
		//!\return pointer to image data
		u8 * GetImage();
		//!Sets up a new image using the GuiImageData object specified
		//!\param img Pointer to GuiImageData object
		void SetImage(GuiImageData * img);
		//!\overload
		//!\param img Pointer to image data
		//!\param w Width
		//!\param h Height
		//!\param f Texture format
		void SetImage(u8 * img, int w, int h, int f = GX_TF_RGBA8);
		//!Gets the pixel color at the specified coordinates of the image
		//!\param x X coordinate
		//!\param y Y coordinate
		GXColor GetPixel(int x, int y);
		//!Sets the pixel color at the specified coordinates of the image
		//!\param x X coordinate
		//!\param y Y coordinate
		//!\param color Pixel color
		void SetPixel(int x, int y, GXColor color);
		//!Directly modifies the image data to create a color-striped effect
		//!Alters the RGB values by the specified amount
		//!\param s Amount to increment/decrement the RGB values in the image
		void ColorStripe(int s);
		//!Sets a stripe effect on the image, overlaying alpha blended rectangles
		//!Does not alter the image data
		//!\param s Alpha amount to draw over the image
		void SetStripe(int s);
		//!Change ImageColor
		void SetImageColor(GXColor * c, int colorCount = 1);
		//!Change ImageColor
		void SetSize(int w, int h);
		//!Get the color pointer
		GXColor * GetColorPtr() { return (GXColor *) &color; }
		//!Set texture format
		void SetFormat(u8 f) { format = f; }
	protected:
		int imgType; //!< Type of image data (IMAGE_TEXTURE, IMAGE_COLOR, IMAGE_DATA)
		u8 * image; //!< Poiner to image data. May be shared with GuiImageData data
		f32 imageangle; //!< Angle to draw the image
		int tileHorizontal; //!< Number of times to draw (tile) the image horizontally
		int tileVertical; //!< Number of times to draw (tile) the image vertically
		int stripe; //!< Alpha value (0-255) to apply a stripe effect to the texture
		u8 format; //!< Texture format
		GXColor color[5];
		GifImage * AnimGif;
};

#endif
