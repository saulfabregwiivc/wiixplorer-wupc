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
#include "DeleteTask.h"
#include "Controls/Application.h"
#include "Controls/Taskbar.h"
#include "Prompts/PromptWindow.h"
#include "Prompts/ProgressWindow.h"
#include "FileOperations/fileops.h"

DeleteTask::DeleteTask(const ItemMarker *p)
	: ProcessTask(tr("Deleting item(s):"), p, "")
{
}

DeleteTask::~DeleteTask()
{
}

void DeleteTask::Execute(void)
{
	TaskBegin(this);

	// No items to process
	if(Process.GetItemcount() == 0)
	{
		TaskEnd(this);
		return;
	}

	if(ProgressWindow::Instance()->IsRunning())
		ProgressWindow::Instance()->SetTitle(tr("Calculating delete size..."));
	else
		StartProgress(tr("Calculating delete size..."));

	list<ItemList> itemList;

	int result = 0;

	if(GetItemList(itemList, true) < 0) {
		result = -1;
	}

	ProgressWindow::Instance()->SetTitle(this->getTitle().c_str());
	ProgressWindow::Instance()->SetCompleteValues(0, -1);
	ProgressWindow::Instance()->SetUnit(tr("files"));

	int doneItems = 0;
	int TotalItems = 0;

	for(list<ItemList>::iterator listItr = itemList.begin(); listItr != itemList.end(); listItr++)
		TotalItems += listItr->files.size() + listItr->dirs.size();

	for(list<ItemList>::iterator listItr = itemList.begin(); listItr != itemList.end(); listItr++)
	{
		//! Remove all files first
		for(list<string>::iterator itr = listItr->files.begin(); itr != listItr->files.end(); itr++)
		{
			if(ProgressWindow::Instance()->IsCanceled())
				break;

			string filepath = listItr->basepath + *itr;
			const char *filename = strrchr(filepath.c_str(), '/');

			int ret = RemoveFile(filepath.c_str());
			if(ret < 0)
				result = ret;

			ShowProgress(doneItems++, TotalItems, filename ? filename+1 : "");
		}

		//! Remove all dirs reverse
		for(list<string>::iterator itr = listItr->dirs.begin(); itr != listItr->dirs.end(); itr++)
		{
			if(ProgressWindow::Instance()->IsCanceled())
				break;

			string filepath = listItr->basepath + *itr;
			const char *filename = strrchr(filepath.c_str(), '/');

			int ret = RemoveFile(filepath.c_str());
			if(ret < 0)
				result = ret;

			ShowProgress(doneItems++, TotalItems, filename ? filename+1 : "");
		}

		if(ProgressWindow::Instance()->IsCanceled())
		{
			result = PROGRESS_CANCELED;
			break;
		}
	}

	ProgressWindow::Instance()->SetUnit(NULL);

	if(result < 0 && result != PROGRESS_CANCELED && !Application::isClosing())
	{
		ThrowMsg(tr("Error:"), tr("Failed deleting some item(s)."));
	}

	TaskEnd(this);
}
