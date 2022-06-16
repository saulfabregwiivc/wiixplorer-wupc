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
#ifndef ___RARFILE_H_
#define ___RARFILE_H_

#include <vector>
#include <string>

#include <libunrar/rar.hpp>
#include "ArchiveStruct.h"

class RarFile
{
	public:
		//!Constructor
		RarFile(const char *filepath);
		//!Destructor
		~RarFile();
		//!Get the archive file structure
		ArchiveFileStruct * GetFileStruct(int fileIndx);
		//!Extract file from a 7zip to file
		int ExtractFile(int fileindex, const char * outpath, bool withpath = false);
		//!Extract all files from the RAR to a path
		int ExtractAll(const char * destpath);
		//!Get the total amount of items inside the archive
		u32 GetItemCount();

	private:
		bool LoadList();
		void ClearList();
		bool CheckPassword();
		bool SeekFile(int index);
		void UnstoreFile(ComprDataIO &DataIO, int64 DestUnpSize);
		int InternalExtractFile(const char * outpath, bool withpath);
		Archive RarArc;

		std::vector<ArchiveFileStruct *> RarStructure;
		std::string Password;
};

#endif
