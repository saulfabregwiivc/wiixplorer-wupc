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
 * Window.h
 *
 * for WiiXplorer 2010
 ***************************************************************************/
#if 0 //currently not used

#ifndef _WINDOW_H
#define _WINDOW_H

#include "libwiigui/gui.h"
#include "libwiigui/PictureButton.h"
#include <string>

class Window : public GuiFrame
{
	public:
		Window();
		Window(int w, int h);
		~Window();

		void Draw();
		void SetIcon(GuiImageData *imgData);
		void SetTitle(std::string text);
		void SetHeight(int h);
		void SetWidth(int w);
		void SizeChanged();

		//!Focussed signal
		sigslot::signal1<Window *> Focussed;
	protected:
		GuiImageData *window_tile_horizontalDat;
		GuiImage *window_tile_horizontal_top;
		GuiImage *window_tile_horizontal_bottom;
		GuiImageData *window_tile_verticalDat;
		GuiImage *window_tile_vertical_left;
		GuiImage *window_tile_vertical_right;
		GuiImageData *window_corner_leftDat;
		GuiImage *window_corner_left_top;
		GuiImage *window_corner_left_bottom;
		GuiImageData *window_corner_rightDat;
		GuiImage *window_corner_right_top;
		GuiImage *window_corner_right_bottom;

		GuiFrame *titleBar;
		GuiButton *titleBarButton;
		GuiText *titleText;
		GuiImage *icon;

		GuiTrigger *trigA;
		GuiTrigger *trigHeldA;

		bool isMovable;
		bool isMoving;
		POINT clickedPoint;
		int clickedChannel;

	private:
		void OnTitleHeld(GuiElement *, int channel, POINT point);
		void OnTitleReleased(GuiElement *, int channel);

		static bool busyMoving;
};

#endif //_WINDOW_H

#endif
