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
#include "SettingsMenu.h"
#include "Settings.h"
#include "Controls/Application.h"
#include "Memory/Resources.h"

SettingsMenu::SettingsMenu(const char * title, GuiFrame *r)
	: GuiFrame(screenwidth, screenheight)
{
	returnFrame = r;

	btnSoundClick = Resources::GetSound("button_click.wav");
	btnSoundOver = Resources::GetSound("button_over.wav");

	btnOutline = Resources::GetImageData("button.png");
	btnOutlineOver = Resources::GetImageData("button_over.png");

	trigA = new SimpleGuiTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigB = new GuiTrigger();
	trigB->SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);

	backBtnTxt = new GuiText(tr("Go Back"), 22, (GXColor){0, 0, 0, 255});
	backBtnImg = new GuiImage(btnOutline);
	backBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	backBtn->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	backBtn->SetPosition(-105-btnOutline->GetWidth()/2, -65);
	backBtn->SetLabel(backBtnTxt);
	backBtn->SetImage(backBtnImg);
	backBtn->SetSoundClick(btnSoundOver);
	backBtn->SetSoundOver(btnSoundOver);
	backBtn->SetTrigger(trigA);
	backBtn->SetTrigger(trigB);
	backBtn->SetEffectGrow();
	backBtn->Clicked.connect(this, &SettingsMenu::OnBackButtonClick);

	optionBrowser = new GuiOptionBrowser(584, 248, &options);
	optionBrowser->SetPosition(0, 100);
	optionBrowser->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	optionBrowser->Clicked.connect(this, &SettingsMenu::OnOptionClick);

	titleTxt = new GuiText(title, 24, (GXColor){0, 0, 0, 255});
	titleTxt->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	titleTxt->SetPosition(-optionBrowser->GetWidth()/2+titleTxt->GetTextWidth()/2+20, optionBrowser->GetTop()-35);

	//! catch each finished effect event
	//EffectFinished.connect(this, &SettingsMenu::OnEffectFinish);

	Append(backBtn);
	Append(optionBrowser);
	Append(titleTxt);

	show();
}

SettingsMenu::~SettingsMenu()
{
	RemoveAll();

	Resources::Remove(btnSoundClick);
	Resources::Remove(btnSoundOver);

	Resources::Remove(btnOutline);
	Resources::Remove(btnOutlineOver);

	delete backBtnImg;

	delete backBtn;

	delete titleTxt;
	delete backBtnTxt;

	delete optionBrowser;

	delete trigA;
	delete trigB;
}

void SettingsMenu::show()
{
	SetEffect(EFFECT_FADE, 20);

	if(parentElement)
		((GuiFrame *) parentElement)->Append(this);
}

void SettingsMenu::hide()
{
	if(GetParent())
		((GuiFrame *) GetParent())->Remove(this);
}

void SettingsMenu::OnEffectFinish(GuiElement *menu)
{
	//! on hide effect remove the explorer from the application
	if((menu->GetEffect() & EFFECT_FADE) && effectAmount < 0)
	{
		if(menu->GetParent())
			((GuiFrame *) menu->GetParent())->Remove(menu);
	}
}

void SettingsMenu::CloseMenu()
{
	hide();

	if(returnFrame)
		returnFrame->show();

	Application::Instance()->PushForDelete(this);
}

void SettingsMenu::OnBackButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	CloseMenu();
}

