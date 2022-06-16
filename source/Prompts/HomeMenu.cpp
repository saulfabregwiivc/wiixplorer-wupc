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
#include <unistd.h>

#include "HomeMenu.h"
#include "Controls/Application.h"
#include "Prompts/PromptWindows.h"
#include "SoundOperations/MusicPlayer.h"
#include "Memory/Resources.h"
#include "sys.h"


HomeMenu::HomeMenu()
	: GuiFrame(0, 0)
{
	this->SetPosition(0, 0);
	this->SetSize(screenwidth, screenheight);

	trigA = new SimpleGuiTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigHome = new GuiTrigger();
	trigHome->SetButtonOnlyTrigger(-1, WiiControls.HomeButton | ClassicControls.HomeButton << 16, GCControls.HomeButton);

	ButtonClickSnd = Resources::GetSound("button_click.wav");
	ButtonOverSnd = Resources::GetSound("button_over.wav");
	HomeInSnd = Resources::GetSound("menuin.ogg");
	HomeOutSnd = Resources::GetSound("menuout.ogg");

	TopBtnImgData = Resources::GetImageData("homemenu_top.png");
	TopBtnOverImgData = Resources::GetImageData("homemenu_top_over.png");
	BottomBtnImgData = Resources::GetImageData("homemenu_bottom.png");
	BottomBtnOverImgData = Resources::GetImageData("homemenu_bottom_over.png");
	CloseBtnImgData = Resources::GetImageData("homemenu_close.png");
	StandardBtnImgData = Resources::GetImageData("homemenu_button.png");
	WiimoteBtnImgData = Resources::GetImageData("wiimote.png");

	BatteryImgData = Resources::GetImageData("battery.png");
	BatteryBarImgData = Resources::GetImageData("battery_bar.png");

	TopBtnImg = new GuiImage(TopBtnImgData);
	TopBtnOverImg = new GuiImage(TopBtnOverImgData);
	BottomBtnImg = new GuiImage(BottomBtnImgData);
	BottomBtnOverImg = new GuiImage(BottomBtnOverImgData);
	CloseBtnImg = new GuiImage(CloseBtnImgData);
	ExitBtnImg = new GuiImage(StandardBtnImgData);
	ShutdownBtnImg = new GuiImage(StandardBtnImgData);
	WiimoteBtnImg = new GuiImage(WiimoteBtnImgData);

	TitleText = new GuiText(tr("Home Menu"), 40, (GXColor) {255, 255, 255, 255});
	TitleText->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	TitleText->SetPosition(30, 40);
	TitleText->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);

	TopBtn = new GuiButton(TopBtnImg->GetWidth(), TopBtnImg->GetHeight());
	TopBtn->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	TopBtn->SetImage(TopBtnImg);
	TopBtn->SetImageOver(TopBtnOverImg);
	TopBtn->SetLabel(TitleText);
	TopBtn->SetSoundClick(ButtonClickSnd);
	TopBtn->SetSoundOver(ButtonOverSnd);
	TopBtn->SetTrigger(trigA);
	TopBtn->SetTrigger(trigHome);
	TopBtn->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);
	TopBtn->Clicked.connect(this, &HomeMenu::OnButtonClick);

	BottomBtn = new GuiButton(BottomBtnImg->GetWidth(), BottomBtnImg->GetHeight());
	BottomBtn->SetAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
	BottomBtn->SetImage(BottomBtnImg);
	BottomBtn->SetImageOver(BottomBtnOverImg);
	BottomBtn->SetSoundClick(ButtonClickSnd);
	BottomBtn->SetSoundOver(ButtonOverSnd);
	BottomBtn->SetTrigger(trigA);
	BottomBtn->SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_IN, 50);
	BottomBtn->SetSelectable(true);
	BottomBtn->Clicked.connect(this, &HomeMenu::OnButtonClick);

	CloseBtnText = new GuiText(tr("Close"), 28, (GXColor) {0, 0, 0, 255});

	CloseBtn = new GuiButton(CloseBtnImg->GetWidth(), CloseBtnImg->GetHeight());
	CloseBtn->SetImage(CloseBtnImg);
	CloseBtn->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	CloseBtn->SetPosition(-20, 30);
	CloseBtn->SetLabel(CloseBtnText);
	CloseBtn->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);

	ExitBtnText = new GuiText(tr("Menu"), 28, (GXColor) {0, 0, 0, 255});

	ExitBtn = new GuiButton(ExitBtnImg->GetWidth(), ExitBtnImg->GetHeight());
	ExitBtn->SetImage(ExitBtnImg);
	ExitBtn->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	ExitBtn->SetPosition(-140, 0);
	ExitBtn->SetLabel(ExitBtnText);
	ExitBtn->SetSoundClick(ButtonClickSnd);
	ExitBtn->SetSoundOver(ButtonOverSnd);
	ExitBtn->SetTrigger(trigA);
	ExitBtn->SetEffectGrow();
	ExitBtn->SetEffect(EFFECT_FADE, 50);
	ExitBtn->Clicked.connect(this, &HomeMenu::OnButtonClick);

	ShutdownBtnText = new GuiText(tr("Shutdown"), 28, (GXColor) {0, 0, 0, 255});

	ShutdownBtn = new GuiButton(ShutdownBtnImg->GetWidth(), ShutdownBtnImg->GetHeight());
	ShutdownBtn->SetImage(ShutdownBtnImg);
	ShutdownBtn->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	ShutdownBtn->SetPosition(140, 0);
	ShutdownBtn->SetLabel(ShutdownBtnText);
	ShutdownBtn->SetSoundClick(ButtonClickSnd);
	ShutdownBtn->SetSoundOver(ButtonOverSnd);
	ShutdownBtn->SetTrigger(trigA);
	ShutdownBtn->SetEffectGrow();
	ShutdownBtn->SetEffect(EFFECT_FADE, 50);
	ShutdownBtn->Clicked.connect(this, &HomeMenu::OnButtonClick);

	WiimoteBtn = new GuiButton(WiimoteBtnImg->GetWidth(), WiimoteBtnImg->GetHeight());
	WiimoteBtn->SetImage(WiimoteBtnImg);
	WiimoteBtn->SetAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
	WiimoteBtn->SetPosition(45, 232);
	WiimoteBtn->SetTrigger(trigA);
	WiimoteBtn->SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_IN, 50);

	Append(BottomBtn);
	Append(TopBtn);
	Append(CloseBtn);
	Append(ExitBtn);
	Append(ShutdownBtn);
	Append(WiimoteBtn);

	for (int i = 0; i < 4; i++)
	{
		char player[] = "P0";
		player[1] = i+'1';

		PlayerText[i] = new GuiText(player, 28, (GXColor) {255, 255, 255, 255});
		PlayerText[i]->SetAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
		PlayerText[i]->SetPosition(178 + i*108, -76);
		PlayerText[i]->SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_IN, 50);
		Append(PlayerText[i]);

		BatteryBarImg[i] = new GuiImage(BatteryBarImgData);
		BatteryBarImg[i]->SetAlignment(ALIGN_LEFT | ALIGN_BOTTOM);

		BatteryImg[i] = new GuiImage(BatteryImgData);
		BatteryImg[i]->SetAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
		BatteryImg[i]->SetPosition(2, -4);

		BatteryBtn[i] = new GuiButton(0,0);
		BatteryBtn[i]->SetAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
		BatteryBtn[i]->SetPosition(214 + i*108, -80);
		BatteryBtn[i]->SetImage(BatteryBarImg[i]);
		BatteryBtn[i]->SetIcon(BatteryImg[i]);
		BatteryBtn[i]->SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_IN, 50);

		Append(BatteryBtn[i]);
	}

	if(!MusicPlayer::Instance()->IsStopped())
		MusicPlayer::Instance()->Pause();

	// play home sound
	HomeInSnd->Play();
}

HomeMenu::~HomeMenu()
{
	RemoveAll();

	HomeOutSnd->Stop();

	if(!MusicPlayer::Instance()->IsStopped())
		MusicPlayer::Instance()->Resume();

	delete WiimoteBtn;
	delete ShutdownBtn;
	delete ExitBtn;
	delete CloseBtn;
	delete TopBtn;
	delete BottomBtn;

	delete TopBtnImg;
	delete TopBtnOverImg;
	delete BottomBtnImg;
	delete BottomBtnOverImg;
	delete CloseBtnImg;
	delete ExitBtnImg;
	delete ShutdownBtnImg;
	delete WiimoteBtnImg;

	delete TitleText;
	delete ShutdownBtnText;
	delete ExitBtnText;
	delete CloseBtnText;

	for (int i = 0; i < 4; i++)
	{
		delete PlayerText[i];
		delete BatteryBarImg[i];
		delete BatteryImg[i];
		delete BatteryBtn[i];
	}

	delete trigA;
	delete trigHome;

	Resources::Remove(TopBtnImgData);
	Resources::Remove(TopBtnOverImgData);
	Resources::Remove(BottomBtnImgData);
	Resources::Remove(BottomBtnOverImgData);
	Resources::Remove(CloseBtnImgData);
	Resources::Remove(StandardBtnImgData);
	Resources::Remove(WiimoteBtnImgData);
	Resources::Remove(BatteryImgData);
	Resources::Remove(BatteryBarImgData);

	Resources::Remove(ButtonClickSnd);
	Resources::Remove(ButtonOverSnd);
	Resources::Remove(HomeInSnd);
	Resources::Remove(HomeOutSnd);
}

void HomeMenu::hide()
{
	TitleText->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	TopBtn->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	CloseBtn->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	ExitBtn->SetEffect(EFFECT_FADE, -50);
	ShutdownBtn->SetEffect(EFFECT_FADE, -50);
	BottomBtn->SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_OUT, 50);
	WiimoteBtn->SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_OUT, 50);

	for (int i = 0; i < 4; i++)
	{
		PlayerText[i]->SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_OUT, 50);
		BatteryBtn[i]->SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_OUT, 50);
	}
}

void HomeMenu::OnStateChange(GuiElement *sender UNUSED, int state, int stateChan UNUSED)
{
	if(state == STATE_SELECTED)
	{
		WiimoteBtn->SetPosition(WiimoteBtn->GetLeft(), 210);
	}
	else if (state != STATE_SELECTED)
	{
		WiimoteBtn->SetPosition(WiimoteBtn->GetLeft(), 232);
	}
}

void HomeMenu::OnButtonClick(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	if(sender == TopBtn || sender == BottomBtn)
	{
		HomeInSnd->Stop();
		HomeOutSnd->Play();
		this->hide();

		Application::Instance()->PushForDelete(this);
	}
	else if (sender == ExitBtn)
	{
		int ret = WindowPrompt(tr("Exit WiiXplorer?"), 0, tr( "Homebrew Channel" ), tr( "Wii Menu" ), tr("Cancel"));
		if (ret == 1)
		{
			Sys_LoadHBC();
		}
		else if(ret == 2)
		{
			Sys_LoadMenu();
		}
	}
	else if (sender == ShutdownBtn)
	{
		int ret = WindowPrompt(tr("Shutdown the Wii?"), 0, tr("To Standby"), tr("To Idle"), tr("Cancel"));
		if (ret == 1)
		{
			Sys_ShutdownToStandby();
		}
		else if(ret == 2)
		{
			Sys_ShutdownToIdle();
		}
	}
}

void HomeMenu::Draw()
{
	//! check if a WiiMote was connected every 60 frames = ~1s
	struct WUPCData *wupc = NULL;

	if((frameCount % 60) == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			if (WPAD_Probe(i, NULL) == WPAD_ERR_NONE)
			{
				int level = ((WPAD_BatteryLevel(i) * 4) / 100);
				if (level > 4) level = 4;

				BatteryImg[i]->SetTileHorizontal(level);
				BatteryBtn[i]->SetAlpha(255);
				PlayerText[i]->SetAlpha(255);
			}
			else if ((wupc = WUPC_Data(i)) != NULL)
			{
				int level = wupc->battery;
				if (level > 4) level = 4;

				BatteryImg[i]->SetTileHorizontal(level);
				BatteryBtn[i]->SetAlpha(255);
				PlayerText[i]->SetAlpha(255);
			}
			else
			{
				BatteryImg[i]->SetTileHorizontal(0);
				BatteryBtn[i]->SetAlpha(130);
				PlayerText[i]->SetAlpha(100);
			}
		}
	}

	GuiFrame::Draw();
}
