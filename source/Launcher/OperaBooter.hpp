/****************************************************************************
 * Copyright (C) 2009-2011 Dimok
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
#ifndef BOOTOPERA_HPP_
#define BOOTOPERA_HPP_

#include <vector>
#include <gccore.h>
#include <mxml.h>

typedef struct
{
	char * name;
	char * addr;
} Link;

class OperaBooter
{
	public:
		OperaBooter(const char * xmlfile);
		~OperaBooter();
		bool Launch(int pos);
		bool DownloadFile(int pos);
		bool SaveXML();
		bool AddLink();
		bool AddLink(const char * name, const char * addr);
		void RemoveLink(int pos);
		const char * GetName(int pos);
		const char * GetLink(int pos);
		int GetCount() { return LinkList.size(); };
	private:
		bool ParseXML();
		void ClearList();
		void Sort();
		static bool SortCallback(const Link & f1, const Link & f2);

		std::vector<Link> LinkList;
		std::string FilePath;
		u8 * xmlbuffer;
		mxml_node_t * xmlfile;
		u64 OperaID;
};

#endif
