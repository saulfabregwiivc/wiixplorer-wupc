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
#ifndef REMOUNTTASK_H_
#define REMOUNTTASK_H_

#include "Controls/ThreadedTaskHandler.hpp"
#include "Controls/Task.hpp"

class RemountTask : public ThreadedTask, public Task
{
public:
	RemountTask(const char *title, int Device);
	virtual ~RemountTask();
	virtual void Execute(void);
	void SetAutoDelete(bool b) { bAutoDelete = b; }
private:
	int RemountDevice;
	bool bAutoDelete;
};

#endif /* REMOUNTTASK_H_ */
