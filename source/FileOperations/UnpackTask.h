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
#ifndef UNPACKTASK_H_
#define UNPACKTASK_H_

#include "FileOperations/ProcessTask.h"
#include "ArchiveOperations/Archive.h"
#include "ArchiveOperations/ArchiveBrowser.h"

class UnpackTask : public ProcessTask
{
public:
	UnpackTask(const ItemMarker *process, const std::string &dest, ArchiveHandle *archive, bool fullUnpack);
	virtual ~UnpackTask();
	virtual void Execute(void);
private:
	u64 GetUnpackSize(void);

	ArchiveHandle *archive;
	ArchiveBrowser *browser;
	bool fullUnpack;
};

#endif /* UNPACKTASK_H_ */
