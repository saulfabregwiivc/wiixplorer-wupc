/****************************************************************************
 * Copyright (C) 2009-2012 Dimok
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
#include <gctypes.h>
#include "PathBrowser.h"
#include "Controls/Application.h"

PathBrowser::PathBrowser(const std::string &path)
	: GuiFrame(screenwidth, screenheight)
{
	btnSoundClick = Resources::GetSound("button_click.wav");
	btnSoundOver = Resources::GetSound("button_over.wav");

	btnOutline = Resources::GetImageData("button.png");
	btnOutlineOver = Resources::GetImageData("button_over.png");

	trigA.SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigB.SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);

	doneBtnTxt = new GuiText(tr("Done"), 20, (GXColor){0, 0, 0, 255});
	doneBtnImg = new GuiImage(btnOutline);
	doneBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	doneBtn->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	doneBtn->SetPosition(100+btnOutline->GetWidth()/2, -65);
	doneBtn->SetLabel(doneBtnTxt);
	doneBtn->SetImage(doneBtnImg);
	doneBtn->SetSoundClick(btnSoundOver);
	doneBtn->SetSoundOver(btnSoundOver);
	doneBtn->SetTrigger(&trigA);
	doneBtn->SetEffectGrow();
	doneBtn->Clicked.connect(this, &PathBrowser::OnButtonClick);

	cancelBtnTxt = new GuiText(tr("Cancel"), 20, (GXColor){0, 0, 0, 255});
	cancelBtnImg = new GuiImage(btnOutline);
	cancelBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	cancelBtn->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	cancelBtn->SetPosition(-105-btnOutline->GetWidth()/2, -65);
	cancelBtn->SetLabel(cancelBtnTxt);
	cancelBtn->SetImage(cancelBtnImg);
	cancelBtn->SetSoundOver(btnSoundOver);
	cancelBtn->SetTrigger(&trigA);
	cancelBtn->SetTrigger(&trigB);
	cancelBtn->SetEffectGrow();
	cancelBtn->Clicked.connect(this, &PathBrowser::OnButtonClick);

	explorer = new Explorer();
	explorer->SetFilter(FILTER_FILES);
	//! Reload path with filter turned on
	explorer->LoadPath(path.c_str());

	Append(explorer);
	Append(doneBtn);
	Append(cancelBtn);
}

PathBrowser::~PathBrowser()
{
	RemoveAll();

	Resources::Remove(btnOutline);
	Resources::Remove(btnOutlineOver);

	Resources::Remove(btnSoundClick);
	Resources::Remove(btnSoundOver);

	delete explorer;

	delete doneBtnTxt;
	delete doneBtnImg;
	delete doneBtn;

	delete cancelBtnTxt;
	delete cancelBtnImg;
	delete cancelBtn;
}

void PathBrowser::OnButtonClick(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	ButtonClick(this, sender == doneBtn, explorer->GetCurrectPath());
}
