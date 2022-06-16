/****************************************************************************
 * Copyright (C) 2013 Dimok
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
#ifndef BOOTHOMEBREWTASK_H_
#define BOOTHOMEBREWTASK_H_

#include "BootHomebrew/BootHomebrew.h"
#include "Controls/Application.h"
#include "Controls/Taskbar.h"
#include "Controls/Task.hpp"
#include "Controls/ThreadedTaskHandler.hpp"
#include "FileOperations/FileLoadTask.h"
#include "Tools/tools.h"

class BootHomebrewTask : public ThreadedTask, public GuiElement, public sigslot::has_slots<>
{
public:
	//! without pre-loading a file
	BootHomebrewTask()
		: bAutoRunOnLoadFinish(true)
		, bGcHomebrew(false)
	{ }

	//! with loading file asynchronous
	BootHomebrewTask(const std::string &filepath, bool gchomebrew = false, bool silent = false)
		: bAutoRunOnLoadFinish(true)
		, bGcHomebrew(gchomebrew)
	{
		//! run asynchronous file loading
		FileLoadTask *loadTask = new FileLoadTask(filepath, silent);
		loadTask->SetAutoDelete(true);
		loadTask->LoadingComplete.connect(this, &BootHomebrewTask::OnFileLoadFinish);
		loadTask->LoadingFailed.connect(this, &BootHomebrewTask::OnFileLoadFail);
		Taskbar::Instance()->AddTask(loadTask);
		ThreadedTaskHandler::Instance()->AddTask(loadTask);
	}

	virtual ~BootHomebrewTask() {}

	void SetAutoRunOnLoadFinish(bool b) { bAutoRunOnLoadFinish = b; }

	// execute homebrew boot which must run from main thread
	void Execute(void)
	{
		if(bGcHomebrew)
			BootGameCubeHomebrew();
		else
			BootHomebrew();
	}

private:
	void OnFileLoadFinish(u8 *buffer, u32 size)
	{
		FreeHomebrewBuffer();
		CopyHomebrewMemory(buffer, 0, size);

		free(buffer);

		if(bAutoRunOnLoadFinish)
			Application::Instance()->addPostRenderTask(this);
	}

	void OnFileLoadFail(int result UNUSED)
	{
		Application::Instance()->PushForDelete(this);
	}

	bool bAutoRunOnLoadFinish;
	bool bGcHomebrew;
};

#endif /* BOOTHOMEBREWTASK_H_ */
