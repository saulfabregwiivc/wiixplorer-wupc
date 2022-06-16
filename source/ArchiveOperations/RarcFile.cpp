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
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "Prompts/PromptWindows.h"
#include "FileOperations/fileops.h"
#include "Tools/uncompress.h"
#include "RarcFile.h"

RarcFile::RarcFile(const char *filepath)
	: WiiArchive(filepath)
{
	ParseFile();
}

RarcFile::RarcFile(const u8 * Buffer, u32 Size)
	: WiiArchive(Buffer, Size)
{
	ParseFile();
}

RarcFile::~RarcFile()
{
	CloseFile();
}

bool RarcFile::ParseFile()
{
	if(!FileBuffer && !File)
		return false;

	ClearList();

	ReadFile(&Header, sizeof(RarcHeader), 0);

	Yaz0_Header Yaz0_Head;
	memcpy(&Yaz0_Head, &Header, sizeof(Yaz0_Header));

	if(Yaz0_Head.magic == 'Yaz0')
	{
		if(!FileBuffer)
		{
			FileBuffer = (u8 *) malloc(FileSize);
			if(!FileBuffer)
			{
				CloseFile();
				return false;
			}
			ReadFile(FileBuffer, FileSize, 0);
			FromMem = true;
		}

		FileSize = Yaz0_Head.decompressed_size;

		u8 * buff = (u8 *) malloc(FileSize);
		if(!buff)
		{
			CloseFile();
			return false;
		}

		uncompressYaz0(FileBuffer, buff, FileSize);

		CloseFile();

		FromMem = true;
		FileBuffer = buff;

		return ParseRarcHeader();
	}
	else if(Header.magic == 'RARC')
	{
		return ParseRarcHeader();
	}

	//Unknown RarcFile
	CloseFile();

	return false;
}

bool RarcFile::ParseRarcHeader()
{
	ReadFile(&Header, sizeof(RarcHeader), 0);

	if(Header.magic != 'RARC')
	{
		CloseFile();
		return false;
	}

	FileSize = Header.size;

	RarcNode RootNode;

	ReadFile(&RootNode, sizeof(RarcNode), sizeof(RarcHeader));

	ItemIndex = 0;

	string ItemPath;
	ParseNode(&RootNode, ItemPath);

	return true;
}

void RarcFile::ParseNode(RarcNode * Node, string & parentDirectory)
{
	u32 StringOffset = Header.stringTableOffset+0x20;
	u32 DataOffset = Header.dataStartOffset+0x20;
	u32 CurrOffset = Header.fileEntriesOffset+0x20+Node->firstFileEntryOffset*sizeof(RarcFileEntry);

	//I love recursion... :P
	string parent_dir = parentDirectory;
	string ItemName;
	GetFilename(StringOffset+Node->filenameOffset, ItemName);

	if(parent_dir.size() == 0)
	{
		parent_dir = ItemName;
	}
	else
	{
		//It's just awesome...
		parent_dir.assign(fmt("%s/%s", parentDirectory.c_str(), ItemName.c_str()));
	}

	ItemName.clear();

	AddListEntrie(parent_dir.c_str(), 0, 0, true, ItemIndex++, 0, ArcArch);
	BufferOffset.push_back(0);

	for(u16 i = 0; i < Node->numFileEntries; i++)
	{
		RarcFileEntry FileEntry;
		ReadFile(&FileEntry, sizeof(RarcFileEntry), CurrOffset);

		GetFilename(StringOffset+FileEntry.filenameOffset, ItemName);

		u32 filelength = FileEntry.dataSize;

		/* It's a dir... */
		if(FileEntry.id == 0xFFFF)
		{
			if(strcmp(ItemName.c_str(), ".") != 0 && strcmp(ItemName.c_str(), "..") != 0)
			{
				RarcNode DirNode;
				ReadFile(&DirNode, sizeof(RarcNode), sizeof(RarcHeader)+sizeof(RarcNode)*FileEntry.dataOffset);
				ParseNode(&DirNode, parent_dir);
			}
		}
		/* It's a file... */
		else
		{
			AddListEntrie(fmt("%s/%s", parent_dir.c_str(), ItemName.c_str()), filelength, filelength, false, ItemIndex++, 0, ArcArch);
			BufferOffset.push_back(DataOffset+FileEntry.dataOffset);
		}

		ItemName.clear();
		CurrOffset += sizeof(RarcFileEntry);
	}
}

void RarcFile::GetFilename(int offset, string & Filename)
{
	int n = -1;
	char Char = 0;

	do
	{
		n++;
		ReadFile(&Char, 1, offset+n);
		Filename.push_back(Char);
	}
	while((Char != 0) && (offset+n < (int) FileSize));
}
