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
#ifndef GUI_WINDOW_H_
#define GUI_WINDOW_H_

#include "gui_frame.h"
#include "gui_frameimage.h"

//!GuiFrame inheritance with a dynamic background image
class GuiWindow : public GuiFrame
{
	public:
		//!Constructor
		//!\param w Width of window
		//!\param h Height of window
		GuiWindow(int w, int h, GuiFrame *p = 0)
			: GuiFrame(w, h, p), frameImage(w, h)
		{
			Append(&frameImage);
		}
	protected:
		GuiFrameImage frameImage;
};

#endif
