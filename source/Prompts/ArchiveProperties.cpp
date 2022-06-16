/***************************************************************************
 * Copyright (C) 2009
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
 * ArchiveProperties.cpp
 *
 * for WiiXplorer 2009
 ***************************************************************************/

#include <sys/statvfs.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

#include "ArchiveProperties.h"
#include "Prompts/PromptWindows.h"
#include "Controls/Application.h"
#include "sys.h"

ArchiveProperties::ArchiveProperties(ArchiveHandle *archive, ItemMarker *Marker)
	:GuiFrame(0,0)
{
	int Position_X = 40;
	int Position_Y = 40;
	char temp[MAXPATHLEN];

	int lastItem = Marker->GetItemcount()-1;
	const char *filenamePtr = 0;
	bool isDir = false;
	if(lastItem >= 0)
	{
		isDir = Marker->IsItemDir(lastItem);

		filenamePtr = strrchr(Marker->GetItemPath(lastItem), '/');
		if(!filenamePtr)
			filenamePtr = Marker->GetItemPath(lastItem);
		else
			filenamePtr++;
	}

	u64 ModTime = 0;
	u8 archiveType = 0;
	u64 totalSize = 0;
	u64 compSize = 0;
	u32 fileCount = 0;

	for(u32 i = 0; i < archive->GetItemCount(); ++i)
	{
		ArchiveFileStruct *fileStruct = archive->GetFileStruct(i);

		if(fileStruct->isdir)
			continue;

		for(int n = 0; n < Marker->GetItemcount(); ++n)
		{
			bool isMarked = false;
			const char *path = Marker->GetItemPath(n);
			if(Marker->IsItemDir(n))
			{
				if(   strlen(path) < strlen(fileStruct->filename)
				   && strncasecmp(path, fileStruct->filename, strlen(path)) == 0)
				{
					isMarked = true;
				}
			}
			else if(strcasecmp(path, fileStruct->filename) == 0)
					isMarked = true;

			if(isMarked)
			{
				// save the last
				archiveType = fileStruct->archiveType;
				ModTime = fileStruct->ModTime;
				totalSize += fileStruct->length;
				compSize += fileStruct->comp_length;
				fileCount++;
			}
		}
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

	int maxTxtWidth = dialogBox->GetWidth()-Position_X;
	TitleTxt = new GuiText(filenamePtr, 22, (GXColor){0, 0, 0, 255});
	TitleTxt->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	TitleTxt->SetPosition(0, Position_Y);
	TitleTxt->SetMaxWidth(maxTxtWidth, DOTTED);

	int ImgPos = (TitleTxt->GetTextWidth() > maxTxtWidth ? maxTxtWidth : TitleTxt->GetTextWidth());
	TitleImg = new GuiImage(titleData);
	TitleImg->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	int IconPosition = -(ImgPos/2+titleData->GetWidth())+10;
	if(IconPosition < (30-width/2))
		IconPosition = 30-width/2;
	TitleImg->SetPosition(IconPosition, Position_Y);
	Position_Y += 80;

	sprintf(temp, tr("Filepath:  %s"), Marker->GetItemPath(lastItem) ? Marker->GetItemPath(lastItem) : "");

	filepathTxt =  new GuiText(temp, 20, (GXColor){0, 0, 0, 255});
	filepathTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filepathTxt->SetPosition(Position_X, Position_Y);
	filepathTxt->SetMaxWidth(dialogBox->GetWidth()-Position_X-10, DOTTED);
	Position_Y += 30;

	filecountTxt = new GuiText(tr("Files:"), 20, (GXColor){0, 0, 0, 255});
	filecountTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filecountTxt->SetPosition(Position_X, Position_Y);

	filecountTxtVal = new GuiText(fmt("%i", fileCount), 20, (GXColor){0, 0, 0, 255});
	filecountTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filecountTxtVal->SetPosition(Position_X+180, Position_Y);
	Position_Y += 30;

	if(totalSize > (u64) GBSIZE)
		sprintf(temp, "%0.2fGB", totalSize/GBSIZE);
	else if(totalSize > (u64) MBSIZE)
		sprintf(temp, "%0.2fMB", totalSize/MBSIZE);
	else if(totalSize > (u64) KBSIZE)
		sprintf(temp, "%0.2fKB", totalSize/KBSIZE);
	else
		sprintf(temp, "%iB", (u32) totalSize);

	filesizeTxt = new GuiText(tr("Size:"), 20, (GXColor){0, 0, 0, 255});
	filesizeTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filesizeTxt->SetPosition(Position_X, Position_Y);

	filesizeTxtVal = new GuiText(temp, 20, (GXColor){0, 0, 0, 255});
	filesizeTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filesizeTxtVal->SetPosition(Position_X+180, Position_Y);
	Position_Y += 30;

	if(compSize > (u64) GBSIZE)
		sprintf(temp, "%0.2fGB", compSize/GBSIZE);
	else if(compSize > (u64) MBSIZE)
		sprintf(temp, "%0.2fMB", compSize/MBSIZE);
	else if(compSize > (u64) KBSIZE)
		sprintf(temp, "%0.2fKB", compSize/KBSIZE);
	else
		sprintf(temp, "%iB", (u32) compSize);

	filesizeCompTxt = new GuiText(tr("Compressed Size:"), 20, (GXColor){0, 0, 0, 255});
	filesizeCompTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filesizeCompTxt->SetPosition(Position_X, Position_Y);

	filesizeCompTxtVal = new GuiText(temp, 20, (GXColor){0, 0, 0, 255});
	filesizeCompTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filesizeCompTxtVal->SetPosition(Position_X+180, Position_Y);
	Position_Y += 30;

	const char * pch = NULL;
	if(isDir)
	{
		snprintf(temp, sizeof(temp), tr("Folder"));
		TitleTxt->SetMaxWidth(dialogBox->GetWidth()-75, DOTTED);
	}
	else if(filenamePtr)
	{
		pch = strrchr(filenamePtr, '.');
		if(pch)
			pch += 1;
		else
			pch = filenamePtr;
		snprintf(temp, sizeof(temp), "%s", pch);
	}
	else
		temp[0] = 0;

	filetypeTxt = new GuiText(tr("Filetype:"), 20, (GXColor){0, 0, 0, 255});
	filetypeTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filetypeTxt->SetPosition(Position_X, Position_Y);

	filetypeTxtVal = new GuiText(temp, 20, (GXColor){0, 0, 0, 255});
	filetypeTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	filetypeTxtVal->SetPosition(Position_X+180, Position_Y);
	Position_Y += 30;

	last_modifTxt = new GuiText(tr("Last modified:"), 20, (GXColor){0, 0, 0, 255});
	last_modifTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	last_modifTxt->SetPosition(Position_X, Position_Y);

	TimeStruct ptm;
	if(archiveType == SZIP)
	{
		ConvertNTFSDate(ModTime, &ptm);
	}
	else
	{
		ConvertDosDate(ModTime, &ptm);
	}
	snprintf(temp, sizeof(temp), "%02d:%02d  %02d.%02d.%04d", ptm.tm_hour, ptm.tm_min, ptm.tm_mday, ptm.tm_mon, ptm.tm_year);
	last_modifTxtVal = new GuiText(temp, 20, (GXColor){0, 0, 0, 255});
	last_modifTxtVal->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	last_modifTxtVal->SetPosition(Position_X+180, Position_Y);
	Position_Y += 30;

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
	CloseBtn->Clicked.connect(this, &ArchiveProperties::OnButtonClick);

	Append(dialogBoxImg);
	Append(TitleTxt);
	if(isDir)
		Append(TitleImg);
	Append(filepathTxt);
	Append(filecountTxt);
	Append(filecountTxtVal);
	Append(filesizeTxt);
	Append(filesizeTxtVal);
	Append(filesizeCompTxt);
	Append(filesizeCompTxtVal);
	Append(filetypeTxt);
	Append(filetypeTxtVal);
	Append(last_modifTxt);
	Append(last_modifTxtVal);
	Append(CloseBtn);

	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 40);
}

ArchiveProperties::~ArchiveProperties()
{
	RemoveAll();

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
	delete filecountTxt;
	delete filecountTxtVal;
	delete filesizeTxt;
	delete filesizeTxtVal;
	delete filesizeCompTxt;
	delete filesizeCompTxtVal;
	delete filetypeTxt;
	delete filetypeTxtVal;
	delete last_modifTxt;
	delete last_modifTxtVal;

	delete CloseBtn;
	delete trigA;
	delete trigB;
}

void ArchiveProperties::OnButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 40);
	Application::Instance()->PushForDelete(this);
}
