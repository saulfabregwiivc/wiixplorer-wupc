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
#ifndef ___DIRLISTASYNC_H_
#define ___DIRLISTASYNC_H_

#include "Controls/ThreadedTaskHandler.hpp"
#include "DirList.h"
#include "stdafx.h"

using namespace std;

class DirListAsync : public DirList, public ThreadedTask
{
public:
	//!\param path Path from where to load the filelist of all files
	//!\param filter A fileext that needs to be filtered
	//!\param flags search/filter flags from the enum
	DirListAsync(const char * path, const char *filter = NULL, u32 flags = Files | Dirs, bool sort = true);
	//! asyncronous called function
	void Execute(void);
	//! SLOTS
	sigslot::signal1<DirListAsync *> StartList;
	sigslot::signal1<DirListAsync *> FinishList;
private:
	string m_path;
	string m_filter;
	u32 m_flags;
	bool m_sort;
};

#endif
