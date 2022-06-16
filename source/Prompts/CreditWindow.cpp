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
 * CreditWindow.cpp
 *
 * for WiiXplorer 2010
 ***************************************************************************/
#include "Controls/Application.h"
#include "CreditWindow.h"
#include "svnrev.h"


CreditWindow::CreditWindow(GuiFrame *p)
	: GuiFrame(0, 0, p)
{
	dialogBox = Resources::GetImageData("bg_properties.png");
	dialogBoxImg = new GuiImage(dialogBox);

	width = dialogBox->GetWidth();
	height = dialogBox->GetHeight();

	SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);

	trigA = new GuiTrigger();
	trigA->SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigB = new GuiTrigger();
	trigB->SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);

	btnClick = Resources::GetSound("button_click.wav");

	int i = 0;
	int x = 30;
	int y = 30;

	Entrie[i] = new GuiText(tr("Credits"), 28, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	Entrie[i]->SetPosition(0, y);
	i++;
	y += 50;

	Entrie[i] = new GuiText(tr("Coders:"), 24, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x, y);
	i++;

	Entrie[i] = new GuiText("Dimok", 22, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x+150, y);
	i++;
	y += 32;

	Entrie[i] = new GuiText("r-win", 22, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x+150, y);
	i++;
	y += 32;

	Entrie[i] = new GuiText("dude", 22, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x+150, y);
	i++;
	y += 40;

	Entrie[i] = new GuiText(tr("Designer:"), 24, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x, y);
	i++;

	Entrie[i] = new GuiText("NeoRame", 22, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x+150, y);
	i++;
	y += 50;

	Entrie[i] = new GuiText(tr("Special thanks to:"), 22, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x,y);
	i++;
	y += 28;

	char text[80];
	snprintf(text, sizeof(text), "Dj_Skual %s", tr("and all the translators."));
	Entrie[i] = new GuiText(text, 20, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x,y);
	i++;
	y += 28;

	snprintf(text, sizeof(text), "Tantric %s ", tr("for his great tool libwiigui."));
	Entrie[i] = new GuiText(text, 20, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x,y);
	i++;
	y += 28;

	snprintf(text, sizeof(text), "Joseph Jordan %s ", tr("for his FTP Server source."));
	Entrie[i] = new GuiText(text, 20, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x,y);
	i++;
	y += 28;

	Entrie[i] = new GuiText(tr("The whole devkitPro & libogc staff."), 20, (GXColor) {0, 0, 0, 255});
	Entrie[i]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Entrie[i]->SetPosition(x,y);
	i++;

	CreditEntries = i;

	arrowUp = Resources::GetImageData("scrollbar_arrowup.png");
	arrowUpOver = Resources::GetImageData("scrollbar_arrowup_over.png");
	arrowUpImg = new GuiImage(arrowUp);
	arrowUpImg->SetAngle(45);
	arrowUpImgOver = new GuiImage(arrowUpOver);
	arrowUpImgOver->SetAngle(45);
	Backbtn = new GuiButton(arrowUpImg->GetWidth(), arrowUpImg->GetHeight());
	Backbtn->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	Backbtn->SetSoundClick(btnClick);
	Backbtn->SetImage(arrowUpImg);
	Backbtn->SetImageOver(arrowUpImgOver);
	Backbtn->SetPosition(-20, 20);
	Backbtn->SetEffectGrow();
	Backbtn->SetTrigger(trigA);
	Backbtn->SetTrigger(trigB);
	Backbtn->Clicked.connect(this, &CreditWindow::OnButtonClick);

	char Rev[50];
	sprintf(Rev, "Rev. %i", atoi(SvnRev()));

	RevNum = new GuiText(Rev, 22, (GXColor) {0, 0, 0, 255});
	RevNum->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	RevNum->SetPosition(20, 20);

	IOSRevTxt = new GuiText((char *) NULL, 16, (GXColor) {0, 0, 0, 255});
	IOSRevTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	IOSRevTxt->SetPosition(20, 50);
	IOSRevTxt->SetTextf("IOS %i Rev. %i", IOS_GetVersion(), IOS_GetRevision());

	Append(dialogBoxImg);
	Append(RevNum);
	Append(IOSRevTxt);
	for(int i = 0; i < CreditEntries; i++)
		Append(Entrie[i]);
	Append(Backbtn);

	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 40);
}

CreditWindow::~CreditWindow()
{
	RemoveAll();

	Resources::Remove(btnClick);
	Resources::Remove(dialogBox);
	Resources::Remove(arrowUp);
	Resources::Remove(arrowUpOver);
	delete dialogBoxImg;
	delete arrowUpImg;
	delete arrowUpImgOver;

	delete Backbtn;

	delete RevNum;
	delete IOSRevTxt;

	delete trigA;
	delete trigB;

	for(int i = 0; i < CreditEntries; i++)
	{
		delete Entrie[i];
		Entrie[i] = NULL;
	}
}

void CreditWindow::OnButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 40);
	Application::Instance()->PushForDelete(this);
}
