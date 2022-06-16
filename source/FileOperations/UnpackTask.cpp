/****************************************************************************
 * Copyright (C) 2012 Dimok
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
#include "UnpackTask.h"
#include "Controls/Application.h"
#include "Controls/Taskbar.h"
#include "Prompts/PromptWindow.h"
#include "Prompts/ProgressWindow.h"
#include "FileOperations/fileops.h"

UnpackTask::UnpackTask(const ItemMarker *p, const std::string &dest, ArchiveHandle *a, bool full)
	: ProcessTask(tr("Extracting item(s):"), p, dest), archive(a), browser(NULL), fullUnpack(full)
{
	if(archive)
	{
		archive->AddReference();
		browser = new ArchiveBrowser(archive);
	}
}

UnpackTask::~UnpackTask()
{
	if(archive && archive->RemoveReference() <= 0)
		delete archive;

	delete browser;
}

u64 UnpackTask::GetUnpackSize(void)
{
	u64 extractSize = 0;

	if(fullUnpack)
	{
		for(u32 i = 0; i < archive->GetItemCount(); ++i)
		{
			ArchiveFileStruct *item = archive->GetFileStruct(i);
			if(!item)
				continue;

			if(!item->isdir)
				extractSize += item->length;
		}
	}
	else
	{
		for(int i = 0; i < Process.GetItemcount(); i++)
		{
			ArchiveFileStruct * currentItem = archive->GetFileStruct(Process.GetItemIndex(i));
			if(!currentItem)
				continue;

			if(currentItem->isdir)
			{
				ArchiveFileStruct * folder = currentItem;

				int length = strlen(folder->filename);

				for(u32 i = 0; i < archive->GetItemCount(); ++i)
				{
					ArchiveFileStruct *item = archive->GetFileStruct(i);
					if(!item)
						continue;

					//! if its a subfolder/file it contains the name of this folder
					if(strncasecmp(folder->filename, item->filename, length) == 0)
					{
						if(!item->isdir)
							extractSize += item->length;
					}
				}
			}
			else
			{
				extractSize += currentItem->length;
			}
		}
	}

	return extractSize;
}

void UnpackTask::Execute(void)
{
	TaskBegin(this);

	// No items to process
	if(!archive || (Process.GetItemcount() == 0 && !fullUnpack))
	{
		TaskEnd(this);
		return;
	}

	if(ProgressWindow::Instance()->IsRunning())
		ProgressWindow::Instance()->SetTitle(tr("Calculating extract size..."));
	else
		StartProgress(tr("Calculating extract size..."));

	CopySize = GetUnpackSize();

	ProgressWindow::Instance()->SetTitle(this->getTitle().c_str());
	ProgressWindow::Instance()->SetCompleteValues(0, CopySize);

	string destPathSlash = (destPath.size() > 0 && destPath[destPath.size()-1] != '/') ? destPath + '/' : destPath;

	int result = 0;

	if(fullUnpack)
	{
		result = browser->ExtractAll(destPathSlash.c_str());
	}
	else
	{
		for(int i = 0; i < Process.GetItemcount(); i++)
		{
			ArchiveFileStruct * currentItem = archive->GetFileStruct(Process.GetItemIndex(i));
			if(currentItem->isdir)
			{
				int ret = browser->ExtractFolder(currentItem->filename, destPathSlash.c_str());
				if(ret < 0)
					result = ret;
			}
			else
			{
				int ret = archive->ExtractFile(currentItem->fileindex, destPathSlash.c_str(), false);
				if(ret < 0)
					result = ret;
			}

			if(ProgressWindow::Instance()->IsCanceled())
			{
				result = PROGRESS_CANCELED;
				break;
			}
		}
	}

	if((result < 0) && !Application::isClosing() && result != PROGRESS_CANCELED)
	{
		ThrowMsg(tr("Error:"), tr("Failed extracting some item(s)."));
	}

	TaskEnd(this);
}
