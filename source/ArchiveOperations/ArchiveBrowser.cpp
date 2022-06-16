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
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "Controls/Application.h"
#include "ArchiveBrowser.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/ProgressWindow.h"
#include "Prompts/ThrobberWindow.h"
#include "FileOperations/fileops.h"
#include "FileStartUp/FileStartUp.h"

/****************************************************************************
 * ArchiveBrowser Class to parse archive directories
 ***************************************************************************/
ArchiveBrowser::ArchiveBrowser(const char * filepath)
	:Browser()
{
	PageIndex = 0;
	SelIndex = 0;
	ItemNumber = 0;
	archive = NULL;
	displayPath = NULL;
	OrigArchiveFilename = NULL;
	strcpy(currentPath, "");

	if(!filepath)
		return;

	ThrobberWindow * Prompt = new ThrobberWindow(tr("Reading archive."), tr("Please wait..."));
	Prompt->DimBackground(true);
	Application::Instance()->Append(Prompt);
	Application::Instance()->SetUpdateOnly(Prompt);

	char * tmp = strrchr(filepath, '/')+1;
	OrigArchiveFilename = new char[strlen(tmp)+1];
	sprintf(OrigArchiveFilename, "%s", tmp);

	snprintf(currentPath, sizeof(currentPath), "%s", filepath);

	lwp_t parsethread = LWP_THREAD_NULL;
	if(LWP_CreateThread(&parsethread, ParseThreadCallback, this, 0, 32708, 100) < 0)
		return;

	while(!LWP_ThreadIsSuspended(parsethread))
		Application::Instance()->updateEvents();

	LWP_ResumeThread(parsethread);
	LWP_JoinThread(parsethread, NULL);

	currentPath[0] = 0;
	archive->AddReference();
	ItemNumber = archive->GetItemCount();
	ParseArchiveDirectory(NULL);

	Prompt->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	Application::Instance()->PushForDelete(Prompt);
}

ArchiveBrowser::ArchiveBrowser(ArchiveHandle * a)
	:Browser()
{
	PageIndex = 0;
	SelIndex = 0;
	ItemNumber = 0;
	displayPath = NULL;
	OrigArchiveFilename = NULL;
	strcpy(currentPath, "");

	archive = a;
	if(!a)
		return;

	archive->AddReference();
	ItemNumber = archive->GetItemCount();
	ParseArchiveDirectory(NULL);
}

/****************************************************************************
 * Destructor
 ***************************************************************************/
ArchiveBrowser::~ArchiveBrowser()
{
	ClearList();

	if(OrigArchiveFilename)
		delete [] OrigArchiveFilename;

	if(displayPath)
		delete [] displayPath;

	if(archive && archive->RemoveReference() <= 0)
		delete archive;
}

int ArchiveBrowser::ExecuteFile(const char *filepath)
{
	if(!filepath)
		return -1;

	const char *filename = strrchr(filepath, '/');
	if(!filename)
		return -2;
	filename++;

	u32 index;
	for(index = 0; index < PathStructure.size(); ++index)
	{
		if(strcasecmp(filename, PathStructure[index]->filename) == 0)
			break;
	}

	if(index == PathStructure.size())
		return -1;

	int choice = WindowPrompt(tr("Extract file to temp path and execute?"), PathStructure.at(index)->filename, tr("Yes"), tr("Cancel"));
	if(choice == 0)
		return -1;

	if(strlen(Settings.TempPath) > 0 && Settings.TempPath[strlen(Settings.TempPath)-1] != '/')
		strcat(Settings.TempPath, "/");

	if(!CreateSubfolder(Settings.TempPath))
	{
		ShowError("Can't create temp directory.");
		return -1;
	}

	StartProgress(tr("Extracting item(s):"));
	int result = ExtractItem(index, Settings.TempPath);
	StopProgress();
	if(result < 0)
	{
		ShowError(tr("Failed extracting the item(s)."));
		return -1;
	}

	char exePath[MAXPATHLEN];
	snprintf(exePath, sizeof(exePath), "%s%s", Settings.TempPath, PathStructure.at(index)->filename);

	return FileStartUp(exePath);
}

int ArchiveBrowser::AddItem(const ItemStruct * Item, const char * arc_filepath, int compression)
{
	if(!Item || !archive)
		return -1;

	if(Item->isdir)
		return archive->AddDirectory(Item->itempath, arc_filepath, compression);

	return archive->AddFile(Item->itempath, arc_filepath, compression);
}

int ArchiveBrowser::ExtractItem(int ind, const char * dest)
{
	if(ind < 0 || ind >= (int) PathStructure.size())
		return -1;

	ArchiveFileStruct * currentItem = archive->GetFileStruct(PathStructure.at(ind)->fileindex);
	if(!currentItem)
		return -1;

	if(currentItem->isdir)
	{
		return ExtractFolder(currentItem->filename, dest);
	}

	return archive->ExtractFile(currentItem->fileindex, dest, false);
}

int ArchiveBrowser::ExtractFolder(const char *name, const char * dest)
{
	int result = 0;
	char foldername[strlen(name)+1];
	snprintf(foldername, sizeof(foldername), "%s", name);

	char * RealName = strrchr(foldername, '/');

	for (u32 i = 0; i < ItemNumber; i++)
	{
		ArchiveFileStruct * TmpArchive = archive->GetFileStruct(i);
		if(!TmpArchive)
			continue;

		//Filter all not to the path belonging files and folders
		if(strncasecmp(foldername, TmpArchive->filename, strlen(foldername)) == 0)
		{
			char realdest[MAXPATHLEN];
			snprintf(realdest, sizeof(realdest), "%s/%s", dest, TmpArchive->filename);

			if(RealName)
			{
				char * pathptr = strstr(TmpArchive->filename, RealName+1);
				if(pathptr)
					snprintf(realdest, sizeof(realdest), "%s/%s", dest, pathptr);
			}

			//Strip filename from folder7
			char * stripPtr = strrchr(realdest, '/');
			if(stripPtr)
			{
				stripPtr += 1;
				stripPtr[0] = '\0';
			}
			result = archive->ExtractFile(TmpArchive->fileindex, realdest, false);
			if(result <= 0)
				break;
		}
	}

	return result;
}

int ArchiveBrowser::ExtractAll(const char * dest)
{
	return archive->ExtractAll(dest);
}

void ArchiveBrowser::SetPageIndex(int ind)
{
	PageIndex = ind;
}

void ArchiveBrowser::SetSelectedIndex(int ind)
{
	SelIndex = ind;
}

const char * ArchiveBrowser::GetItemName(int ind)
{
	if(ind < 0 || ind >= (int) PathStructure.size())
		return NULL;

	return PathStructure.at(ind)->filename;
}

bool ArchiveBrowser::IsDir(int ind)
{
	if(ind < 0 || ind >= (int) PathStructure.size())
		return false;

	return PathStructure.at(ind)->isdir;
}

int ArchiveBrowser::EnterSelDir()
{
	int dirlength = strlen(currentPath);
	int filelength = strlen(PathStructure.at(SelIndex)->filename);
	if((dirlength+filelength+1) > MAXPATHLEN)
		return -1;

	if(dirlength == 0)
		sprintf(currentPath, "%s", PathStructure.at(SelIndex)->filename);
	else
		sprintf(currentPath, "%s/%s", currentPath, PathStructure.at(SelIndex)->filename);

	return ParseArchiveDirectory(currentPath);
}

int ArchiveBrowser::LeaveCurDir()
{
	char * ptr = strrchr(currentPath, '/');
	if(ptr)
	{
		ptr[0] = '\0';
	}
	else if(!ptr)
	{
		if(strlen(currentPath) != 0)
		{
			strcpy(currentPath, "");
		}
		else
		{
			//leaving Archive because root and ".." was clicked
			return CLOSE_ARCHIVE;
		}
	}

	return ParseArchiveDirectory(currentPath);
}

int ArchiveBrowser::ChangeDirectory()
{
	if(!PathStructure.at(SelIndex)->filename)
		return -1;

	if(strcmp(PathStructure.at(SelIndex)->filename, "..") == 0)
	{
		return LeaveCurDir();
	}

	return EnterSelDir();
}

const char * ArchiveBrowser::GetCurrentPath()
{
	if(!OrigArchiveFilename)
		return currentPath;

	if(displayPath)
		delete [] displayPath;

	displayPath = new char[strlen(OrigArchiveFilename)+strlen(currentPath)+2];

	sprintf(displayPath, "%s/%s", OrigArchiveFilename, currentPath);

	return displayPath;
}

const char * ArchiveBrowser::GetCurrentSelectedFilepath()
{
	if(!OrigArchiveFilename)
		return currentPath;

	const char *Filename = GetCurrentName();
	if(!displayPath)
		return "";

	if(displayPath)
		delete [] displayPath;

	displayPath = new char[strlen(OrigArchiveFilename)+strlen(currentPath)+strlen(Filename)+3];

	if(strlen(currentPath) == 0 || currentPath[strlen(currentPath)-1] == '/')
		sprintf(displayPath, "%s/%s%s", OrigArchiveFilename, currentPath, Filename);
	else
		sprintf(displayPath, "%s/%s/%s", OrigArchiveFilename, currentPath, Filename);

	return displayPath;
}

u64 ArchiveBrowser::GetFilesize(int ind)
{
	if(ind < 0 || ind >= (int) PathStructure.size())
		return 0;

	return PathStructure.at(ind)->length;
}

ArchiveFileStruct * ArchiveBrowser::GetItemStructure(int ind)
{
	if(ind < 0 || ind >= (int) PathStructure.size())
		return NULL;

	return archive->GetFileStruct(PathStructure.at(ind)->fileindex);
}

void ArchiveBrowser::Refresh()
{
	ParseArchiveDirectory(currentPath);
}

void ArchiveBrowser::ReloadList()
{
	if(!archive)
		return;

	archive->ReloadList();
	ItemNumber = archive->GetItemCount();
	Refresh();
}

int ArchiveBrowser::ParseArchiveDirectory(const char * ArcPath)
{
	if(!PathStructure.empty())
		ClearList();

	bool firstpage = false;

	if(ArcPath != NULL)
	{
		if(strcmp(ArcPath, "") == 0 || strcmp(ArcPath, "/") == 0 )
			firstpage = true;
	}
	else
	{
		firstpage = true;
	}

	// add '..' folder in case the user wants exit the archive or go up in directory
	AddListEntrie("..", 0, 0, true, -1, 0, UNKNOWN);

	// get contents and parse them into file list structure
	for (u32 i = 0; i < ItemNumber; i++)
	{
		ArchiveFileStruct * TmpArchive = archive->GetFileStruct(i);
		if(!TmpArchive)
			continue;

		//Filter all not to the path belonging files and folders
		if(InDirectoryTree(ArcPath, TmpArchive->filename, firstpage) == true)
		{
			AddListEntrie(TmpArchive->filename,  TmpArchive->length,
						  TmpArchive->comp_length, TmpArchive->isdir,
						  TmpArchive->fileindex, TmpArchive->ModTime,
						  TmpArchive->archiveType);
		}
	}

	PageIndex = 0;
	SelIndex = 0;

	SortList();

	return PathStructure.size();
}

void ArchiveBrowser::ClearList()
{
	for(u32 i = 0; i < PathStructure.size(); i++)
	{
		if(!PathStructure.at(i))
			continue;

		if(PathStructure.at(i)->filename != NULL)
			delete [] PathStructure.at(i)->filename;

		delete PathStructure.at(i);
	}

	PathStructure.clear();
	std::vector<ArchiveFileStruct *>().swap(PathStructure);
}

void ArchiveBrowser::SortList()
{
	std::sort(PathStructure.begin(), PathStructure.end(), FileSortCallback);
}

void ArchiveBrowser::AddListEntrie(const char * filename, size_t length, size_t comp_length, bool isdir, u32 index, u64 modtime, u8 Type)
{
	if(!filename)
		return;

	ArchiveFileStruct * TempStruct = new ArchiveFileStruct;
	TempStruct->filename = new char[strlen(filename)+1];
	char * realfilename = strrchr(filename, '/');
	if(realfilename == NULL)
		snprintf(TempStruct->filename, strlen(filename)+1, "%s", filename);
	else
		snprintf(TempStruct->filename, strlen(filename)+1, "%s", realfilename+1);
	TempStruct->length = length;
	TempStruct->comp_length = comp_length;
	TempStruct->isdir = isdir;
	TempStruct->fileindex = index;
	TempStruct->ModTime = modtime;
	TempStruct->archiveType = Type;

	PathStructure.push_back(TempStruct);
	TempStruct = NULL;
}

ItemStruct * ArchiveBrowser::GetItemStruct(int pos)
{
	if(pos < 0 || pos >= (int) PathStructure.size())
		return NULL;

	ItemStruct * Item = new ItemStruct;
	memset(Item, 0, sizeof(ItemStruct));

	ArchiveFileStruct * CurArchive = archive->GetFileStruct(PathStructure.at(pos)->fileindex);
	if(!CurArchive)
		return NULL;

	Item->itempath = strdup(CurArchive->filename);
	Item->itemsize = CurArchive->length;
	Item->isdir = CurArchive->isdir;
	Item->itemindex = pos;

	return Item;
}

bool ArchiveBrowser::InDirectoryTree(const char * Path, const char * itemFullFilename, bool firstpage)
{
	if(firstpage)
	{
		if(strrchr(itemFullFilename, '/') == NULL)
		{
			return true;
		}
	}
	else
	{
		if(strncasecmp(Path, itemFullFilename, strlen(Path)) == 0 && strcasecmp(Path, itemFullFilename) != 0)
		{
			char * ptr1 = strrchr(itemFullFilename, '/');
			if(ptr1 != NULL)
			{
				int position = ptr1-itemFullFilename;
				if(position <= (int) strlen(Path))
					return true;
			}
		}
	}

	return false;
}


bool ArchiveBrowser::FileSortCallback(const ArchiveFileStruct * f1, const ArchiveFileStruct * f2)
{
	if(f1->filename[0] == '.' || f2->filename[0] == '.')
	{
		if(strcmp(f1->filename, "..") == 0) { return true; }
		if(strcmp(f2->filename, "..") == 0) { return false; }
	}

	if(f1->isdir && !(f2->isdir)) return true;
	if(!(f1->isdir) && f2->isdir) return false;

	if(stricmp(f1->filename, f2->filename) > 0)
		return false;
	else
		return true;
}

/****************************************************************************
 * ParseThread
 ***************************************************************************/
void *ArchiveBrowser::ParseThreadCallback(void *arg)
{
	ArchiveBrowser *browser = (ArchiveBrowser *) arg;

	browser->archive = new ArchiveHandle(browser->currentPath);

	LWP_SuspendThread(LWP_GetSelf());

	return NULL;
}

