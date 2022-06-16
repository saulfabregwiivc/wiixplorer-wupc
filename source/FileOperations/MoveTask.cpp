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
#include "MoveTask.h"
#include "Controls/Application.h"
#include "Controls/Taskbar.h"
#include "Prompts/PromptWindow.h"
#include "Prompts/ProgressWindow.h"
#include "FileOperations/fileops.h"

MoveTask::MoveTask(const ItemMarker *p, const std::string &dest)
	: ProcessTask(tr("Moving item(s):"), p, dest)
{
	this->SetTitle(tr("Moving item(s):"));
}

MoveTask::~MoveTask()
{
}

void MoveTask::Execute(void)
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

	ProgressWindow::Instance()->SetTitle(this->getTitle().c_str());

	string destPathSlash = (destPath.size() > 0 && destPath[destPath.size()-1] != '/') ? destPath + '/' : destPath;

	int result = 0;

	//! On same device we move files instead of copy them
	for(int i = 0; i < Process.GetItemcount(); ++i)
	{
		if(CompareDevices(Process.GetItemPath(i), destPathSlash.c_str()))
		{
			string srcpath = Process.GetItemPath(i);
			while(srcpath[srcpath.size()-1] == '/')
				srcpath.erase(srcpath.size()-1);

			const char *pathname = strrchr(srcpath.c_str(), '/');
			if(!pathname)
				continue;

			string dstpath = destPathSlash + (pathname+1);

			if(strcasecmp(srcpath.c_str(), dstpath.c_str()) == 0)
			{
				//! nothing to be done here
				Process.RemoveItem(Process.GetItem(i));
				i--;
				continue;
			}

			//! moving directory to a path where the same directory name exists
			//! we will move all files and remove src directory in the later process
			if(Process.IsItemDir(i) && CheckFile(dstpath.c_str()))
				continue;

			int ret = MoveFile(srcpath.c_str(), dstpath.c_str());
			if(ret < 0)
				result = ret;

			Process.RemoveItem(Process.GetItem(i));
			i--;
		}
	}

	list<ItemList> itemList;

	if(GetItemList(itemList, true) < 0) {
		result = -1;
	}

	//! free memory of process which is no longer required
	Process.Reset();

	//! On same device we move files instead of copy them
	ProgressWindow::Instance()->SetCompleteValues(0, CopySize);

	for(list<ItemList>::iterator listItr = itemList.begin(); listItr != itemList.end(); listItr++)
	{
		//! first move/remove all files in all sub directories
		for(list<string>::iterator itr = listItr->files.begin(); itr != listItr->files.end(); itr++)
		{
			if(ProgressWindow::Instance()->IsCanceled())
				break;

			string srcpath = listItr->basepath + *itr;
			string dstpath = destPathSlash + *itr;

			string folderpath = dstpath;
			size_t pos = folderpath.rfind('/');
			if(pos != string::npos)
				folderpath.erase(pos);

			CreateSubfolder(folderpath.c_str());

			int ret = MoveFile(srcpath.c_str(), dstpath.c_str());
			if(ret < 0)
				result = ret;
		}

		//! Remove all dirs reversed as they were appended to the list
		for(list<string>::iterator itr = listItr->dirs.begin(); itr != listItr->dirs.end(); itr++)
		{
			if(ProgressWindow::Instance()->IsCanceled())
				break;

			RemoveFile((listItr->basepath + *itr).c_str());
		}

		if(ProgressWindow::Instance()->IsCanceled())
		{
			result = PROGRESS_CANCELED;
			break;
		}
	}

	if(result < 0 && result != PROGRESS_CANCELED && !Application::isClosing())
	{
		ThrowMsg(tr("Error:"), tr("Failed moving some item(s)."));
	}

	TaskEnd(this);
}
