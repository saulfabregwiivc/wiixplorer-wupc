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
#include "FileLoadTask.h"
#include "FileOperations/fileops.h"
#include "Controls/Application.h"
#include "Prompts/ProgressWindow.h"

FileLoadTask::FileLoadTask(const std::string &filepath, bool silent)
	: ProcessTask(tr("Loading file:"), &Process, filepath)
	, m_silent(silent)
	, bAutoDelete(false)
{
}

FileLoadTask::~FileLoadTask()
{
}

void FileLoadTask::Execute(void)
{
	TaskBegin(this);

	// No items to process
	if(destPath.size() == 0)
	{
		TaskEnd(this);
		return;
	}

	u8 *buffer = NULL;
	u32 buffer_size = 0;

	int result;

	if(m_silent)
		result = LoadFileToMem(destPath.c_str(), &buffer, &buffer_size);
	else
		result = LoadFileToMemWithProgress(this->getTitle().c_str(), destPath.c_str(), &buffer, &buffer_size);

	if(!buffer || result < 0)
	{
		if(buffer)
			free(buffer);
		LoadingFailed(result);
	}
	else
	{
		LoadingComplete(buffer, buffer_size);
	}

	TaskEnd(this);

	if(bAutoDelete)
		Application::Instance()->PushForDelete(this);
}
