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
#ifndef ___ARCHIVE_H_
#define ___ARCHIVE_H_

#include <gctypes.h>

#include "ArchiveStruct.h"
#include "RarFile.h"
#include "7ZipFile.h"
#include "ZipFile.h"
#include "U8Archive.h"
#include "RarcFile.h"

class ArchiveHandle
{
	public:
		//!Constructor
		ArchiveHandle(const char  * filepath);
		ArchiveHandle(ZipFile * zipFile);
		//!Destructor
		~ArchiveHandle();
		//!Get the archive file structure
		ArchiveFileStruct * GetFileStruct(int fileIndx);
		//!Add a new file into a destination path
		int AddFile(const char * filepath, const char *destpath, int compression);
		//!Add a full directory into a destination path
		int AddDirectory(const char * path, const char *destpath, int compression);
		//!Extract a files from a zip file to a path
		int ExtractFile(int ind, const char *destpath, bool withpath = false);
		//!Extract all files from a zip file to a directory
		int ExtractAll(const char *destpath);
		//!Reload archive list
		bool ReloadList();
		//!Get the total amount of items inside the archive
		u32 GetItemCount();
		//!Add reference
		int AddReference() { return (++References); }
		//!Remove reference
		int RemoveReference() { return (--References); }
	private:
		//!Check what kind of archive it is
		bool IsZipFile (const char *buffer);
		bool Is7ZipFile(const char *buffer);
		bool IsRarFile(const char *buffer);
		bool IsU8ArchiveFile(const char *buffer);
		bool IsRarcFile(const char *buffer);

		SzFile * szFile;
		ZipFile * zipFile;
		RarFile * rarFile;
		U8Archive * u8File;
		RarcFile * rarcFile;

		int References;
};

#endif //ARCHIVE_BROWSER_H_
