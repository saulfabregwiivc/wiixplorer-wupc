/****************************************************************************
 * Copyright (C) 2009
 * by Dimok
 *
 * Original Filebrowser by Tantric for libwiigui
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 * FileBrowser
 *
 * Directory parsing Class
 * for WiiXplorer 2009
 ***************************************************************************/
#ifndef _FILEBROWSER_H_
#define _FILEBROWSER_H_

#include <unistd.h>
#include <gccore.h>
#include <sys/dir.h>
#include "Controls/CThread.h"

#define FILTER_DIRECTORIES  0x01
#define FILTER_FILES		0x02

#define MAXJOLIET 255

#include "Prompts/DeviceMenu.h"
#include "Browser.hpp"

typedef struct
{
	char dir[MAXPATHLEN]; // directory path of browserList
	char rootdir[10]; // directory path of browserList
	int numEntries; // # of entries in browserList
	int selIndex; // currently selected index of browserList
	int pageIndex; // starting index of browserList page display
} BROWSERINFO;

typedef struct
{
	char *filename; // full filename
	u64 length; // file length in 64 bytes for sizes higher than 4GB
	bool isdir; // 0 - file, 1 - directory
} BROWSERENTRY;

class FileBrowser : public Browser, public CThread
{
	public:
		FileBrowser();
		virtual ~FileBrowser();
		int BrowsePath(const char *path);
		int BrowseDevice(int device);
		void SetPageIndex(int ind);
		void SetSelectedIndex(int ind);
		int GetEntrieCount() { return browser.numEntries; };
		int GetPageIndex() { return browser.pageIndex; };
		int GetSelIndex() { return browser.selIndex; };
		bool IsDir(int ind) { if(valid(ind)) return browserList[ind].isdir; else return false; };
		u64 GetFilesize(int ind) { if(valid(ind)) return browserList[ind].length; else return 0; };
		u64 GetCurrentFilesize() { if(valid(browser.selIndex)) return browserList[browser.selIndex].length; else return 0; };
		const char * GetItemName(int ind) { if(valid(ind)) return (browserList[ind].filename ? browserList[ind].filename : ""); else return ""; };
		const char * GetCurrentFilename() { if(valid(browser.selIndex)) return (browserList[browser.selIndex].filename ? browserList[browser.selIndex].filename : ""); else return ""; };
		const char * GetRootDir() { return browser.rootdir; };
		const char * GetDir() { return browser.dir; };
		const char * GetCurrentPath();
		const char * GetCurrentSelectedFilepath();
		ItemStruct * GetItemStruct(int pos);
		int ChangeDirectory();
		int BackInDirectory() { return LeaveCurDir(); };
		void Refresh();
		bool listChanged() { bool c = bChanged; bChanged = false; return c; }
		void SetFilter(u8 filtermode) { Filter = filtermode; };
		int ExecuteFile(const char *filepath);
		void Lock(void) { while(Locked) usleep(100); Locked = true; }
		void Unlock(void) { Locked = false; }
	private:
		bool valid(int ind) { if(ind >= 0 && ind < browser.numEntries) return true; else return false; }
		int ParseDirectory(bool ResetPosition = true);
		int UpdateDirName();
		void ResetBrowser();
		int EnterSelDir();
		int LeaveCurDir();
		void InitParseThread();
		void ShutdownParseThread();
		bool ParseDirEntries();
		void executeThread(void);

		BROWSERINFO browser;
		BROWSERENTRY * browserList;
		u8 Filter;
		bool exit_Requested;
		bool Locked;
		bool directoryChange;
		bool bChanged;
		DIR *dirIter;
		char currentpath[MAXPATHLEN];
};

#endif
