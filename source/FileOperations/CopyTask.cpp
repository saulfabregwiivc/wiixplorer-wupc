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
#include "CopyTask.h"
#include "Controls/Application.h"
#include "Controls/Taskbar.h"
#include "Prompts/PromptWindow.h"
#include "Prompts/ProgressWindow.h"
#include "FileOperations/fileops.h"

CopyTask::CopyTask(const ItemMarker *p, const std::string &dest)
	: ProcessTask(tr("Copying item(s):"), p, dest)
{
}

CopyTask::~CopyTask()
{
}

void CopyTask::Execute(void)
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

	int result = 0;

	if(GetItemList(itemList, false) < 0) {
		result = -1;
	}

	//! free memory of process which is no longer required
	Process.Reset();

	ProgressWindow::Instance()->SetTitle(this->getTitle().c_str());
	ProgressWindow::Instance()->SetCompleteValues(0, CopySize);

	string destPathSlash = (destPath.size() > 0 && destPath[destPath.size()-1] != '/') ? destPath + '/' : destPath;

	for(list<ItemList>::iterator listItr = itemList.begin(); listItr != itemList.end(); listItr++)
	{
		for(list<string>::iterator itr = listItr->files.begin(); itr != listItr->files.end(); itr++)
		{
			string srcpath = listItr->basepath + *itr;
			string dstpath = destPathSlash + *itr;

			string folderpath = dstpath;
			size_t pos = folderpath.rfind('/');
			if(pos != string::npos)
				folderpath.erase(pos);

			CreateSubfolder(folderpath.c_str());

			if(strcasecmp(srcpath.c_str(), dstpath.c_str()) == 0)
				dstpath = destPathSlash + tr("Copy of ") + *itr;

			int ret = CopyFile(srcpath.c_str(), dstpath.c_str());
			if(ret < 0)
				result = ret;
		}

		if(ProgressWindow::Instance()->IsCanceled())
		{
			result = PROGRESS_CANCELED;
			break;
		}
	}

	if(result < 0 && result != PROGRESS_CANCELED && !Application::isClosing())
	{
		ThrowMsg(tr("Error:"), tr("Failed copying some item(s)."));
	}
	TaskEnd(this);
}
