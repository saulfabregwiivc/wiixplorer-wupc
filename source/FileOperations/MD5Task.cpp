/****************************************************************************
 * Copyright (C) 2011 Dimok
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
#include <stdio.h>
#include <malloc.h>
#include <sys/dir.h>
#include <algorithm>
#include "MD5Task.h"
#include "Language/gettext.h"
#include "FileOperations/fileops.h"
#include "FileOperations/MD5.h"
#include "Prompts/ProgressWindow.h"
#include "DirList.h"

#define BLOCKSIZE   102400

MD5Task::MD5Task(const ItemMarker *p, const std::string &dest)
	: ProcessTask(tr("Calculating MD5"), p, dest),
	  LogFile(NULL), FolderCounter(0), FileCounter(0), ErrorCounter(0)
{
}

MD5Task::~MD5Task()
{
	CloseLog();
}

bool MD5Task::OpenLog(const char * path)
{
	LogFile = fopen(path, "ab");
	if(!LogFile)
		LogFile = fopen(path, "wb");

	return (LogFile != NULL);
}

void MD5Task::CloseLog()
{
	if(LogFile)
		fclose(LogFile);

	LogFile = NULL;
}

void MD5Task::Execute(void)
{
	TaskBegin(this);

	if(Process.GetItemcount() == 0 || !OpenLog(destPath.c_str()))
	{
		ThrowMsg(tr("Error:"), tr("MD5 log file can't be created."));
		TaskEnd(this);
	}

	if(ProgressWindow::Instance()->IsRunning())
		ProgressWindow::Instance()->SetTitle(tr("Calculating total size..."));
	else
		StartProgress(tr("Calculating total size..."));

	list<ItemList> itemList;
	GetItemList(itemList, false);
	list<ItemList>().swap(itemList);

	ProgressWindow::Instance()->SetTitle(this->getTitle().c_str());
	ProgressWindow::Instance()->SetCompleteValues(0, CopySize);

	FolderCounter = 0;
	FileCounter = 0;
	ErrorCounter = 0;

	fprintf(LogFile, tr("--------------------------------------------------\n"));
	fprintf(LogFile, tr("WiiXplorer MD5 Calculator\n"));

	char currentpath[1024];

	for(int i = 0; i < Process.GetItemcount(); i++)
	{
		if(ProgressWindow::Instance()->IsCanceled())
			break;

		fprintf(LogFile, "\n");

		if(Process.IsItemDir(i))
		{
			snprintf(currentpath, sizeof(currentpath), "%s/", Process.GetItemPath(i));
			fprintf(LogFile, "%s %s\n\n", tr("Checking directory:"), currentpath);
			CalculateDirectory(currentpath);
		}
		else
		{
			snprintf(currentpath, sizeof(currentpath), "%s", Process.GetItemPath(i));
			fprintf(LogFile, "%s %s\n\n", tr("Checking file:"), currentpath);
			CalculateFile(currentpath);
		}
	}

	fprintf(LogFile, tr("--------------------------------------------------\n"));
	fprintf(LogFile, tr("Checked %i file(s) in %i folder(s).\n"), FileCounter, FolderCounter);
	fprintf(LogFile, tr("Encountered %i error(s).\n"), ErrorCounter);
	fprintf(LogFile, tr("--------------------------------------------------\n\n"));

	CloseLog();

	if(ErrorCounter > 0)
		ThrowMsg(tr("Hashing complete."), fmt(tr("%i error(s) encountered. Check MD5.log for more details."), ErrorCounter));
	else
		ThrowMsg(tr("Hashing successfully finished."), tr("Log was saved in MD5.log"));

	TaskEnd(this);
}

bool MD5Task::CalculateFile(const char * filepath)
{
	if(!LogFile || !filepath)
	{
		++ErrorCounter;
		return false;
	}

	char * filename = strrchr(filepath, '/');
	if(!filename)
	{
		fprintf(LogFile, tr("Error - %s is not a file\n"), filepath);
		++ErrorCounter;
		return false;
	}

	filename++;

	u8 Hash[32];
	auth_md5Ctx ctx[1];
	int read = 0;
	u64 done = 0;
	u64 filesize = FileSize(filepath);

	FILE * file = fopen(filepath, "rb");

	if (file == NULL || filesize == 0)
	{
		fprintf(LogFile, tr("Error - Could not open file: %s\n"), filepath);
		++ErrorCounter;
		return false;
	}

	auth_md5InitCtx(ctx);

	u8 * buffer = (u8 *) malloc(BLOCKSIZE);

	if(!buffer)
	{
		fclose(file);
		fprintf(LogFile, tr("Error - Not enough memory on: %s\n"), filepath);
		++ErrorCounter;
		return false;
	}

	do
	{
		if(ProgressWindow::Instance()->IsCanceled())
		{
			fclose(file);
			free(buffer);
			return false;
		}

		ShowProgress(done, filesize, filename);

		read = fread(buffer, 1, BLOCKSIZE, file);

		if(read > 0)
			auth_md5SumCtx(ctx, buffer, read);

		done += read;
	}
	while(read > 0);

	fclose(file);
	free(buffer);

	// finish up the progress for this file
	FinishProgress(filesize);

	auth_md5CloseCtx(ctx, Hash);

	char HashString[100];
	memset(HashString, 0, sizeof(HashString));

	MD5ToString(Hash, HashString);

	++FileCounter;

	fprintf(LogFile, "%s - %i. %s\n", HashString, FileCounter, filepath);

	return true;
}

bool MD5Task::CalculateDirectory(const char * path)
{
	if(!LogFile || !path)
	{
		++ErrorCounter;
		return false;
	}

	if(ProgressWindow::Instance()->IsRunning())
		ProgressWindow::Instance()->SetTitle(tr("Getting file list..."));
	else
		StartProgress(tr("Getting file list..."));

	DirList dir;
	dir.LoadPath(path, 0, DirList::Files | DirList::Dirs | DirList::CheckSubfolders);
	dir.SortList();

	if(dir.GetFilecount() <= 0)
	{
		fprintf(LogFile, tr("Error - Could not parse directory: %s\n"), path);
		++ErrorCounter;
		return false;
	}

	for(int i = 0; i < dir.GetFilecount(); i++)
	{
		if(ProgressWindow::Instance()->IsCanceled())
			break;

		if(!dir.IsDir(i))
		{
			CalculateFile(dir.GetFilepath(i));
		}
		else
		{
			++FolderCounter;
		}
	}

	return true;
}
