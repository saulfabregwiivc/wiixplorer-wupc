/****************************************************************************
 * Copyright (C) 2010-2011 Dimok
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
#include <malloc.h>
#include "ThreadedTaskHandler.hpp"

ThreadedTaskHandler * ThreadedTaskHandler::instance = NULL;

ThreadedTaskHandler::ThreadedTaskHandler()
	: CThread(80, 65*1024)
	, ExitRequested(false)
{
	startThread();
}

ThreadedTaskHandler::~ThreadedTaskHandler()
{
	ExitRequested = true;
}

void ThreadedTaskHandler::executeThread(void)
{
	while(!ExitRequested)
	{
		suspendThread();

		while(!ExitRequested && !TaskList.empty())
		{
			TaskList.front()->Execute();
			TaskList.pop();
		}
	}
}
