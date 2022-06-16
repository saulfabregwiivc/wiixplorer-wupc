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
#include "Prompts/ProgressWindow.h"
#include "FileOperations/fileops.h"
#include "WiiArchive.h"

WiiArchive::WiiArchive(const char *filepath)
{
	File = NULL;
	FileBuffer = NULL;
	FileSize = 0;
	FromMem = false;

	LoadFile(filepath);
}

WiiArchive::WiiArchive(const u8 * Buffer, u32 Size)
{
	File = NULL;
	FileBuffer = NULL;
	FileSize = 0;
	FromMem = true;

	if(Buffer)
	{
		LoadFile(Buffer, Size);
	}
}

WiiArchive::~WiiArchive()
{
	CloseFile();
}

void WiiArchive::CloseFile()
{
	ClearList();

	if(FileBuffer)
		free(FileBuffer);

	if(File)
		fclose(File);

	File = NULL;
	FileBuffer = NULL;
	FileSize = 0;
	FromMem = false;
}

bool WiiArchive::LoadFile(const char * filepath)
{
	if(!filepath)
		return false;

	CloseFile();

	File = fopen(filepath, "rb");
	if(!File)
		return false;

	fseek(File, 0, SEEK_END);

	FileSize = ftell(File);
	rewind(File);

	FromMem = false;

	return true;
}

bool WiiArchive::LoadFile(const u8 * Buffer, u32 Size)
{
	if(!Buffer)
		return false;

	CloseFile();

	FileBuffer = (u8 *) malloc(Size);
	if(!FileBuffer)
		return false;

	FileSize = Size;

	FromMem = true;

	memcpy(FileBuffer, Buffer, FileSize);

	return true;
}

ArchiveFileStruct * WiiArchive::GetFileStruct(int ind)
{
	if(ind > (int) PathStructure.size() || ind < 0)
		return NULL;

	return PathStructure.at(ind);
}

void WiiArchive::AddListEntrie(const char * filename, size_t length, size_t comp_length, bool isdir, u32 index, u64 modtime, u8 Type)
{
	if(!filename)
		return;

	ArchiveFileStruct * TempStruct = new ArchiveFileStruct;
	TempStruct->filename = new char[strlen(filename)+1];
	sprintf(TempStruct->filename, "%s", filename);
	TempStruct->length = length;
	TempStruct->comp_length = comp_length;
	TempStruct->isdir = isdir;
	TempStruct->fileindex = index;
	TempStruct->ModTime = modtime;
	TempStruct->archiveType = Type;

	PathStructure.push_back(TempStruct);
	TempStruct = NULL;
}

void WiiArchive::ClearList()
{
	for(u32 i = 0; i < PathStructure.size(); i++)
	{
		if(PathStructure.at(i)->filename != NULL)
		{
			delete [] PathStructure.at(i)->filename;
			PathStructure.at(i)->filename = NULL;
		}
		if(PathStructure.at(i) != NULL)
		{
			delete PathStructure.at(i);
			PathStructure.at(i) = NULL;
		}
	}

	PathStructure.clear();
	BufferOffset.clear();
}

size_t WiiArchive::ReadFile(void * buffer, size_t size, off_t offset)
{
	if(!FileBuffer && !File)
		return -1;

	if(FromMem)
	{
		memcpy(buffer, FileBuffer+offset, size);
		return size;
	}

	fseek(File, offset, SEEK_SET);
	return fread(buffer, 1, size, File);
}

int WiiArchive::ExtractFile(int ind, const char *dest, bool withpath)
{
	ArchiveFileStruct * File = GetFileStruct(ind);
	if(!File)
		return -2;

	char * RealFilename = strrchr(File->filename, '/');
	if(RealFilename)
		RealFilename += 1;
	else
		RealFilename = File->filename;

	char writepath[MAXPATHLEN];
	if(withpath)
		snprintf(writepath, sizeof(writepath), "%s/%s", dest, File->filename);
	else
		snprintf(writepath, sizeof(writepath), "%s/%s", dest, RealFilename);

	if(File->isdir)
	{
		strncat(writepath, "/", sizeof(writepath));
		CreateSubfolder(writepath);
		return 1;
	}

	char * temppath = strdup(writepath);
	char * pointer = strrchr(temppath, '/');
	if(pointer)
	{
		pointer += 1;
		pointer[0] = '\0';
	}

	CreateSubfolder(temppath);

	free(temppath);
	temppath = NULL;

	u32 blocksize = 1024*50;
	u8 * buffer = (u8 *) malloc(1024*50);
	if(!buffer)
	{
		return -3;
	}

	u32 FileOffset = BufferOffset.at(File->fileindex);
	u32 filesize = File->length;

	FILE *pfile = fopen(writepath, "wb");
	if(!pfile)
	{
		free(buffer);
		fclose(pfile);
		return -3;
	}

	u32 done = 0;

	do
	{
		if(ProgressWindow::Instance()->IsCanceled())
		{
			free(buffer);
			fclose(pfile);
			return PROGRESS_CANCELED;
		}

		ShowProgress(done, filesize, RealFilename);

		if(filesize - done < blocksize)
			blocksize = filesize - done;

		int ret = ReadFile(buffer, blocksize, FileOffset+done);
		if(ret < 0)
		{
			fclose(pfile);
			free(buffer);
			return -3;
		}

		ret = fwrite(buffer, 1, ret, pfile);
		if(ret < 0)
		{
			fclose(pfile);
			free(buffer);
			return -3;
		}

		done += ret;
	}
	while(done < filesize);

	fclose(pfile);
	free(buffer);

	// finish up the progress for this file
	FinishProgress(filesize);

	return done;
}

int WiiArchive::ExtractAll(const char * destpath)
{
	for(u32 i = 0; i < PathStructure.size(); i++)
	{
		int ret = ExtractFile(PathStructure.at(i)->fileindex, destpath, true);
		if(ret < 0)
			return ret;
	}

	return 1;
}
