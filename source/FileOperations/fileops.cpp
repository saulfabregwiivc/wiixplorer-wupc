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
#include <stdlib.h>
#include <string.h>
#include <gccore.h>
#include <sys/dir.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "Controls/Application.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/ProgressWindow.h"
#include "DirList.h"
#include "fileops.h"
#include "filebrowser.h"

using namespace std;

#define BLOCKSIZE			   71680	  //70KB
#define VectorResize(List) if(List.capacity()-List.size() == 0) List.reserve(List.size()+100)

static bool replaceall = false;
static bool replacenone = false;

/****************************************************************************
 * GetReplaceChoice
 *
 * Get the user choice if he wants to replace a file or not
 ***************************************************************************/
static int GetReplaceChoice(const char * filename)
{
	ProgressWindow::Instance()->CloseWindow();

	PromptWindow *window = new PromptWindow(fmt("%s %s", tr("File already exists:"), filename), tr("Do you want to replace this file?"), tr("Yes"), tr("No"), tr("Yes to all"), tr("No to all"));
	window->DimBackground(true);
	Application::Instance()->Append(window);
	Application::Instance()->SetUpdateOnly(window);

	int choice;

	while((choice = window->GetChoice()) == -1)
		usleep(10000);

	window->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	Application::Instance()->PushForDelete(window);

	ProgressWindow::Instance()->OpenWindow();

	if(choice == 3)
	{
		replaceall = true;
		replacenone = false;
		return choice;
	}
	else if(choice == 0)
	{
		replaceall = false;
		replacenone = true;
		return choice;
	}
	else
	{
		replaceall = false;
		return choice;
	}
}

/****************************************************************************
 * ResetReplaceChoice
 *
 * This should be called after the process is done
 ***************************************************************************/
void ResetReplaceChoice()
{
	replaceall = false;
	replacenone = false;
}

/****************************************************************************
 * CheckFile
 *
 * Check if file is existing
 ***************************************************************************/
bool CheckFile(const char * filepath)
{
	if(!filepath)
		return false;

	if(strchr(filepath, '/') == NULL)
		return false;

	int length = strlen(filepath);

	char * dirnoslash = (char *) malloc(length+2);
	if(!dirnoslash) return false;

	strcpy(dirnoslash, filepath);

	while(length > 0 && dirnoslash[length-1] == '/')
		--length;

	dirnoslash[length] = '\0';

	char * notRoot = strchr(dirnoslash, '/');
	if(!notRoot)
		strcat(dirnoslash, "/");

	struct stat filestat;

	int ret = stat(dirnoslash, &filestat);

	free(dirnoslash);

	return (ret == 0);
}

/****************************************************************************
 * FileSize
 *
 * Get filesize in bytes. u64 for files bigger than 4GB
 ***************************************************************************/
u64 FileSize(const char * filepath)
{
  struct stat filestat;

  if (!filepath || stat(filepath, &filestat) != 0)
	return 0;

  return filestat.st_size;
}

/****************************************************************************
 * LoadFileToMem
 *
 * Load up the file into a block of memory
 ***************************************************************************/
int LoadFileToMem(const char *filepath, u8 **inbuffer, u32 *size)
{
	if(!filepath)
		return -1;

	int ret = -1;
	u32 filesize = FileSize(filepath);
	char * filename = strrchr(filepath, '/');
	if(filename)
		filename++;

	*inbuffer = NULL;
	*size = 0;

	FILE *file = fopen(filepath, "rb");
	if (file == NULL)
		return -1;

	u8 *buffer = (u8 *) malloc(filesize);
	if (buffer == NULL)
	{
		fclose(file);
		return -2;
	}

	u32 done = 0;
	u32 blocksize = BLOCKSIZE;

	do
	{
		if(ProgressWindow::Instance()->IsCanceled())
		{
			free(buffer);
			fclose(file);
			return -10;
		}

		if(blocksize > filesize-done)
			blocksize = filesize-done;

		ShowProgress(done, filesize, filename);

		ret = fread(buffer+done, 1, blocksize, file);
		if(ret < 0)
		{
			free(buffer);
			fclose(file);
			return -3;
		}

		done += ret;

	}
	while(ret > 0);

	fclose(file);

	// update at 100% once
	ShowProgress(done, filesize, filename);

	// finish up the progress for this file
	FinishProgress(filesize);

	if (done != filesize)
	{
		free(buffer);
		return -3;
	}

	*inbuffer = buffer;
	*size = filesize;

	return 1;
}

/****************************************************************************
 * LoadFileToMemWithProgress
 *
 * Load up the file into a block of memory, while showing a progress dialog
 ***************************************************************************/
int LoadFileToMemWithProgress(const char *progressText, const char *filepath, u8 **inbuffer, u32 *size)
{
	StartProgress(progressText);
	int ret = LoadFileToMem(filepath, inbuffer, size);
	StopProgress();

	if(ret == -1) {
		ThrowMsg(tr("Error:"), "%s %s", tr("Can not open the file"), filepath);
	}
	else if(ret == -2) {
		ThrowMsg(tr("Error:"), tr("Not enough memory."));
	}
	else if(ret == -3) {
		ThrowMsg(tr("Error:"), tr("Error while reading file."));
	}
	else if(ret == PROGRESS_CANCELED) {
		ThrowMsg(tr("Error:"), tr("Action cancelled."));
	}
	return ret;
}

/****************************************************************************
 * CreateSubfolder
 *
 * Create recursive all subfolders to the given path
 ***************************************************************************/
bool CreateSubfolder(const char * fullpath)
{
	if(!fullpath)
		return false;

	//! make copy of string
	int length = strlen(fullpath);

	char *dirpath = (char *) malloc(length+2);
	if(!dirpath)
		return false;

	strcpy(dirpath, fullpath);

	//! remove unnecessary slashes
	while(length > 0 && dirpath[length-1] == '/')
		--length;

	dirpath[length] = '\0';

	//! if its the root then add one slash
	char * notRoot = strrchr(dirpath, '/');
	if(!notRoot)
		strcat(dirpath, "/");

	int ret;
	//! clear stack when done as this is recursive
	{
		struct stat filestat;
		ret = stat(dirpath, &filestat);
	}

	//! entry found
	if(ret == 0)
	{
		free(dirpath);
		return true;
	}
	//! if its root and stat failed the device doesnt exist
	else if(!notRoot)
	{
		free(dirpath);
		return false;
	}

	//! cut to previous slash and do a recursion
	*notRoot = '\0';

	bool result = false;

	if(CreateSubfolder(dirpath))
	{
		//! the directory inside which we create the new directory exists, so its ok to create
		//! add back the slash for directory creation
		*notRoot = '/';
		//! try creating the directory now
		result = (mkdir(dirpath, 0777) == 0);
	}

	free(dirpath);

	return result;
}

/****************************************************************************
 * CopyFile
 *
 * Copy the file from source filepath to destination filepath
 ***************************************************************************/
int CopyFile(const char * src, const char * dest)
{
	u32 read;
	u32 wrote;

	u64 sizesrc = FileSize(src);

	char * filename = strrchr(src, '/');
	if(filename)
		filename++;
	else
		return -1;

	bool fileexist = CheckFile(dest);

	if(fileexist == true)
	{
		int choice = -1;
		if(!replaceall && !replacenone)
			choice = GetReplaceChoice(filename);

		if(replacenone || choice == 2) {
			ShowProgress(0, sizesrc, filename);
			// finish up the progress for this file
			FinishProgress(sizesrc);
			return 1;
		}
	}

	FILE * source = fopen(src, "rb");

	if(!source)
		return -2;

	u32 blksize = BLOCKSIZE;

	u8 * buffer = (u8 *) memalign(32, blksize);

	if(buffer == NULL){
		//no memory
		fclose(source);
		return -1;
	}

	FILE * destination = fopen(dest, "wb");

	if(destination == NULL)
	{
		free(buffer);
		fclose(source);
		return -3;
	}

	u64 done = 0;

	do
	{
		if(ProgressWindow::Instance()->IsCanceled())
			break;

		if(blksize > sizesrc - done)
			blksize = sizesrc - done;

		read = fread(buffer, 1, blksize, source);
		if(read != blksize)
			break;

		wrote = fwrite(buffer, 1, read, destination);
		if(wrote != read)
			break;

		//Display progress
		ShowProgress(done, sizesrc, filename);

		done += wrote;
	}
	while (read > 0);

	free(buffer);
	fclose(source);
	fclose(destination);

	// finish up the progress for this file
	FinishProgress(sizesrc);

	if(ProgressWindow::Instance()->IsCanceled())
	{
		RemoveFile(dest);
		return PROGRESS_CANCELED;
	}
	else if(sizesrc != done)
	{
		RemoveFile(dest);
		return -4;
	}

	return 1;
}

/****************************************************************************
 * MoveFile
 *
 * Move a file from srcpath to destdir
 ***************************************************************************/
int MoveFile(const char *srcpath, const char *destdir)
{
	if(CompareDevices(srcpath, destdir))
	{
		const char * filename = strrchr(destdir, '/');

		if(CheckFile(destdir))
		{
			int choice = -1;
			if(!replaceall && !replacenone)
				choice = GetReplaceChoice(filename ? filename+1 : destdir);


			if(replacenone || choice == 2)
			{
				//Display progress
				ShowProgress(0, 1,filename);
				FinishProgress(1);
				return 1;
			}

			else if(replaceall || choice == 1)
				RemoveFile(destdir);
		}
		//Display progress
		ShowProgress(0, 1,filename);
		FinishProgress(1);

		if(RenameFile(srcpath, destdir))
			return 1;
	}
	else
	{
		int res = CopyFile(srcpath, destdir);
		if(res < 0)
			return res;

		if(RemoveFile(srcpath))
			return 1;
	}

	return -1;
}

/****************************************************************************
 * RemoveDirectory
 *
 * Remove a directory and its content recursively
 ***************************************************************************/
int RemoveDirectory(const char * dirpath)
{
	if(!dirpath)
		return -1;

	std::string folderpath = dirpath;
	while(folderpath.size() > 0 && folderpath[folderpath.size()-1] == '/')
		folderpath.erase(folderpath.size()-1);

	//! load list not sorted, to remove in correct order
	DirList dir;
	dir.LoadPath(folderpath.c_str(), 0, DirList::Dirs | DirList::Files | DirList::CheckSubfolders);

	int fileCount = dir.GetFilecount();

	//! remove files first
	for(int i = 0; i < fileCount; i++)
	{
		if(!dir.IsDir(i))
			RemoveFile(dir.GetFilepath(i));
	}

	//! now remove all folders
	for(int i = 0; i < fileCount; i++)
	{
		if(dir.IsDir(i))
			RemoveFile(dir.GetFilepath(i));
	}

	return RemoveFile(folderpath.c_str());
}

/****************************************************************************
 * RemoveFile
 *
 * Delete the file from a given filepath
 ***************************************************************************/
bool RemoveFile(const char * filepath)
{
	return (remove(filepath) == 0);
}

/****************************************************************************
 * RenameFile
 *
 * Rename the file from a given srcpath to a given destpath
 ***************************************************************************/
bool RenameFile(const char * srcpath, const char * destpath)
{
	return (rename(srcpath, destpath) == 0);
}

/****************************************************************************
 * GetFolderSize
 *
 * Get recursivly complete foldersize
 ***************************************************************************/
void GetFolderSize(const char * folderpath, u64 &foldersize, u32 &filecount, const bool &bCancel)
{
	struct dirent *dirent = NULL;
	DIR *dir = opendir(folderpath);
	if(dir == NULL)
		return;

	char *filename = (char *) malloc(MAXPATHLEN);
	if(!filename)
	{
		closedir(dir);
		return;
	}

	memset(filename, 0, MAXPATHLEN);

	struct stat * st = (struct stat *) malloc(sizeof(struct stat));
	if(!st)
	{
		free(filename);
		closedir(dir);
		return;
	}

	while ((dirent = readdir(dir)) != 0)
	{
		if(bCancel)
			break;

		snprintf(filename, MAXPATHLEN, "%s/%s", folderpath, dirent->d_name);

		if(stat(filename, st) != 0)
			continue;

		if(st->st_mode & S_IFDIR)
		{
			if(strcmp(dirent->d_name,".") != 0 && strcmp(dirent->d_name,"..") != 0)
			{
				GetFolderSize(filename, foldersize, filecount, bCancel);
			}
		}
		else
		{
			++filecount;
			foldersize += st->st_size;
		}
	}

	closedir(dir);
	free(filename);
	free(st);
}

/****************************************************************************
 * CompareDevices
 *
 * Compare if its the devices are equal
 ***************************************************************************/
bool CompareDevices(const char *src, const char *dest)
{
	if(!src || !dest)
		return false;

	char *device1 = strchr(src, ':');
	if(!device1)
		return false;

	int length = device1-src+1;

	if(strncasecmp(src, dest, length) == 0)
		return true;

	return false;
}
