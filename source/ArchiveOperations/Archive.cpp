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
#include <stdio.h>
#include <string.h>

#include "Archive.h"

ArchiveHandle::ArchiveHandle(const char  * filepath)
{
	References = 0;
	szFile = NULL;
	zipFile = NULL;
	rarFile = NULL;
	u8File = NULL;
	rarcFile = NULL;

	char checkbuffer[6];
	memset(checkbuffer, 0, sizeof(checkbuffer));

	FILE * file = fopen(filepath, "rb");
	if(!file)
		return;

	int ret = 1;

	while(checkbuffer[0] == 0 && ret > 0)
		ret = fread(&checkbuffer, 1, 1, file);

	fread(&checkbuffer[1], 1, 5, file);
	fclose(file);

	if(IsZipFile(checkbuffer))
		zipFile = new ZipFile(filepath);

	if(Is7ZipFile(checkbuffer))
		szFile = new SzFile(filepath);

	if(IsRarFile(checkbuffer))
		rarFile = new RarFile(filepath);

	if(IsU8ArchiveFile(checkbuffer))
		u8File = new U8Archive(filepath);

	if(IsRarcFile(checkbuffer))
		rarcFile = new RarcFile(filepath);
}

ArchiveHandle::ArchiveHandle(ZipFile * zip)
{
	References = 0;
	szFile = NULL;
	zipFile = zip;
	rarFile = NULL;
	u8File = NULL;
	rarcFile = NULL;
}

ArchiveHandle::~ArchiveHandle()
{
	if(zipFile)
		delete zipFile;

	if(szFile)
		delete szFile;

	if(rarFile)
		delete rarFile;

	if(u8File)
		delete u8File;

	if(rarcFile)
		delete rarcFile;

	zipFile = NULL;
	szFile = NULL;
	rarFile = NULL;
	u8File = NULL;
	rarcFile = NULL;
}

ArchiveFileStruct * ArchiveHandle::GetFileStruct(int ind)
{
	if(zipFile)
		return zipFile->GetFileStruct(ind);

	if(szFile)
		return szFile->GetFileStruct(ind);

	if(rarFile)
		return rarFile->GetFileStruct(ind);

	if(u8File)
		return u8File->GetFileStruct(ind);

	if(rarcFile)
		return rarcFile->GetFileStruct(ind);

	return NULL;
}

u32 ArchiveHandle::GetItemCount()
{
	if(zipFile)
		return zipFile->GetItemCount();

	if(szFile)
		return szFile->GetItemCount();

	if(rarFile)
		return rarFile->GetItemCount();

	if(u8File)
		return u8File->GetItemCount();

	if(rarcFile)
		return rarcFile->GetItemCount();

	return 0;
}

bool ArchiveHandle::ReloadList()
{
	if(zipFile)
		return zipFile->LoadList();

	return false;
}

int ArchiveHandle::AddFile(const char * filepath, const char *destpath, int compression)
{
	if(zipFile)
		return zipFile->AddFile(filepath, destpath, compression, false);

	return -30;
}

int ArchiveHandle::AddDirectory(const char * filepath, const char *destpath, int compression)
{
	if(zipFile)
		return zipFile->AddDirectory(filepath, destpath, compression);

	return -30;
}

int ArchiveHandle::ExtractFile(int ind, const char *destpath, bool withpath)
{
	if(zipFile)
		return zipFile->ExtractFile(ind, destpath, withpath);

	if(szFile)
		return szFile->ExtractFile(ind, destpath, withpath);

	if(rarFile)
		return rarFile->ExtractFile(ind, destpath, withpath);

	if(u8File)
		return u8File->ExtractFile(ind, destpath, withpath);

	if(rarcFile)
		return rarcFile->ExtractFile(ind, destpath, withpath);

	return 0;
}

int ArchiveHandle::ExtractAll(const char * destpath)
{
	if(zipFile)
		return zipFile->ExtractAll(destpath);

	if(szFile)
		return szFile->ExtractAll(destpath);

	if(rarFile)
		return rarFile->ExtractAll(destpath);

	if(u8File)
		return u8File->ExtractAll(destpath);

	if(rarcFile)
		return rarcFile->ExtractAll(destpath);

	return 0;
}

bool ArchiveHandle::IsZipFile (const char *buffer)
{
	unsigned int *check;

	check = (unsigned int *) buffer;

	if (check[0] == 0x504b0304)
		return true;

	return false;
}

bool ArchiveHandle::Is7ZipFile(const char *buffer)
{
	// 7z signature
	int i;
	for(i = 0; i < 6; i++)
		if(buffer[i] != k7zSignature[i])
			return false;

	return true; // 7z archive found
}

bool ArchiveHandle::IsRarFile(const char *buffer)
{
	// Rar signature	Rar!\x1A\a\0
	Byte Signature[6] = {'R', 'a', 'r', 0x21, 0x1a, 0x07};

	int i;
	for(i = 0; i < 6; i++)
		if(buffer[i] != Signature[i])
			return false;

	return true; // RAR archive found
}

bool ArchiveHandle::IsU8ArchiveFile(const char *buffer)
{
	char SignatureIMET[4] = {'I', 'M', 'E', 'T'};
	char SignatureIMD5[4] = {'I', 'M', 'D', '5'};
	char SignatureU8HEAD[4] = {0x55, 0xAA, 0x38, 0x2D};

	int i;
	bool sign_passed = true;

	for(i = 0; i < 4; i++)
		if(buffer[i] != SignatureIMET[i])
			sign_passed = false;

	if(!sign_passed)
	{
		sign_passed = true;
		for(i = 0; i < 4; i++)
			if(buffer[i] != SignatureIMD5[i])
				sign_passed = false;
	}

	if(!sign_passed)
	{
		sign_passed = true;
		for(i = 0; i < 4; i++)
			if(buffer[i] != SignatureU8HEAD[i])
				sign_passed = false;
	}

	return sign_passed;
}

bool ArchiveHandle::IsRarcFile(const char *buffer)
{
	char SignatureYaz0[4] = {'Y', 'a', 'z', '0'};
	char SignatureRARC[4] = {'R', 'A', 'R', 'C'};

	int i;
	bool sign_passed = true;

	for(i = 0; i < 4; i++)
		if(buffer[i] != SignatureYaz0[i])
			sign_passed = false;

	if(!sign_passed)
	{
		sign_passed = true;
		for(i = 0; i < 4; i++)
			if(buffer[i] != SignatureRARC[i])
				sign_passed = false;
	}

	return sign_passed;
}
