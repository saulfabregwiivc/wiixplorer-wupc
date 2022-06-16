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
#ifndef PROCESSTASK_H_
#define PROCESSTASK_H_

#include <list>
#include "Controls/ThreadedTaskHandler.hpp"
#include "Controls/Task.hpp"
#include "FileOperations/ItemMarker.h"

using namespace std;

class ProcessTask : public ThreadedTask, public Task
{
public:
	ProcessTask(const string &title, const ItemMarker *process, const string &dest);
	virtual ~ProcessTask() {}
	virtual void Execute(void) = 0;
protected:
	struct ItemList
	{
		string basepath;
		list<string> files;
		list<string> dirs;
	};

	int GetItemList(list<ItemList> &fileLists, bool listDirs);
	int ReadDirectory(string &path, ItemList &fileLists, bool listDirs);
	void ShowProgressWindow(Task *task, int param);

	ItemMarker Process;
	const std::string destPath;
	u64 CopySize;
	u32 CopyFiles;
};

#endif /* PROCESSTASK_H_ */
