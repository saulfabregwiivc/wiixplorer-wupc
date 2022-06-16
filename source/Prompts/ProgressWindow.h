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
#ifndef _PROGRESSWINDOW_H_
#define _PROGRESSWINDOW_H_

#include "GUI/gui.h"
#include "PromptWindow.h"
#include "ProgressBar.h"

#define PROGRESS_CANCELED	-10

class ProgressWindow : public GuiFrame, public sigslot::has_slots<>
{
public:
	static ProgressWindow * Instance() { if(!instance) instance = new ProgressWindow; return instance; }
	static void DestroyInstance() { delete instance; instance = NULL; }

	void OpenWindow(void) { OpenRequest = !Minimized; CloseRequest = false; }
	void CloseWindow(void) { OpenRequest = false; CloseRequest = true; }

	void StartProgress(const char *title, const char *msg = NULL);
	void ShowProgress(const u64 &done, const u64 &total, const char *filename);
	void ShowProgress(const u64 &done, const u64 &total);
	void FinishProgress(const u64 &filesize) { completeDone += filesize; ShowProgress(filesize, filesize); }
	void StopProgress();
	void SetTitle(const char *title);
	void SetMessage(const char *msg);

	const char * GetTitle() const { return ProgressTitle; };
	void SetProgressType(int mode);
	bool IsCanceled() const { return Canceled; }
	bool IsMinimized() const { return Minimized; }
	bool IsRunning() const { return !WindowClosed || Minimized; }
	void Draw();
	void SetCompleteValues(const s64 &done, const s64 &total) { completeDone = done; completeTotal = total; }
	void SetUnit(const char *u) { ProgressUnit = u; }
private:
	ProgressWindow();
	virtual ~ProgressWindow();
	void OnCancelClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED);
	void SetupProgressbar();
	void TryOpenWindow(void);
	void TryCloseWindow(void);
	static ProgressWindow * instance;

	u64 progressDone;
	u64 progressTotal;
	s64 completeDone;
	s64 completeTotal;
	bool Changed;
	bool Canceled;
	bool WindowClosed;
	bool OpenRequest;
	bool CloseRequest;
	bool Minimized;

	Timer delayTimer;
	Timer ProgressTimer;

	char ProgressTitle[50];
	char ProgressMsg[80];
	const char *ProgressUnit;

	ProgressBar *progressBar;
	ProgressBar *totalProgressBar;

	GuiImageData *dialogBox;
	GuiImageData *btnOutline;

	GuiImage * dialogBoxImg;
	GuiImage * buttonImg;
	GuiImage * MinimizeImg;

	GuiText * titleTxt;
	GuiText * msgTxt;
	GuiText * speedTxt;
	GuiText * sizeTxt;
	GuiText * sizeTotalTxt;
	GuiText * TimeTxt;;
	GuiText * AbortBtnTxt;
	GuiText * MinimizeTxt;

	GuiButton * AbortBtn;
	GuiButton * MinimizeBtn;

	GuiSound * soundClick;
	GuiSound * soundOver;

	GuiTrigger * trigA;
};

#define StartProgress	ProgressWindow::Instance()->StartProgress
#define ShowProgress	ProgressWindow::Instance()->ShowProgress
#define FinishProgress	ProgressWindow::Instance()->FinishProgress
#define StopProgress	ProgressWindow::Instance()->StopProgress

#endif
