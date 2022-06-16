/****************************************************************************
 * Copyright (C) 2012 Dimok
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
#ifndef _THROBBERWINDOW_H_
#define _THROBBERWINDOW_H_

#include "PromptWindow.h"

class ThrobberWindow : public PromptWindow
{
	public:
		ThrobberWindow(const char *title, const char *msg = NULL, const char *btnText = NULL);
		virtual ~ThrobberWindow();
		//! Enable/disable automatically rotated throbber by 3 degree each frame (default enabled)
		void SetAutoRotate(bool b) { bAutoRotate = b; }
		//! Set custom throbber angle
		void SetThrobberAngle(int i) { throbberImg->SetAngle(i % 360); }
		//! Advance throbber rotation by 3 degree
		void AdvanceRotation(void)
		{
			int iThrobberAngle = throbberImg->GetAngle() + 3;
			if(iThrobberAngle >= 360)
				iThrobberAngle -= 360;

			throbberImg->SetAngle(iThrobberAngle);
		}
		void Draw();
	protected:
		bool bAutoRotate;

		GuiImageData *throbberImgData;
		GuiImage *throbberImg;
};

#endif
