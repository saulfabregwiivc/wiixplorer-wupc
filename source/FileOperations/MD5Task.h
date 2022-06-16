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
#ifndef MD5TASK_HPP_
#define MD5TASK_HPP_

#include <vector>
#include "FileOperations/ItemMarker.h"
#include "FileOperations/ProcessTask.h"

class MD5Task : public ProcessTask
{
public:
	MD5Task(const ItemMarker *p, const std::string &dest);
	virtual ~MD5Task();
	void Execute(void);
private:
	bool OpenLog(const char * path);
	void CloseLog();
	bool CalculateFile(const char * filepath);
	bool CalculateDirectory(const char * path);

	FILE * LogFile;
	int FolderCounter;
	int FileCounter;
	int ErrorCounter;
};

#endif
