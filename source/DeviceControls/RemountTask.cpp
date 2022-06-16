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
#include "RemountTask.h"
#include "DeviceControls/DeviceHandler.hpp"
#include "Controls/Application.h"
#include "SoundOperations/MusicPlayer.h"
#include "Prompts/ProgressWindow.h"

RemountTask::RemountTask(const char *title, int Device)
	: Task(title), RemountDevice(Device), bAutoDelete(false)
{
}

RemountTask::~RemountTask()
{
}

void RemountTask::Execute(void)
{
	TaskBegin(this);

	ProgressWindow::Instance()->StartProgress(this->getTitle().c_str(), tr("Please wait..."));
	ProgressWindow::Instance()->SetUnit(tr("devices"));

	if(RemountDevice == MAXDEVICES)
	{
		// update progress information
		ProgressWindow::Instance()->ShowProgress(0, MAXDEVICES);

		MusicPlayer::Instance()->Pause();

		DeviceHandler::DestroyInstance();
		DeviceHandler::Instance()->USBSpinUp(10);

		for(int dev = SD; dev < MAXDEVICES; ++dev)
		{
			if(ProgressWindow::Instance()->IsCanceled())
				break;

			ProgressWindow::Instance()->ShowProgress(dev, MAXDEVICES);
			DeviceHandler::Instance()->Mount(dev);
		}

		MusicPlayer::Instance()->Resume();
	}
	else
	{
		ProgressWindow::Instance()->ShowProgress(0, 1);
		DeviceHandler::Instance()->UnMount(RemountDevice);
		DeviceHandler::Instance()->Mount(RemountDevice);
		ProgressWindow::Instance()->ShowProgress(1, 1);
	}

	ProgressWindow::Instance()->StopProgress();
	ProgressWindow::Instance()->SetUnit(NULL);

	TaskEnd(this);

	if(bAutoDelete)
		Application::Instance()->PushForDelete(this);
}
