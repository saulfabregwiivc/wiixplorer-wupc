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
#include "DirListAsync.h"

DirListAsync::DirListAsync(const char * path, const char *filter, u32 flags, bool sort)
	: m_path(path)
	, m_filter(filter ? filter : "")
	, m_flags(flags)
	, m_sort(sort)
{
}

void DirListAsync::Execute(void)
{
	StartList(this);

	//! load up the list and sort it
	DirList::LoadPath(m_path.c_str(), m_filter.size() ? m_filter.c_str() : NULL, m_flags);

	if(m_sort)
		DirList::SortList();

	FinishList(this);
}
