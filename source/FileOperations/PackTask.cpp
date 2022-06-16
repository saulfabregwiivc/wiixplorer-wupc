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
#include "PackTask.h"
#include "Controls/Application.h"
#include "Controls/Taskbar.h"
#include "Prompts/PromptWindow.h"
#include "Prompts/ProgressWindow.h"
#include "FileOperations/fileops.h"

PackTask::PackTask(const ItemMarker *p, const std::string &dest, ArchiveHandle *a, int comp)
	: ProcessTask(tr("Compressing item(s):"), p, dest), archive(a), compression(comp)
{
	if(archive)
		archive->AddReference();
}

PackTask::~PackTask()
{
	if(archive && archive->RemoveReference() <= 0)
		delete archive;
}

static inline void RemoveDoubleSlash(char *str)
{
	if(!str) return;
	int count = 0;
	const char *ptr = str;
	while(*ptr != 0)
	{
		if(*ptr == '/' && ptr[1] == '/') {
			ptr++;
			continue;
		}
		str[count] = *ptr;
		ptr++;
		count++;
	}

	str[count] = 0;
}

void PackTask::Execute(void)
{
	TaskBegin(this);

	// No items to process
	if(Process.GetItemcount() == 0)
	{
		TaskEnd(this);
		return;
	}

	if(ProgressWindow::Instance()->IsRunning())
		ProgressWindow::Instance()->SetTitle(tr("Calculating transfer size..."));
	else
		StartProgress(tr("Calculating transfer size..."));

	list<ItemList> itemList;
	GetItemList(itemList, false);
	list<ItemList>().swap(itemList);

	ProgressWindow::Instance()->SetTitle(this->getTitle().c_str());
	ProgressWindow::Instance()->SetCompleteValues(0, CopySize);

	int result = 0;
	char destpath[MAXPATHLEN];

	for(int i = 0; i < Process.GetItemcount(); i++)
	{
		int ret;
		if(destPath.size() > 0)
			snprintf(destpath, sizeof(destpath), "%s/%s", destPath.c_str(), Process.GetItemName(i));
		else
			snprintf(destpath, sizeof(destpath), "%s", Process.GetItemName(i));

		RemoveDoubleSlash(destpath);

		if(Process.IsItemDir(i))
			ret = archive->AddDirectory(Process.GetItemPath(i), destpath, compression);
		else
			ret = archive->AddFile(Process.GetItemPath(i), destpath, compression);
		if(ret < 0)
			result = ret;

		if(ProgressWindow::Instance()->IsCanceled())
		{
			result = PROGRESS_CANCELED;
			break;
		}

	}

	if(!Application::isClosing() && result != PROGRESS_CANCELED)
	{
		if(result == -30)
			ThrowMsg(tr("Error:"), tr("Pasting files is currently only supported on ZIP archives."));
		else if(result < 0)
			ThrowMsg(tr("Error:"), tr("Failed adding some item(s)."));
	}

	TaskEnd(this);
}
