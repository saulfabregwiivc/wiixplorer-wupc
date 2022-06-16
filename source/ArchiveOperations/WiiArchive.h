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
#ifndef _WIIARCHIVE_H_
#define _WIIARCHIVE_H_

#include <string>
#include "ArchiveStruct.h"

using namespace std;

class WiiArchive
{
	public:
		//!Constructor
		WiiArchive(const char *filepath);
		//!Overload
		WiiArchive(const u8 * Buffer, u32 Size);
		//!Destructor
		virtual ~WiiArchive();
		//!Load the file
		virtual bool LoadFile(const char * filepath);
		//!Load the file from a buffer
		virtual bool LoadFile(const u8 * Buffer, u32 Size);
		//!Get the archive file structure
		virtual ArchiveFileStruct * GetFileStruct(int fileIndx);
		//!Extract a file
		virtual int ExtractFile(int ind, const char *dest, bool withpath = false);
		//!Extract all files
		virtual int ExtractAll(const char *dest);
		//!Get the total amount of items inside the archive
		virtual u32 GetItemCount() { return PathStructure.size(); };
		//!Close File
		virtual void CloseFile();

	protected:
		//!Add archive listname
		void AddListEntrie(const char * filename, size_t length, size_t comp_length, bool isdir, u32 index, u64 modtime, u8 Type);
		//!Clear the list
		void ClearList();
		//!Raw read from the file
		size_t ReadFile(void * buffer, size_t size, off_t offset);

		bool FromMem;
		FILE * File;
		u8 * FileBuffer;
		u32 FileSize;
		vector<ArchiveFileStruct *> PathStructure;
		vector<u32> BufferOffset;
};

#endif
