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
#include <sys/statvfs.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include "Properties.h"
#include "sys.h"
#include "Controls/Application.h"
#include "FileOperations/fileops.h"

bool sizegainrunning = false;

Properties::Properties(ItemMarker * IMarker)
	: GuiFrame(0,0)
{
	int Position_X = 40;
	int Position_Y = 40;

	folder = false;
	bClosing = false;
	sizegaindone = false;
	FileCount = 0;
	OldSize = 0;
	TotalSize = 0;
	devicefree = 0;
	devicesize = 0;
	Marker = *IMarker;
	foldersizethread = LWP_THREAD_NULL;

	for(int i = 0; i < Marker.GetItemcount(); i++)
	{
		if(Marker.IsItemDir(i))
		{
			folder = true;
		}
		else
		{
			TotalSize += Marker.GetItemSize(i);
			++FileCount;
		}
	}

	OldSize = TotalSize;

	if(folder) {
		//!Initialize GetSizeThread for Properties
		LWP_CreateThread(&foldersizethread, FolderSizeThread, this, NULL, 32768, 60);
	}

	dialogBox = Resources::GetImageData("bg_properties.png");
	titleData = Resources::GetImageData("icon_folder.png");
	arrowUp = Resources::GetImageData("close.png");
	arrowUpOver = Resources::GetImageData("close_over.png");

	btnClick = Resources::GetSound("button_click.wav");

	dialogBoxImg = new GuiImage(dialogBox);

	width = dialogBox->GetWidth();
	height = dialogBox->GetHeight();

	trigA = new SimpleGuiTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigB = new GuiTrigger();
	trigB->SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);

	const char * filename = Marker.GetItemName(Marker.GetItemcount()-1);

	TitleTxt = new GuiText(filename, 22, (GXColor){0, 0, 0, 255});
	TitleTxt->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	TitleTxt->SetPosition(0, Position_Y);
	TitleTxt->SetMaxWidth(dialogBox->GetWidth()-Position_X, DOTTED);

	TitleImg = new GuiImage(titleData);
	TitleImg->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	int IconPosition = -(TitleTxt->GetTextWidth()/2+titleData->GetWidth())+10;
	if(IconPosition < (30-width/2))
		IconPosition = 30-width/2;
	TitleImg->SetPosition(IconPosition, Position_Y);
	Position_Y += 50;

	filepathTxt =  new GuiText(tr("Filepath:"), 20, (GXColor){0, 0, 0, 255});
	filepathTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filepathTxt->SetPosition(Position_X, Position_Y);

	filepathvalTxt =  new GuiText(Marker.GetItemPath(Marker.GetItemcount()-1), 20, (GXColor){0, 0, 0, 255});
	filepathvalTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filepathvalTxt->SetPosition(Position_X+80, Position_Y);
	filepathvalTxt->SetMaxWidth(dialogBox->GetWidth()-Position_X-130, SCROLL_HORIZONTAL);
	Position_Y += 30;

	filecountTxt = new GuiText(tr("Files:"), 20, (GXColor){0, 0, 0, 255});
	filecountTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filecountTxt->SetPosition(Position_X, Position_Y);

	filecountTxtVal = new GuiText(fmt("%i", FileCount), 20, (GXColor){0, 0, 0, 255});
	filecountTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filecountTxtVal->SetPosition(Position_X+180, Position_Y);
	Position_Y += 30;

	char temp[MAXPATHLEN];

	if(OldSize > GBSIZE)
		sprintf(temp, "%0.2fGB", OldSize/GBSIZE);
	else if(OldSize > MBSIZE)
		sprintf(temp, "%0.2fMB", OldSize/MBSIZE);
	else if(OldSize > KBSIZE)
		sprintf(temp, "%0.2fKB", OldSize/KBSIZE);
	else
		sprintf(temp, "%iB", (u32) OldSize);

	filesizeTxt = new GuiText(tr("Size:"), 20, (GXColor){0, 0, 0, 255});
	filesizeTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filesizeTxt->SetPosition(Position_X, Position_Y);

	filesizeTxtVal = new GuiText(temp, 20, (GXColor){0, 0, 0, 255});
	filesizeTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filesizeTxtVal->SetPosition(Position_X+180, Position_Y);
	Position_Y += 30;

	char * pch = NULL;
	if(Marker.GetItemcount() > 1)
	{
		snprintf(temp, sizeof(temp), tr("Multiple Items"));
	}
	else if(folder)
	{
		snprintf(temp, sizeof(temp), tr("Folder(s)"));
		TitleTxt->SetMaxWidth(dialogBox->GetWidth()-75, DOTTED);
	}
	else
	{
		if(filename)
			pch = strrchr(filename, '.');
		if(pch)
			pch += 1;
		else
			pch = (char *) filename;

		snprintf(temp, sizeof(temp), "%s", pch != NULL ? pch : "");
	}

	filetypeTxt = new GuiText(tr("Filetype:"), 20, (GXColor){0, 0, 0, 255});
	filetypeTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filetypeTxt->SetPosition(Position_X, Position_Y);

	filetypeTxtVal = new GuiText(temp, 20, (GXColor){0, 0, 0, 255});
	filetypeTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filetypeTxtVal->SetPosition(Position_X+180, Position_Y);
	Position_Y += 30;

	devicefreeTxt = NULL;
	devicefreeTxtVal = NULL;
	devicetotalTxt = NULL;
	devicetotalTxtVal = NULL;

	if(folder)
	{
		devicefreeTxt = new GuiText(tr("Free Space:"), 20, (GXColor){0, 0, 0, 255});
		devicefreeTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
		devicefreeTxt->SetPosition(Position_X, Position_Y);

		devicefreeTxtVal = new GuiText("0B", 20, (GXColor){0, 0, 0, 255});
		devicefreeTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
		devicefreeTxtVal->SetPosition(Position_X+180, Position_Y);
		Position_Y += 30;

		devicetotalTxt = new GuiText(tr("Total Space:"), 20, (GXColor){0, 0, 0, 255});
		devicetotalTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
		devicetotalTxt->SetPosition(Position_X, Position_Y);

		devicetotalTxtVal = new GuiText("0B", 20, (GXColor){0, 0, 0, 255});
		devicetotalTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
		devicetotalTxtVal->SetPosition(Position_X+180, Position_Y);
		Position_Y += 30;
	}

	struct stat filestat;
	memset(&filestat, 0, sizeof(struct stat));
	if(Marker.GetItemcount() > 0)
		stat(Marker.GetItemPath(Marker.GetItemcount()-1), &filestat);

	last_accessTxt = new GuiText(tr("Last access:"), 20, (GXColor){0, 0, 0, 255});
	last_accessTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	last_accessTxt->SetPosition(Position_X, Position_Y);

	strftime(temp, sizeof(temp), "%H:%M  %d %b %Y", localtime(&filestat.st_atime));
	last_accessTxtVal = new GuiText(temp, 20, (GXColor){0, 0, 0, 255});
	last_accessTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	last_accessTxtVal->SetPosition(Position_X+180, Position_Y);
	Position_Y += 30;

	last_modifTxt = new GuiText(tr("Last modified:"), 20, (GXColor){0, 0, 0, 255});
	last_modifTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	last_modifTxt->SetPosition(Position_X, Position_Y);

	strftime(temp, sizeof(temp), "%H:%M  %d %b %Y", localtime(&filestat.st_mtime));
	last_modifTxtVal = new GuiText(temp, 20, (GXColor){0, 0, 0, 255});
	last_modifTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	last_modifTxtVal->SetPosition(Position_X+180, Position_Y);
	Position_Y += 30;

	last_changeTxt = new GuiText(tr("Last change:"), 20, (GXColor){0, 0, 0, 255});
	last_changeTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	last_changeTxt->SetPosition(Position_X, Position_Y);

	strftime(temp, sizeof(temp), "%H:%M  %d %b %Y", localtime(&filestat.st_ctime));
	last_changeTxtVal = new GuiText(temp, 20, (GXColor){0, 0, 0, 255});
	last_changeTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	last_changeTxtVal->SetPosition(Position_X+180, Position_Y);

	arrowUpImg = new GuiImage(arrowUp);
	arrowUpImgOver = new GuiImage(arrowUpOver);
	CloseBtn = new GuiButton(arrowUpImg->GetWidth(), arrowUpImg->GetHeight());
	CloseBtn->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	CloseBtn->SetSoundClick(btnClick);
	CloseBtn->SetImage(arrowUpImg);
	CloseBtn->SetImageOver(arrowUpImgOver);
	CloseBtn->SetPosition(-20, 20);
	CloseBtn->SetEffectGrow();
	CloseBtn->SetTrigger(trigA);
	CloseBtn->SetTrigger(trigB);
	CloseBtn->Clicked.connect(this, &Properties::OnButtonClick);

	Append(dialogBoxImg);
	Append(TitleTxt);
	if(Marker.IsItemDir(Marker.GetItemcount()-1))
		Append(TitleImg);
	Append(filepathTxt);
	Append(filepathvalTxt);
	Append(filecountTxt);
	Append(filecountTxtVal);
	Append(filesizeTxt);
	Append(filesizeTxtVal);
	Append(filetypeTxt);
	Append(filetypeTxtVal);
	if(folder)
	{
		Append(devicefreeTxt);
		Append(devicefreeTxtVal);
		Append(devicetotalTxt);
		Append(devicetotalTxtVal);
	}
	Append(last_accessTxt);
	Append(last_accessTxtVal);
	Append(last_modifTxt);
	Append(last_modifTxtVal);
	Append(last_changeTxt);
	Append(last_changeTxtVal);
	Append(CloseBtn);

	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 40);
}

Properties::~Properties()
{
	RemoveAll();

	if(foldersizethread != LWP_THREAD_NULL)
		LWP_JoinThread(foldersizethread, NULL);

	Resources::Remove(dialogBox);
	Resources::Remove(titleData);
	Resources::Remove(arrowUp);
	Resources::Remove(arrowUpOver);
	Resources::Remove(btnClick);

	delete dialogBoxImg;
	delete TitleImg;
	delete arrowUpImg;
	delete arrowUpImgOver;
	delete TitleTxt;
	delete filepathTxt;
	delete filepathvalTxt;
	delete filecountTxt;
	delete filecountTxtVal;
	delete filesizeTxt;
	delete filesizeTxtVal;
	delete filetypeTxt;
	delete filetypeTxtVal;
	if(devicefreeTxt)
		delete devicefreeTxt;
	if(devicefreeTxtVal)
		delete devicefreeTxtVal;
	if(devicetotalTxt)
		delete devicetotalTxt;
	if(devicetotalTxtVal)
		delete devicetotalTxtVal;
	delete last_accessTxt;
	delete last_accessTxtVal;
	delete last_modifTxt;
	delete last_modifTxtVal;
	delete last_changeTxt;
	delete last_changeTxtVal;

	delete CloseBtn;
	delete trigA;
	delete trigB;
}

void Properties::OnButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	bClosing = true;
	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 40);
	Application::Instance()->PushForDelete(this);
}

void Properties::UpdateSizeValue()
{
	OldSize = TotalSize;
	char sizetext[20];
	char filecounttext[20];

	if(OldSize > GBSIZE)
		snprintf(sizetext, sizeof(sizetext), "%0.2fGB", OldSize/GBSIZE);
	else if(OldSize > MBSIZE)
		snprintf(sizetext, sizeof(sizetext), "%0.2fMB", OldSize/MBSIZE);
	else if(OldSize > KBSIZE)
		snprintf(sizetext, sizeof(sizetext), "%0.2fKB", OldSize/KBSIZE);
	else
		snprintf(sizetext, sizeof(sizetext), "%iB", (u32) OldSize);

	filesizeTxtVal->SetText(sizetext);
	snprintf(filecounttext, sizeof(filecounttext), "%i", FileCount);
	filecountTxtVal->SetText(filecounttext);

	if(devicefree > 0 && devicesize > 0)
	{
		if(devicefree > GBSIZE)
			snprintf(sizetext, sizeof(sizetext), "%0.2fGB", devicefree/GBSIZE);
		else if(devicefree > MBSIZE)
			snprintf(sizetext, sizeof(sizetext), "%0.2fMB", devicefree/MBSIZE);
		else if(devicefree > KBSIZE)
			snprintf(sizetext, sizeof(sizetext), "%0.2fKB", devicefree/KBSIZE);
		else
			snprintf(sizetext, sizeof(sizetext), "%iB", (u32) devicefree);

		devicefreeTxtVal->SetText(sizetext);

		if(devicesize > GBSIZE)
			snprintf(sizetext, sizeof(sizetext), "%0.2fGB", devicesize/GBSIZE);
		else if(devicesize > MBSIZE)
			snprintf(sizetext, sizeof(sizetext), "%0.2fMB", devicesize/MBSIZE);
		else if(devicesize > KBSIZE)
			snprintf(sizetext, sizeof(sizetext), "%0.2fKB", devicesize/KBSIZE);
		else
			snprintf(sizetext, sizeof(sizetext), "%iB", (u32) devicesize);

		devicetotalTxtVal->SetText(sizetext);
	}
}

void Properties::Draw()
{
	if(OldSize != TotalSize || sizegaindone)
	{
		sizegaindone = false;
		UpdateSizeValue();
	}
	GuiFrame::Draw();
}

/****************************************************************************
 * GetCurrentFolderSize in the background
 ***************************************************************************/
void Properties::InternalFolderSizeGain()
{
	bool gotDeviceSize = false;
	char *folderpath = (char *) malloc(MAXPATHLEN);
	if(!folderpath)
		return;

	for(int i = 0; i < Marker.GetItemcount(); i++)
	{
		if(!Marker.IsItemDir(i))
			continue;

		snprintf(folderpath, MAXPATHLEN, "%s/", Marker.GetItemPath(i));

		if(!gotDeviceSize)
		{
			//background capacity getting because it is very slow
			struct statvfs stats;
			memset(&stats, 0, sizeof(stats));
			memcpy(&stats.f_flag, "SCAN", 4);
			statvfs(folderpath, &stats);

			devicefree = (u64)stats.f_frsize * (u64)stats.f_bfree;
			devicesize = (u64)stats.f_frsize * (u64)stats.f_blocks;

			gotDeviceSize = true;
		}

		GetFolderSize(folderpath, TotalSize, FileCount, bClosing);
	}

	free(folderpath);

	sizegaindone = true;
}

void * Properties::FolderSizeThread(void *arg)
{
	((Properties *) arg)->InternalFolderSizeGain();
	return NULL;
}
