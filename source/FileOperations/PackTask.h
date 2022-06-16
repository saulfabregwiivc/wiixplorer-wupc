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
#ifndef PACKTASK_H_
#define PACKTASK_H_

#include "FileOperations/ProcessTask.h"
#include "ArchiveOperations/Archive.h"

class PackTask : public ProcessTask
{
public:
	PackTask(const ItemMarker *process, const std::string &dest, ArchiveHandle *archive, int compression);
	virtual ~PackTask();
	virtual void Execute(void);
private:
	ArchiveHandle *archive;
	int compression;
};

#endif /* PACKTASK_H_ */
