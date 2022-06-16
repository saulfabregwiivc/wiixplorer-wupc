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
#ifndef _RARCFILE_H_
#define _RARCFILE_H_

#include "WiiArchive.h"

typedef struct
{
  u32 magic; //'RARC'
  u32 size; //size of the file
  u32 unknown;
  u32 dataStartOffset; //where does the actual data start? You have to add 0x20 to this value.
  u32 unknown2[4];

  u32 numNodes;
  u32 unknown3[2];
  u32 fileEntriesOffset;
  u32 unknown4;
  u32 stringTableOffset; //where is the string table stored? You have to add 0x20 to this value.
  u32 unknown5[2];
} RarcHeader;

typedef struct
{
  char type[4];
  u32 filenameOffset; //directory name, offset into string table
  u16 unknown;
  u16 numFileEntries; //how many files belong to this node?
  u32 firstFileEntryOffset;
} RarcNode;

typedef struct
{
  u16 id; //file id. If this is 0xFFFF, then this entry is a subdirectory link
  u16 unknown;
  u16 unknown2;
  u16 filenameOffset; //file/subdir name, offset into string table
  u32 dataOffset; //offset to file data (for subdirs: index of Node representing the subdir)
  u32 dataSize; //size of data
  u32 zero; //seems to be always '0'
} RarcFileEntry;

class RarcFile : public WiiArchive
{
	public:
		//!Constructor
		RarcFile(const char *filepath);
		//!Overload
		RarcFile(const u8 * Buffer, u32 Size);
		//!Destructor
		virtual ~RarcFile();
	protected:
		//!Parse the archive
		bool ParseFile();
		bool ParseRarcHeader();
		void ParseNode(RarcNode * Node, string & parentDirectory);
		//!Get the Filename
		void GetFilename(int offset, string & Filename);

		RarcHeader Header;
		int ItemIndex;
};

#endif
