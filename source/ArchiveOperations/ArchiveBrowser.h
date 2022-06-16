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
#ifndef ARCHIVE_BROWSER_H_
#define ARCHIVE_BROWSER_H_

#include <gctypes.h>
#include <vector>
#include <algorithm>
#include "FileOperations/Browser.hpp"
#include "FileOperations/ItemMarker.h"
#include "Archive.h"

#define CLOSE_ARCHIVE	-0x666

class ArchiveBrowser : public Browser
{
	public:
		//!Constructor
		ArchiveBrowser(const char * filepath);
		ArchiveBrowser(ArchiveHandle * archive);
		//!Destructor
		virtual ~ArchiveBrowser();
		//!Execute the file in filepath
		int ExecuteFile(const char * filepath);
		//!Add new items into a destination path
		int AddItem(const ItemStruct * Item, const char * arc_filepath, int compression);
		//!Extract item (folder or file) into a destination path
		int ExtractItem(int ind, const char * dest);
		int ExtractCurrentItem(const char * dest) { return ExtractItem(SelIndex, dest); };
		//!Extract a folder into a destination path
		int ExtractFolder(const char * foldername, const char * dest);
		//!Extract the all archive items into a destination path
		int ExtractAll(const char * dest);
		//!Get the current EntrieCount
		int GetEntrieCount() { return PathStructure.size(); };
		//!Get current pageindex
		int GetPageIndex() { return PageIndex; };
		//!Get current pageindex
		int GetSelIndex() { return SelIndex; };
		//!Set the pageindex
		void SetPageIndex(int ind);
		//!Set the SelectedIndex
		void SetSelectedIndex(int ind);
		//!Is the current ind item a directory
		bool IsDir(int ind);
		//!Get the item archive structure of ind file
		ArchiveFileStruct * GetItemStructure(int ind);
		ArchiveFileStruct * GetCurrentItemStructure() { return GetItemStructure(SelIndex); };
		//!Get the name to display in the explorer
		const char * GetItemName(int ind);
		//!Get the name to display in the explorer
		const char * GetCurrentName() { return GetItemName(SelIndex); };
		//!Get the uncompressed size of the item
		u64 GetFilesize(int ind);
		//!Change the folder
		int ChangeDirectory();
		//!Parse the archive directory into an archive structure
		int ParseArchiveDirectory(const char * ArcPath);
		//!Returns a ItemStruct for the file marker
		ItemStruct * GetItemStruct(int pos);
		//!Get the current archive path
		const char * GetCurrentPath();
		//!Get the full file path to the selected item
		const char * GetCurrentSelectedFilepath();
		//!Get the archive filename
		const char * GetArchiveName() { return OrigArchiveFilename; };
		//!Return one time in directory tree
		int BackInDirectory() { return LeaveCurDir(); };
		//!Refresh current directory
		void Refresh();
		//!Reload complete archive file list
		void ReloadList();
		//!Get the archive handle
		ArchiveHandle *GetArchive(void) const { return archive; }

		//!Clear the current PathStructure and free the memory
		void ClearList();
		//!Sort the current PathStructure
		void SortList();
	private:
		int EnterSelDir();
		int LeaveCurDir();
		void AddListEntrie(const char * filename, size_t length, size_t comp_length, bool isdir, u32 index, u64 modtime, u8 archiveType);
		bool InDirectoryTree(const char * Path, const char * itemFullFilename, bool firstpage);
		static void *ParseThreadCallback(void *arg);

		ArchiveHandle * archive;
		int PageIndex;
		int SelIndex;
		u32 ItemNumber;
		char currentPath[MAXPATHLEN];
		char * displayPath;
		char * OrigArchiveFilename;

		std::vector<ArchiveFileStruct *> PathStructure;

		static bool FileSortCallback(const ArchiveFileStruct * f1, const ArchiveFileStruct * f2);
};

#endif //ARCHIVE_BROWSER_H_
