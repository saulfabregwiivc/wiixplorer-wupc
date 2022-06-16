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
#include "Prompts/ControlsSetupWindow.h"
#include "Prompts/PromptWindows.h"
#include "Controls/Application.h"
#include "Controls/AppControls.hpp"

ControlsSetupWindow::ControlsSetupWindow(int type, u16 * button)
	: PromptWindow((char*)NULL, tr("Press any button(s)"), tr("Cancel"))
	, controlType(type)
	, controlButton(button)
	, bCancelClicked(false)
{
	ButtonClicked.connect(this, &ControlsSetupWindow::OnCancelButtonClick);

	switch(controlType)
	{
		default:
		case AppControls::TypeWiiMote:
			SetTitle(tr("WiiMote Buttons Setup"));
			break;

		case AppControls::TypeWiiClassic:
			SetTitle(tr("Wii Classic Buttons Setup"));
			break;

		case AppControls::TypeGCPad:
			SetTitle(tr("GC Pad Buttons Setup"));
			break;
	}
}

void ControlsSetupWindow::OnCancelButtonClick(PromptWindow *window UNUSED, int iButton UNUSED)
{
	bCancelClicked = true;
	Application::Instance()->PushForDelete(this);
	Application::Instance()->UnsetUpdateOnly(this);
}

void ControlsSetupWindow::OnButtonClick(GuiTrigger *t)
{
	u32 buttons = 0;
	std::string controlText;

	if(controlType == AppControls::TypeWiiMote && (t->wpad.btns_d & 0xFFFF) != 0)
	{
		buttons = t->wpad.btns_d & 0xFFFF;
		controlText = AppControls::ControlButtonsToString(controlType, buttons);

	}
	else if(controlType == AppControls::TypeWiiClassic && (t->wpad.btns_d >> 16) != 0 && t->wpad.exp.type == WPAD_EXP_CLASSIC)
	{
		buttons = t->wpad.btns_d >> 16;
		controlText = AppControls::ControlButtonsToString(controlType, buttons << 16);
	}
	else if(controlType == AppControls::TypeGCPad && t->pad.btns_d != 0)
	{
		buttons = t->pad.btns_d & 0xFFFF;
		controlText = AppControls::ControlButtonsToString(controlType, buttons);
	}

	if(buttons != 0)
	{
		int choice = WindowPrompt(tr("Set the pressed buttons?"), controlText.c_str(), tr("Yes"), tr("No"));
		if(choice == 1)
		{
			if(controlButton)
				*controlButton = buttons;
			Application::Instance()->PushForDelete(this);
			Application::Instance()->UnsetUpdateOnly(this);
		}
	}
}

void ControlsSetupWindow::Update(GuiTrigger *t)
{
	if(!t || state == STATE_DISABLED)
		return;

	PromptWindow::Update(t);

	if((t->wpad.btns_d != 0 || t->pad.btns_d != 0) && !bCancelClicked)
	{
		OnButtonClick(t);
	}
}
