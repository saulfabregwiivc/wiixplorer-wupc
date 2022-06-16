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
#include "Prompts/ProgressWindow.h"
#include "Controls/Application.h"
#include "Memory/Resources.h"
#include "Settings.h"

#undef StartProgress
#undef ShowProgress
#undef FinishProgress
#undef StopProgress

ProgressWindow * ProgressWindow::instance = NULL;

ProgressWindow::ProgressWindow()
	: GuiFrame(0, 0)
{
	completeDone = 0;
	completeTotal = -1;

	memset(ProgressTitle, 0, sizeof(ProgressTitle));
	memset(ProgressMsg, 0, sizeof(ProgressMsg));
	progressDone = 0;
	progressTotal = 0;
	Changed = false;
	Minimized = false;
	OpenRequest = false;
	CloseRequest = true;
	WindowClosed = true;
	Canceled = false;
	ProgressUnit = NULL;

	progressBar = NULL;
	totalProgressBar = NULL;

	dialogBox = NULL;
	btnOutline = NULL;

	dialogBoxImg = NULL;
	buttonImg = NULL;
	MinimizeImg = NULL;

	titleTxt = NULL;
	msgTxt = NULL;
	speedTxt = NULL;
	sizeTxt = NULL;
	sizeTotalTxt = NULL;
	TimeTxt = NULL;
	AbortBtnTxt = NULL;
	MinimizeTxt = NULL;

	AbortBtn = NULL;
	MinimizeBtn = NULL;

	soundClick = NULL;
	soundOver = NULL;

	trigA = NULL;
}

ProgressWindow::~ProgressWindow()
{
	Canceled = true;
	TryCloseWindow();
}

void ProgressWindow::TryOpenWindow()
{
	//! To skip progressbar for fast processes
	if(WindowClosed && delayTimer.elapsedMilliSecs() < 500)
		return;

	WindowClosed = false;

	dialogBox = Resources::GetImageData("dialogue_box.png");

	dialogBoxImg = new GuiImage(dialogBox);
	Append(dialogBoxImg);

	width = dialogBox->GetWidth();
	height = dialogBox->GetHeight();

	const int yPos = 140;

	progressBar = new ProgressBar;
	progressBar->SetPosition(58, yPos);
	Append(progressBar);

	sizeTxt = new GuiText((char*) NULL, 18, (GXColor){0, 0, 0, 255});
	sizeTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	sizeTxt->SetPosition(55, yPos+50);
	Append(sizeTxt);

	speedTxt = new GuiText((char*) NULL, 18, (GXColor){0, 0, 0, 255});
	speedTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	speedTxt->SetPosition(55, yPos+70);
	Append(speedTxt);

	TimeTxt = new GuiText((char*) NULL, 18, (GXColor){0, 0, 0, 255});
	TimeTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	TimeTxt->SetPosition(290, yPos+50);
	Append(TimeTxt);

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	soundClick = Resources::GetSound("button_click.wav");
	soundOver = Resources::GetSound("button_over.wav");

	titleTxt = new GuiText(ProgressTitle, 22, (GXColor){0, 0, 0, 255});
	titleTxt->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	titleTxt->SetPosition(0,50);
	titleTxt->SetMaxWidth(430, DOTTED);
	Append(titleTxt);

	msgTxt = new GuiText((char*) NULL, 22, (GXColor){0, 0, 0, 255});
	msgTxt->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	msgTxt->SetPosition(0,100);
	msgTxt->SetMaxWidth(430, DOTTED);
	Append(msgTxt);

	btnOutline = Resources::GetImageData("small_button.png");
	buttonImg = new GuiImage(btnOutline);
	AbortBtnTxt = new GuiText(tr("Cancel"), 22, (GXColor){0, 0, 0, 255});
	AbortBtn = new GuiButton(buttonImg->GetWidth(), buttonImg->GetHeight());
	AbortBtn->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	AbortBtn->SetPosition(0, -48);
	AbortBtn->SetLabel(AbortBtnTxt);
	AbortBtn->SetImage(buttonImg);
	AbortBtn->SetTrigger(trigA);
	AbortBtn->SetSoundClick(soundClick);
	AbortBtn->SetSoundOver(soundOver);
	AbortBtn->SetEffectGrow();
	AbortBtn->Clicked.connect(this, &ProgressWindow::OnCancelClick);
	Append(AbortBtn);
//
//	if(Minimizable)
//	{
//		MinimizeImg = new GuiImage(btnOutline);
//		MinimizeTxt = new GuiText(tr("Minimize"), 22, (GXColor){0, 0, 0, 255});
//		MinimizeBtn = new GuiButton(MinimizeImg->GetWidth(), MinimizeImg->GetHeight());
//		MinimizeBtn->SetLabel(MinimizeTxt);
//		MinimizeBtn->SetImage(MinimizeImg);
//		MinimizeBtn->SetTrigger(trigA);
//		MinimizeBtn->SetSoundClick(soundClick);
//		MinimizeBtn->SetSoundOver(soundOver);
//		MinimizeBtn->SetEffectGrow();
//		Append(MinimizeBtn);
//
//		AbortBtn->SetAlignment(ALIGN_RIGHT | ALIGN_BOTTOM);
//		AbortBtn->SetPosition(-60, -48);
//		MinimizeBtn->SetAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
//		MinimizeBtn->SetPosition(60, -48);
//	}

	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);
	SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	DimBackground(true);

	Application::Instance()->Append(this);
	Application::Instance()->SetUpdateOnly(this);
}

void ProgressWindow::TryCloseWindow()
{
	if(WindowClosed)
		return;

	static bool effectRunning = false;

	if(!effectRunning)
	{
		if(Minimized)
			SetEffect(EFFECT_FADE, 30);
		else
			SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);

		effectRunning = true;
	}

	if(GetEffect() > 0 && !Application::Instance()->isClosing())
		return;

	RemoveAll();
	Application::Instance()->UnsetUpdateOnly(this);
	DimBackground(false);

	if(soundClick)
		Resources::Remove(soundClick);
	if(soundOver)
		Resources::Remove(soundOver);

	if(dialogBox)
		Resources::Remove(dialogBox);
	if(btnOutline)
		Resources::Remove(btnOutline);

	delete progressBar;
	delete totalProgressBar;

	delete dialogBoxImg;
	delete buttonImg;
	delete MinimizeImg;

	delete titleTxt;
	delete msgTxt;
	delete speedTxt;
	delete sizeTxt;
	delete sizeTotalTxt;
	delete TimeTxt;
	delete AbortBtnTxt;
	delete MinimizeTxt;

	delete AbortBtn;
	delete MinimizeBtn;

	delete trigA;

	progressBar = NULL;
	totalProgressBar = NULL;

	dialogBox = NULL;
	btnOutline = NULL;

	dialogBoxImg = NULL;
	buttonImg = NULL;
	MinimizeImg = NULL;

	titleTxt = NULL;
	msgTxt = NULL;
	speedTxt = NULL;
	sizeTxt = NULL;
	sizeTotalTxt = NULL;
	TimeTxt = NULL;
	AbortBtnTxt = NULL;
	MinimizeTxt = NULL;

	AbortBtn = NULL;
	MinimizeBtn = NULL;

	soundClick = NULL;
	soundOver = NULL;

	trigA = NULL;

	effectRunning = false;
	WindowClosed = true;
}


void ProgressWindow::Draw()
{
	static u32 drawCounter = 0;

	if(OpenRequest && WindowClosed)
		TryOpenWindow();

	if(CloseRequest && !WindowClosed)
		TryCloseWindow();

	if(!WindowClosed && Changed && drawCounter > 2)
	{
		msgTxt->SetText(ProgressMsg);

		float fDone;
		float fTotal;

		if(completeTotal >= 0)
		{
			static s64 oldCompleteDone = 0;

			if(progressDone == progressTotal && (oldCompleteDone <= completeDone))
			{
				fDone = (float) completeDone;
			}
			else
			{
				oldCompleteDone = completeDone + progressDone;
				fDone = (float) oldCompleteDone;
			}

			fTotal = (float) completeTotal;
		}
		else
		{
			fDone = (float) progressDone;
			fTotal = (float) progressTotal;
		}

		if(fDone > fTotal)
			fDone = fTotal;

		float Percent;
		if(fTotal > 0.0f)
			Percent = 100.0f*fDone/fTotal;
		else
			Percent = 0.0f;

		Percent = LIMIT(Percent, 0.0f, 100.0f);

		progressBar->SetPercent(Percent);

		float speed = 0.0f;

		if(ProgressTimer.elapsed() > 0.0f)
			speed = (fDone/ProgressTimer.elapsed());

		int TimeLeft = 0;
		if(speed > 0.0f)
		{
			TimeLeft = (int) ((fTotal-fDone)/speed);
			if(TimeLeft < 0)
				TimeLeft = 0;
		}

		TimeTxt->SetTextf("%s %02i:%02i:%02i", tr("Time left:"), TimeLeft / 3600, (TimeLeft / 60) % 60, TimeLeft % 60);

		if(ProgressUnit)
		{
			speedTxt->SetTextf("%i %s/s", (int) speed, ProgressUnit);
			sizeTxt->SetTextf("%i/%i %s", (int) fDone, (int) fTotal, ProgressUnit);
		}
		else
		{
			speedTxt->SetTextf("%0.1fKB/s", speed/KBSIZE);

			//! Single progress bar values
			if(fTotal > GBSIZE)
				sizeTxt->SetTextf("%0.2fGB/%0.2fGB", fDone/GBSIZE, fTotal/GBSIZE);
			else if(fTotal > MBSIZE)
				sizeTxt->SetTextf("%0.2fMB/%0.2fMB", fDone/MBSIZE, fTotal/MBSIZE);
			else if(fTotal > KBSIZE)
				sizeTxt->SetTextf("%0.2fKB/%0.2fKB", fDone/KBSIZE, fTotal/KBSIZE);
			else
				sizeTxt->SetTextf("%0.0fB/%0.0fB", fDone, fTotal);
		}

		drawCounter = 0;
		Changed = false;
	}

	drawCounter++;
	GuiFrame::Draw();
}

void ProgressWindow::StartProgress(const char *title, const char *msg)
{
	if(title)
		strncpy(ProgressTitle, title, sizeof(ProgressTitle)-1);

	if(msg)
		strncpy(ProgressMsg, msg, sizeof(ProgressMsg)-1);
	else
		ProgressMsg[0] = 0;

	delayTimer.reset();

	progressDone = 0;
	progressTotal = 0;
	ProgressTimer.reset();

	Changed = true;
	Canceled = false;
	OpenRequest = true;
	CloseRequest = false;
	Minimized = false;
}

void ProgressWindow::StopProgress()
{
	OpenRequest = false;
	CloseRequest = true;
	ProgressTitle[0] = 0;
	ProgressMsg[0] = 0;
	completeDone = 0;
	completeTotal = -1;
}

void ProgressWindow::ShowProgress(const u64 &done, const u64 &total)
{
	progressDone = (done > total) ? total : done;
	progressTotal = total;

	Changed = true;
}

void ProgressWindow::ShowProgress(const u64 &done, const u64 &total, const char *msg)
{
	if(msg)
		strncpy(ProgressMsg, msg, sizeof(ProgressMsg)-1);
	else
		ProgressMsg[0] = 0;

	ShowProgress(done, total);
}

void ProgressWindow::SetTitle(const char *title)
{
	if(title)
		strncpy(ProgressTitle, title, sizeof(ProgressTitle)-1);
	else
		ProgressTitle[0] = 0;
}

void ProgressWindow::SetMessage(const char *msg)
{
	if(msg)
		strncpy(ProgressMsg, msg, sizeof(ProgressMsg)-1);
	else
		ProgressMsg[0] = 0;
}

void ProgressWindow::OnCancelClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	OpenRequest = false;
	CloseRequest = true;
	Canceled = true;
}
