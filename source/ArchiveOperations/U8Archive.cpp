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
#include "U8Archive.h"

U8Archive::U8Archive(const char *filepath)
	: WiiArchive(filepath)
{
	ParseFile();
}

U8Archive::U8Archive(const u8 * Buffer, u32 Size)
	: WiiArchive(Buffer, Size)
{
	ParseFile();
}

U8Archive::~U8Archive()
{
	CloseFile();
}

bool U8Archive::ParseFile()
{
	if(!FileBuffer && !File)
		return false;

	ClearList();

	IMETHeader IMET_Header;
	ReadFile(&IMET_Header, sizeof(IMETHeader), 0);

	IMD5Header IMD5_Header;
	memcpy(&IMD5_Header, &IMET_Header, sizeof(IMD5Header));
	U8Header U8_Header;
	memcpy(&U8_Header, &IMET_Header, sizeof(U8Header));
	u32 U8HeaderOffset = 0;

	//It's opening.bnr
	if (IMET_Header.fcc == 'IMET')
	{
		//Add header.imet as a file
		AddListEntrie("header.imet", sizeof(IMETHeader), sizeof(IMETHeader), false, 0, 0, U8Arch);
		BufferOffset.push_back(0);

		U8HeaderOffset = sizeof(IMETHeader);

		return ParseU8Header(U8HeaderOffset);
	}

	//It's icon.bin/banner.bin/sound.bin
	else if(IMD5_Header.fcc == 'IMD5')
	{
		FileSize = IMD5_Header.filesize;
		u32 LZ77Magic = 0;
		U8HeaderOffset = sizeof(IMD5Header);
		ReadFile(&LZ77Magic, sizeof(u32), U8HeaderOffset);

		if(LZ77Magic == 'LZ77')
		{
			FileSize -= sizeof(IMD5Header);
			u8 * BinBuffer = (u8 *) malloc(FileSize);
			if(!BinBuffer)
				return false;

			ReadFile(BinBuffer, FileSize, U8HeaderOffset);

			u8 * UncBinBuffer = uncompressLZ77(BinBuffer, FileSize, &FileSize);
			if(!UncBinBuffer)
				return false;

			u32 tmpSize = FileSize;
			free(BinBuffer);
			CloseFile();

			FileBuffer = UncBinBuffer;
			FileSize = tmpSize;
			FromMem = true;
			U8HeaderOffset = 0;
		}

		return ParseU8Header(U8HeaderOffset);
	}
	//It's a direct U8Archive...weird but oh well...
	else if(U8_Header.fcc == 0x55AA382D /* U.8- */)
	{
		return ParseU8Header(0);
	}

	//Unknown U8Archive
	CloseFile();

	return false;
}

bool U8Archive::ParseU8Header(u32 U8HeaderOffset)
{
	u32 U8Magic;
	ReadFile(&U8Magic, sizeof(u32), U8HeaderOffset);

	if(U8Magic != 0x55AA382D /* U.8- */)
	{
		CloseFile();
		return false;
	}

	u32 rootNodeOffset = 0;
	ReadFile(&rootNodeOffset, sizeof(u32), U8HeaderOffset+sizeof(u32));

	u32 fstOffset = U8HeaderOffset + rootNodeOffset;
	U8Entry * MainFST = (U8Entry *) malloc(sizeof(U8Entry));
	if(!MainFST)
	{
		CloseFile();
		return false;
	}

	ReadFile(MainFST, sizeof(U8Entry), fstOffset);

	u32 fstNums = MainFST[0].numEntries;
	U8Entry * fst = (U8Entry *) realloc(MainFST, fstNums*sizeof(U8Entry));
	if(!fst)
	{
		free(MainFST);
		CloseFile();
		return false;
	}

	ReadFile(fst, fstNums*sizeof(U8Entry), fstOffset);

	char filename[MAXPATHLEN];
	char directory[MAXPATHLEN];
	strcpy(directory, "");
	u32 dir_stack[100];
	int dir_index = 0;

	for (u32 i = 1; i < fstNums; ++i)
	{
		string RealFilename;
		U8Filename(fst, fstOffset, i, RealFilename);
		snprintf(filename, sizeof(filename), "%s%s", directory, RealFilename.c_str());

		bool isDir = (fst[i].fileType == 0) ? false : true;

		if(isDir)
		{
			dir_stack[++dir_index] = fst[i].numEntries;
			char tmp[MAXPATHLEN];
			snprintf(tmp, sizeof(tmp), "%s/", RealFilename.c_str());
			strncat(directory, tmp, sizeof(directory));
		}

		AddListEntrie(filename, fst[i].fileLength, fst[i].fileLength, isDir, GetItemCount(), 0, U8Arch);
		BufferOffset.push_back(U8HeaderOffset+fst[i].fileOffset);

		while (dir_stack[dir_index] == i+1 && dir_index > 0)
		{
			if(directory[strlen(directory)-1] == '/')
				directory[strlen(directory)-1] = '\0';

			char * ptr = strrchr(directory, '/');
			if(ptr)
			{
				ptr++;
				ptr[0] = '\0';
			}
			else
			{
				directory[0] = '\0';
			}
			dir_index--;
		}
	}

	free(fst);

	return true;
}

void U8Archive::U8Filename(const U8Entry * fst, int fstoffset, int i, string & Filename)
{
	u32 nameoffset = fstoffset+fst[0].numEntries*sizeof(U8Entry)+fst[i].nameOffset;
	int n = -1;
	char Char;

	do
	{
		n++;
		ReadFile(&Char, 1, nameoffset+n);
		Filename.push_back(Char);
	}
	while(Char != 0 && nameoffset+n < FileSize);
}
