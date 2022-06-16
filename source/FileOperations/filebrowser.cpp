/****************************************************************************
 * Copyright (C) 2009-2011
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
 ***************************************************************************/
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiiuse/wpad.h>
#include <malloc.h>

#include "filebrowser.h"
#include "FileStartUp/FileStartUp.h"
#include "Prompts/PromptWindows.h"

#define MAX_PARSE_ITEMS	(20)

/****************************************************************************
 * FileBrowser Class to parse directories on the fly
 ***************************************************************************/
FileBrowser::FileBrowser()
	: CThread(70, 65536)
{
	Filter = 0;
	browserList = NULL;
	dirIter = NULL;
	exit_Requested = false;
	directoryChange = false;
	bChanged = false;
	memset(&browser, 0, sizeof(BROWSERINFO));
	browser.selIndex = 0;
	browser.pageIndex = 0;
	Locked = false;

	//!Reset and prepare browser
	ResetBrowser();
	//!Initialize Parsethread for browser
	startThread();
}

/****************************************************************************
 * Destructor
 ***************************************************************************/
FileBrowser::~FileBrowser()
{
	exit_Requested = true;
	shutdownThread();
	closedir(dirIter); // close directory
	for(int i = 0; i < browser.numEntries; ++i)
		delete [] browserList[i].filename;
	if(browserList != NULL)
		free(browserList);
}

/****************************************************************************
 * BrowsePath
 * Displays a list of files on the selected path
 ***************************************************************************/
int FileBrowser::BrowsePath(const char *path)
{
	if(!path)
		return -1;

	int length = strlen(path)+1;

	char *device = strchr(path, ':');
	int position = device-path+2;

	if(!device || position < 0)
		return -1;

	snprintf(browser.rootdir, position+1, "%s", path);

	int n = 0;
	for(int i = position; i < length; i++)
	{
		browser.dir[n] = path[i];
		n++;
	}
	browser.dir[n] = '\0';

	return ParseDirectory(); // Parse root directory
}

/****************************************************************************
 * BrowseDevice
 * Displays a list of files on the selected device
 ***************************************************************************/
int FileBrowser::BrowseDevice(int device)
{
	if(device < 0 || device >= MAXDEVICES)
		return -1;

	strcpy(browser.dir, "");
	sprintf(browser.rootdir, "%s:/", DeviceName[device]);

	return ParseDirectory(); // Parse root directory
}

/****************************************************************************
 * Enter the current selected directory
 ***************************************************************************/
int FileBrowser::EnterSelDir()
{
	if(!browserList[browser.selIndex].filename)
		return -1;

	int dirlength = strlen(browser.dir);
	int filelength = strlen(browserList[browser.selIndex].filename);
	if((dirlength+filelength+1) > MAXPATHLEN)
		return -1;

	if(dirlength == 0)
		sprintf(browser.dir, "%s", browserList[browser.selIndex].filename);
	else
		sprintf(browser.dir, "%s/%s", browser.dir, browserList[browser.selIndex].filename);

	return 1;
}

/****************************************************************************
 * Leave the current directory
 ***************************************************************************/
int FileBrowser::LeaveCurDir()
{
	char * ptr = strrchr(browser.dir, '/');
	if(ptr)
	{
		ptr[0] = '\0';
		return 1;
	}
	else
	{
		if(strlen(browser.dir) != 0)
		{
			strcpy(browser.dir, "");
			return 1;
		}
		else
		{
			//There is no upper directory
			return 1;
		}
	}
}

/****************************************************************************
 * UpdateDirName()
 * Update curent directory name for file browser
 ***************************************************************************/
int FileBrowser::UpdateDirName()
{
	if(!browserList[browser.selIndex].filename)
		return -1;

	if(strcmp(browserList[browser.selIndex].filename, "..") == 0)
	{
		return LeaveCurDir();
	}

	return EnterSelDir();
}

/****************************************************************************
 * SetPageIndex
 * not inline for later mutex purpose
 ***************************************************************************/
void FileBrowser::SetPageIndex(int ind)
{
	browser.pageIndex = ind;
}
/****************************************************************************
 * SetSelectedIndex
 * not inline for later mutex purpose
 ***************************************************************************/
void FileBrowser::SetSelectedIndex(int ind)
{
	browser.selIndex = ind;
}
/****************************************************************************
 * Get the current full path
 ***************************************************************************/
const char * FileBrowser::GetCurrentPath()
{
	snprintf(currentpath, sizeof(currentpath), "%s%s", browser.rootdir, browser.dir);

	return currentpath;
}
/****************************************************************************
 * Get the current full path with filename
 ***************************************************************************/
const char * FileBrowser::GetCurrentSelectedFilepath()
{
	if(!browserList[browser.selIndex].filename)
		return "";

	if(strcmp(browser.dir, "") != 0)
		snprintf(currentpath, sizeof(currentpath), "%s%s/%s", browser.rootdir, browser.dir, browserList[browser.selIndex].filename);
	else
		snprintf(currentpath, sizeof(currentpath), "%s%s%s", browser.rootdir, browser.dir, browserList[browser.selIndex].filename);

	return currentpath;
}

/****************************************************************************
 * Get the current item structure
 ***************************************************************************/
ItemStruct * FileBrowser::GetItemStruct(int pos)
{
	if(!valid(pos)  || !browserList[pos].filename)
		return NULL;

	ItemStruct * Item = new ItemStruct;
	memset(Item, 0, sizeof(ItemStruct));

	Item->itempath = (char *) malloc(strlen(browser.rootdir)+strlen(browser.dir)+strlen(browserList[pos].filename)+2);
	if(Item->itempath)
		sprintf(Item->itempath, "%s%s/%s", browser.rootdir, browser.dir, browserList[pos].filename);
	Item->itemsize = browserList[pos].length;
	Item->isdir = browserList[pos].isdir;
	Item->itemindex = pos;

	return Item;
}

/****************************************************************************
 * ResetBrowser()
 * Clears the file browser memory, and allocates one initial entry
 ***************************************************************************/
void FileBrowser::ResetBrowser()
{
	Lock();
	for(int i = 0; i < browser.numEntries; ++i)
		delete [] browserList[i].filename;

	// Clear any existing values
	if(browserList != NULL)
	{
		free(browserList);
		browserList = NULL;
	}

	browser.numEntries = 0;

	// set aside space for 1 entry
	browserList = (BROWSERENTRY *)malloc(sizeof(BROWSERENTRY));
	memset(browserList, 0, sizeof(BROWSERENTRY));
	Unlock();
}

/****************************************************************************
 * FileSortCallback
 *
 * Quick sort callback to sort file entries with the following order:
 *   .
 *   ..
 *   <dirs>
 *   <files>
 ***************************************************************************/
static int FileSortCallback(const void *file1, const void *file2)
{
	const BROWSERENTRY *f1 = (const BROWSERENTRY *) file1;
	const BROWSERENTRY *f2 = (const BROWSERENTRY *) file2;

	if(!f1 || !f1->filename)  { return 1; }
	if(!f2 || !f2->filename)  { return -1; }

	/* Special case for implicit directories */
	if(f1->filename[0] == '.' || f2->filename[0] == '.')
	{
		if(strcmp(f1->filename, ".") == 0) { return -1; }
		if(strcmp(f2->filename, ".") == 0) { return 1; }
		if(strcmp(f1->filename, "..") == 0) { return -1; }
		if(strcmp(f2->filename, "..") == 0) { return 1; }
	}

	/* If one is a file and one is a directory the directory is first. */
	if(f1->isdir && !(f2->isdir)) return -1;
	if(!(f1->isdir) && (f2->isdir)) return 1;

	return stricmp(f1->filename, f2->filename);
}

/****************************************************************************
 * ParseDirEntries
 *
 * Update current directory and set new entry list and entrynum
 ***************************************************************************/
bool FileBrowser::ParseDirEntries()
{
	if(!dirIter)
		return false;

	char filename[MAXPATHLEN];
	struct stat filestat;

	struct dirent *dirent = 0;
	int nameLength, fileCount = 0;

	BROWSERENTRY *tmpBrowser = (BROWSERENTRY *) malloc(MAX_PARSE_ITEMS * sizeof(BROWSERENTRY));
	if(!tmpBrowser) // failed to allocate required memory
	{
		ResetBrowser();
		ThrowMsg(tr("Out of memory: too many files!"), 0 , tr("OK"));
		return false;
	}

	for(int i = 0; i < MAX_PARSE_ITEMS; i++)
	{
		if(directoryChange)
			break;

		dirent = readdir(dirIter);
		if(dirent == 0)
			break;

		if(!dirent->d_name)
			continue;

		if(strcmp(dirent->d_name,".") == 0)
			continue;

		if(Settings.HideSystemFiles)
		{
			if(dirent->d_name[0] == '.' && strcmp(dirent->d_name,"..") != 0)
				continue;

			else if(dirent->d_name[0] == '$')
				continue;

			else if(strcasecmp(dirent->d_name,"thumb.db") == 0)
				continue;
		}

		snprintf(filename, MAXPATHLEN, "%s%s/%s", browser.rootdir, browser.dir, dirent->d_name);

		if(strcmp(dirent->d_name,"..") == 0) {
			filestat.st_mode = S_IFDIR;
			filestat.st_size = 0;
		}
		else if(stat(filename, &filestat) != 0) {
			continue;
		}

		if((Filter & FILTER_DIRECTORIES) && (filestat.st_mode & S_IFDIR))
			continue;

		if((Filter & FILTER_FILES) && !(filestat.st_mode & S_IFDIR))
			continue;

		nameLength = strlen(dirent->d_name)+1;
		if(nameLength > 255)
			nameLength = 255;

		tmpBrowser[fileCount].filename = new (std::nothrow) char[nameLength];
		if(tmpBrowser[fileCount].filename) {
			strncpy(tmpBrowser[fileCount].filename, dirent->d_name, nameLength);
			tmpBrowser[fileCount].filename[nameLength-1] = 0;
		}

		tmpBrowser[fileCount].length = filestat.st_size;
		tmpBrowser[fileCount].isdir = (filestat.st_mode & S_IFDIR) != 0; // flag this as a dir

		fileCount++;
	}

	// Sort the file list
	if(!directoryChange && fileCount > 0)
	{
		Lock();
		BROWSERENTRY * newBrowserList = (BROWSERENTRY *)realloc(browserList, (browser.numEntries+fileCount+1) * sizeof(BROWSERENTRY));
		if(!newBrowserList) // failed to allocate required memory
		{
			for(int i = 0; i < fileCount; ++i)
				delete [] tmpBrowser[i].filename;
			ThrowMsg(tr("Out of memory: too many files!"), 0 , tr("OK"));
		}
		else
		{
			browserList = newBrowserList;
			memcpy(&browserList[browser.numEntries], tmpBrowser, fileCount * sizeof(BROWSERENTRY));
			qsort(browserList, browser.numEntries+fileCount, sizeof(BROWSERENTRY), FileSortCallback);
			browser.numEntries += fileCount;	//make sure reload is after the sort
		}
		Unlock();
		bChanged = true;
	}
	else {
		for(int i = 0; i < fileCount; ++i)
			delete [] tmpBrowser[i].filename;
	}

	free(tmpBrowser);

	if(!dirent || directoryChange)
	{
		closedir(dirIter); // close directory
		dirIter = NULL;
		return false; // no more entries
	}
	return true; // more entries
}

/***************************************************************************
 * Browse subdirectories
 **************************************************************************/
void FileBrowser::Refresh()
{
	ParseDirectory(false);
}

/***************************************************************************
 * Browse subdirectories
 **************************************************************************/
int FileBrowser::ParseDirectory(bool ResetPosition)
{
	if(ResetPosition)
	{
		browser.pageIndex = 0;
		browser.selIndex = 0;
	}

	char fulldir[MAXPATHLEN];
	struct stat st;

	// Check if directory exists
	snprintf(fulldir, sizeof(fulldir), "%s%s", browser.rootdir, browser.dir); // add device to path
	if((stat(fulldir, &st) != 0) || !(st.st_mode & S_IFDIR)) {
		// if we can't open the dir, try opening the root dir
		strcpy(browser.dir, "");
		snprintf(fulldir, sizeof(fulldir), "%s%s", browser.rootdir, browser.dir);
		if((stat(fulldir, &st) != 0) || !(st.st_mode & S_IFDIR))
			return -1;
	}

	directoryChange = true;
	resumeThread();

	return 1;
}

/****************************************************************************
 * ExecuteFile
 ***************************************************************************/
int FileBrowser::ExecuteFile(const char *filepath)
{
	if(!filepath)
		return -1;

	return FileStartUp(filepath);
}

/****************************************************************************
 * ChangeDirectory
 *
 * Update current directory and set new entry list if directory has changed
 ***************************************************************************/
int FileBrowser::ChangeDirectory()
{
	if(!UpdateDirName())
		return -1;

	return ParseDirectory();
}

/****************************************************************************
 * Threaded function
 ***************************************************************************/
void FileBrowser::executeThread(void)
{
	while(!exit_Requested)
	{
		if(directoryChange)
		{
			directoryChange = false;
			// reset browser
			ResetBrowser();
			if(dirIter) {
				closedir(dirIter);
				dirIter = NULL;
			}

			char fulldir[MAXPATHLEN];
			// open the directory
			snprintf(fulldir, MAXPATHLEN, "%s%s", browser.rootdir, browser.dir); // add device to path
			dirIter = opendir(fulldir);
			if(dirIter == NULL) {
				// if we can't open the dir, try opening the root dir
				strcpy(browser.dir, "");
				snprintf(fulldir, MAXPATHLEN, "%s%s", browser.rootdir, browser.dir);
				dirIter = opendir(fulldir);
			}
		}

		if(!exit_Requested && !ParseDirEntries() && !directoryChange)
			suspendThread();

		usleep(100);
	}
}
