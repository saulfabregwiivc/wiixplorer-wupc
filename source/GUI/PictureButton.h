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
#ifndef _PICTUREBUTTON_H
#define _PICTUREBUTTON_H

#include "GUI/gui_button.h"
#include "GUI/gui_image.h"

class PictureButton : public GuiButton
{
	public:
		PictureButton(const char *img = NULL, const char *imgOver = NULL, GuiSound * sndClick = NULL, GuiSound * sndOver = NULL);

		PictureButton(GuiImageData *img);
		PictureButton(GuiImageData *img, GuiImageData *imgOver);
		PictureButton(GuiImageData *img, GuiImageData *imgOver, GuiSound * sndClick);
		PictureButton(GuiImageData *img, GuiImageData *imgOver, GuiSound * sndClick, GuiSound * sndOver);

		virtual ~PictureButton();
	private:
		void Init(const char *img, const char *imgOver, GuiSound * sndClick, GuiSound * sndOver);
		void Init(GuiImageData *img, GuiImageData *imgOver, GuiSound * sndClick, GuiSound * sndOver);
		GuiImageData *imgData;
		GuiImageData *imgOverData;
};

#endif //_PICTUREBUTTON_H
