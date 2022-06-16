/****************************************************************************
 * Copyright (C) 2013 Dimok
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
#ifndef _CONTROLSSETUPWINDOW_H_
#define _CONTROLSSETUPWINDOW_H_

#include "Prompts/PromptWindow.h"

class ControlsSetupWindow : public PromptWindow
{
	public:
		ControlsSetupWindow(int controlType, u16 * controlButton);
		virtual ~ControlsSetupWindow() {}
		void Update(GuiTrigger *t);
	protected:
		void OnButtonClick(GuiTrigger *t);
		void OnCancelButtonClick(PromptWindow *window, int iButton);

		int controlType;
		u16 * controlButton;
		bool bCancelClicked;
};

#endif
