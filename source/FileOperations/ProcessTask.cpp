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
#include "ProcessTask.h"
#include "Controls/Taskbar.h"
#include "Prompts/ProgressWindow.h"
#include "FileOperations/fileops.h"

ProcessTask::ProcessTask(const std::string &title, const ItemMarker *p, const std::string &dest)
	: Task(title), destPath(dest)
{
	TaskType = Task::PROCESS;
	CopyFiles = 0;
	CopySize = 0;
	Process = *p;

	ShowNormal.connect(this, &ProcessTask::ShowProgressWindow);
}

void ProcessTask::ShowProgressWindow(Task *task UNUSED, int param UNUSED)
{
	ProgressWindow::Instance()->OpenWindow();
}

int ProcessTask::GetItemList(list<ItemList> &fileLists, bool listDirs)
{
	int ret = 0;

	for(int i = 0; i < Process.GetItemcount(); i++)
	{
		if(ProgressWindow::Instance()->IsCanceled())
			break;

		if(Process.IsItemDir(i) == true)
		{
			fileLists.resize(fileLists.size()+1);
			fileLists.back().basepath = Process.GetItemPath(i);

			while(   fileLists.back().basepath.size() > 0
				  && fileLists.back().basepath[fileLists.back().basepath.size()-1] == '/')
				  fileLists.back().basepath.erase(fileLists.back().basepath.size()-1);

			size_t pos = fileLists.back().basepath.rfind('/');
			if(pos != std::string::npos)
				fileLists.back().basepath.erase(pos+1);

			string path = Process.GetItemName(i);

			int res = ReadDirectory(path, fileLists.back(), listDirs);
			if(res < 0)
				ret = res;
		}
		else
		{
			fileLists.resize(fileLists.size()+1);
			fileLists.back().basepath = Process.GetItemPath(i);
			size_t pos = fileLists.back().basepath.rfind('/');
			if(pos == string::npos) {
				fileLists.resize(fileLists.size()-1);
				continue;
			}
			//! split into base path and the filename
			fileLists.back().files.push_back(fileLists.back().basepath.substr(pos+1));
			fileLists.back().basepath = fileLists.back().basepath.substr(0, pos+1);
			CopySize += FileSize(Process.GetItemPath(i));
			++CopyFiles;
		}
		// Update progress window
		ShowProgress(0, CopySize);
	}

	return ret;
}

int ProcessTask::ReadDirectory(string &path, ItemList &fileList, bool listDirs)
{
	int ret = 0;
	struct dirent *dirent = NULL;

	DIR *dir = opendir((fileList.basepath + path).c_str());
	if(dir == NULL)
		return -1;

	while((dirent = readdir(dir)) != 0)
	{
		if(ProgressWindow::Instance()->IsCanceled())
			break;

		if(dirent->d_type & DT_DIR)
		{
			if(strcmp(dirent->d_name,".") != 0 && strcmp(dirent->d_name,"..") != 0)
			{
				int pos = path.size();
				path += '/';
				path += dirent->d_name;

				if(ReadDirectory(path, fileList, listDirs) < 0)
					ret = -2;

				path.erase(pos);
			}
		}
		else
		{
			string filepath(path + "/" + dirent->d_name);
			struct stat st;

			if(stat((fileList.basepath + filepath).c_str(), &st) != 0)
				continue;

			fileList.files.push_back(filepath);
			CopySize += st.st_size;
			++CopyFiles;
		}
		// Update progress window
		ShowProgress(0, CopySize);
	}
	closedir(dir);

	if(listDirs)
		fileList.dirs.push_back(path);

	return ret;
}
