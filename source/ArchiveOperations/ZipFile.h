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
#ifndef _ZIPFILE_H_
#define _ZIPFILE_H_

#include <vector>
#include <string>
#include <zip/zip.h>
#include <zip/unzip.h>
#include "ArchiveStruct.h"

class ZipFile
{
	public:
		//!Constructor
		ZipFile(const char *filepath, short mode = ZipFile::OPEN);
		//!Destructor
		~ZipFile();
		//!Get the archive file structure
		ArchiveFileStruct * GetFileStruct(int fileIndx);
		//!Add a file to the zip file
		int AddFile(const char *filepath, const char *destfilepath, int compresslevel = Z_DEFAULT_COMPRESSION, bool RefreshList = true);
		//!Add a directory to the zip file with all of it's content
		int AddDirectory(const char *dirpath, const char *destfilepath, int compresslevel = Z_DEFAULT_COMPRESSION);
		//!Extract a files from a zip file to a path
		int ExtractFile(int ind, const char *dest, bool withpath = false);
		//!Extract all files from a zip file to a directory
		int ExtractAll(const char *dest);
		//!Get the total amount of items inside the archive
		u32 GetItemCount() { return ZipStructure.size(); };
		//!Load/Reload of the full item list in the zip
		bool LoadList();
		//!Enum for opening modes
		enum
		{
			CREATE = 0,
			OPEN,
			APPEND,
		};
	private:
		bool SeekFile(int ind);
		void PathControl();
		void CheckMissingPath(const char * path);
		void ClearList();
		bool SwitchMode(short mode);

		zipFile zFile;
		unzFile uzFile;
		short OpenMode;
		int RealArchiveItemCount;
		std::string ZipFilePath;
		std::vector<ArchiveFileStruct *> ZipStructure;
};

#endif
