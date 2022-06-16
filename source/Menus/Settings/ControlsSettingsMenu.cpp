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
#include "ControlsSettingsMenu.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/ControlsSetupWindow.h"
#include "Controls/AppControls.hpp"
#include "Controls/Application.h"
#include "Settings.h"

ControlsSettingsMenu::ControlsSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("Controls Settings"), r)
{
	resetBtnTxt = new GuiText(tr("Reset"), 22, (GXColor){0, 0, 0, 255});
	resetBtnImg = new GuiImage(btnOutline);
	resetBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	resetBtn->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	resetBtn->SetPosition(100+btnOutline->GetWidth()/2, -65);
	resetBtn->SetLabel(resetBtnTxt);
	resetBtn->SetImage(resetBtnImg);
	resetBtn->SetSoundOver(btnSoundOver);
	resetBtn->SetTrigger(trigA);
	resetBtn->SetEffectGrow();
	resetBtn->Clicked.connect(this, &ControlsSettingsMenu::OnResetButtonClick);
	Append(resetBtn);

	SetupOptions();
}

ControlsSettingsMenu::~ControlsSettingsMenu()
{
	Remove(resetBtn);
	delete resetBtnTxt;
	delete resetBtnImg;
	delete resetBtn;
}

void ControlsSettingsMenu::OnResetButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	int choice = WindowPrompt(tr("Do you want to reset the control settings?"), 0, tr("Yes"), tr("Cancel"));
	if(choice)
	{
		Settings.Controls.SetDefault();
		SetOptionValues();
	}
}

void ControlsSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Screen Pointer Speed"));
	options.SetName(i++, tr("Screenshot Hold Button"));
	options.SetName(i++, tr("Screenshot Press Button"));
	options.SetName(i++, tr("Click Button"));
	options.SetName(i++, tr("Click Button"));
	options.SetName(i++, tr("Click Button"));
	options.SetName(i++, tr("Back Button"));
	options.SetName(i++, tr("Back Button"));
	options.SetName(i++, tr("Back Button"));
	options.SetName(i++, tr("Up Button"));
	options.SetName(i++, tr("Up Button"));
	options.SetName(i++, tr("Up Button"));
	options.SetName(i++, tr("Down Button"));
	options.SetName(i++, tr("Down Button"));
	options.SetName(i++, tr("Down Button"));
	options.SetName(i++, tr("Left Button"));
	options.SetName(i++, tr("Left Button"));
	options.SetName(i++, tr("Left Button"));
	options.SetName(i++, tr("Right Button"));
	options.SetName(i++, tr("Right Button"));
	options.SetName(i++, tr("Right Button"));
	options.SetName(i++, tr("Context Menu Button"));
	options.SetName(i++, tr("Context Menu Button"));
	options.SetName(i++, tr("Context Menu Button"));
	options.SetName(i++, tr("Mark Item Button"));
	options.SetName(i++, tr("Mark Item Button"));
	options.SetName(i++, tr("Mark Item Button"));
	options.SetName(i++, tr("Demark Item Button"));
	options.SetName(i++, tr("Demark Item Button"));
	options.SetName(i++, tr("Demark Item Button"));
	options.SetName(i++, tr("Demark All Items Button"));
	options.SetName(i++, tr("Demark All Items Button"));
	options.SetName(i++, tr("Demark All Items Button"));
	options.SetName(i++, tr("Home Button"));
	options.SetName(i++, tr("Home Button"));
	options.SetName(i++, tr("Home Button"));
	options.SetName(i++, tr("Edit Textline Button"));
	options.SetName(i++, tr("Edit Textline Button"));
	options.SetName(i++, tr("Edit Textline Button"));
	options.SetName(i++, tr("Slide Show Button"));
	options.SetName(i++, tr("Slide Show Button"));
	options.SetName(i++, tr("Slide Show Button"));
	options.SetName(i++, tr("Keyboard Backspace Button"));
	options.SetName(i++, tr("Keyboard Backspace Button"));
	options.SetName(i++, tr("Keyboard Backspace Button"));
	options.SetName(i++, tr("Keyboard Shift Button"));
	options.SetName(i++, tr("Keyboard Shift Button"));
	options.SetName(i++, tr("Keyboard Shift Button"));
	options.SetName(i++, tr("ZoomIn Button"));
	options.SetName(i++, tr("ZoomIn Button"));
	options.SetName(i++, tr("ZoomIn Button"));
	options.SetName(i++, tr("ZoomOut Button"));
	options.SetName(i++, tr("ZoomOut Button"));
	options.SetName(i++, tr("ZoomOut Button"));
	options.SetName(i++, tr("Back in Directory Button"));
	options.SetName(i++, tr("Back in Directory Button"));
	options.SetName(i++, tr("Back in Directory Button"));
	options.SetName(i++, tr("Button Hold Scroll"));
	options.SetName(i++, tr("Button Hold Scroll"));
	options.SetName(i++, tr("Button Hold Scroll"));

	SetOptionValues();
}

void ControlsSettingsMenu::SetOptionValues()
{
	int i = 0;

	options.SetValue(i++, "%g", Settings.PointerSpeed);

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, Settings.Controls.ScreenshotHoldButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, Settings.Controls.ScreenshotClickButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.ClickButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.ClickButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.ClickButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.BackButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.BackButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.BackButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.UpButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.UpButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.UpButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.DownButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.DownButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.DownButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.LeftButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.LeftButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.LeftButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.RightButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.RightButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.RightButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.ContextMenuButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.ContextMenuButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.ContextMenuButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.MarkItemButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.MarkItemButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.MarkItemButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.DeMarkItemButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.DeMarkItemButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.DeMarkItemButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.DeMarkAllButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.DeMarkAllButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.DeMarkAllButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.HomeButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.HomeButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.HomeButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.EditTextLine).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.EditTextLine << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.EditTextLine).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.SlideShowButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.SlideShowButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.SlideShowButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.KeyBackspaceButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.KeyBackspaceButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.KeyBackspaceButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.KeyShiftButton).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.KeyShiftButton << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.KeyShiftButton).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.ZoomIn).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.ZoomIn << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.ZoomIn).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.ZoomOut).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.ZoomOut << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.ZoomOut).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.UpInDirectory).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.UpInDirectory << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.UpInDirectory).c_str());

	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiMote, WiiControls.OneButtonScroll).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeWiiClassic, ClassicControls.OneButtonScroll << 16).c_str());
	options.SetValue(i++, "%s", AppControls::ControlButtonsToString(AppControls::TypeGCPad, GCControls.OneButtonScroll).c_str());
}

void ControlsSettingsMenu::OnControlsSetupWindowClose(GuiFrame *frame UNUSED)
{
	SetOptionValues();
	this->SetState(STATE_DEFAULT);
}

void ControlsSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int iClick)
{
	int iOption = -1;
	ControlsSetupWindow *window = 0;

	if(++iOption == iClick)
	{
		char entered[50];
		snprintf(entered, sizeof(entered), "%g", Settings.PointerSpeed);
		int result = OnScreenKeyboard(entered, sizeof(entered));
		if(result)
			Settings.PointerSpeed = atof(entered);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &Settings.Controls.ScreenshotHoldButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &Settings.Controls.ScreenshotClickButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.ClickButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.ClickButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.ClickButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.BackButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.BackButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.BackButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.UpButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.UpButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.UpButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.DownButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.DownButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.DownButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.LeftButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.LeftButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.LeftButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.RightButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.RightButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.RightButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.ContextMenuButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.ContextMenuButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.ContextMenuButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.MarkItemButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.MarkItemButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.MarkItemButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.DeMarkItemButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.DeMarkItemButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.DeMarkItemButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.DeMarkAllButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.DeMarkAllButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.DeMarkAllButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.HomeButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.HomeButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.HomeButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.EditTextLine);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.EditTextLine);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.EditTextLine);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.SlideShowButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.SlideShowButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.SlideShowButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.KeyBackspaceButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.KeyBackspaceButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.KeyBackspaceButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.KeyShiftButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.KeyShiftButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.KeyShiftButton);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.ZoomIn);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.ZoomIn);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.ZoomIn);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.ZoomOut);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.ZoomOut);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.ZoomOut);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.UpInDirectory);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.UpInDirectory);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.UpInDirectory);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiMote, &WiiControls.OneButtonScroll);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeWiiClassic, &ClassicControls.OneButtonScroll);
	}
	else if(++iOption == iClick)
	{
		window = new ControlsSetupWindow(AppControls::TypeGCPad, &GCControls.OneButtonScroll);
	}

	if(window != 0)
	{
		window->Closing.connect(this, &ControlsSettingsMenu::OnControlsSetupWindowClose);
		window->DimBackground(true);
		this->SetState(STATE_DISABLED);

		Application::Instance()->Append(window);
		Application::Instance()->SetUpdateOnly(window);
	}

	SetOptionValues();
}
