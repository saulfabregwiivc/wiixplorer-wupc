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
#ifndef UPDATETASK_H_
#define UPDATETASK_H_

#include "Controls/ThreadedTaskHandler.hpp"
#include "Controls/Task.hpp"

class UpdateTask : public ThreadedTask, public Task
{
public:
	UpdateTask(bool bUpdateApp, bool bUpdateLang, bool silent);
	virtual ~UpdateTask();
	virtual void Execute(void);
	void SetAutoDelete(bool b) { bAutoDelete = b; }
	int CheckForUpdate(void);
	int DownloadApp(const char *url);
	bool DownloadMetaXml(void);
	bool DownloadIconPNG(void);
	bool UpdateLanguageFiles(void);
private:
	bool bAutoDelete;
	bool bUpdateApp;
	bool bUpdateLang;
	bool bSilent;
};

#endif /* UPDATETASK_H_ */
