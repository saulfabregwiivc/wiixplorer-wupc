/****************************************************************************
 * Copyright (C) 2012 Dimok
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
#ifndef DOWNLOADTASK_H_
#define DOWNLOADTASK_H_

#include "Controls/ThreadedTaskHandler.hpp"
#include "Controls/Task.hpp"

class DownloadTask : public ThreadedTask, public Task
{
public:
	DownloadTask(const char *title, const std::string &DownloadURL, const char *Filepath);
	virtual ~DownloadTask();
	virtual void Execute(void);
	void SetAutoDelete(bool b) { bAutoDelete = b; }
	sigslot::signal3<int, u8 *, u32> DownloadFinished;
private:
	std::string DownloadURL;
	const char *Filepath;
	bool bAutoDelete;
};

#endif /* REMOUNTTASK_H_ */
