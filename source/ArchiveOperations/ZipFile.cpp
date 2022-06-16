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
#include <malloc.h>
#include <sys/dirent.h>

#include "ZipFile.h"
#include "Prompts/ProgressWindow.h"
#include "FileOperations/fileops.h"
#include "FileOperations/DirList.h"

ZipFile::ZipFile(const char *filepath, short mode)
{
	if(!filepath)
		return;

	ZipFilePath = filepath;
	OpenMode = mode;
	zFile = 0;
	uzFile = 0;

	if(OpenMode == ZipFile::OPEN || OpenMode == ZipFile::APPEND)
	{
		uzFile = unzOpen(ZipFilePath.c_str());
		if(uzFile)
			this->LoadList();
	}
}

ZipFile::~ZipFile()
{
	ClearList();
	if(uzFile)
		unzClose(uzFile);
	if(zFile)
		zipClose(zFile, NULL);
}

bool ZipFile::SwitchMode(short mode)
{
	if(mode == ZipFile::OPEN)
	{
		if(zFile)
		{
			zipClose(zFile, NULL);
			zFile = 0;
		}

		if(!uzFile)
			uzFile = unzOpen(ZipFilePath.c_str());

		return (uzFile != 0);
	}
	else if(mode == ZipFile::CREATE || mode == ZipFile::APPEND)
	{
		if(uzFile)
		{
			unzClose(uzFile);
			uzFile = 0;
		}

		if(!zFile)
			zFile = zipOpen(ZipFilePath.c_str(), mode);

		return (zFile != 0);
	}

	return false;
}

void ZipFile::ClearList()
{
	for(u32 i = 0; i < ZipStructure.size(); i++)
	{
		if(ZipStructure[i]->filename)
			delete [] ZipStructure[i]->filename;

		if(ZipStructure[i])
			delete ZipStructure[i];
	}

	ZipStructure.clear();
	std::vector<ArchiveFileStruct *>().swap(ZipStructure);
}

bool ZipFile::LoadList()
{
	ClearList();

	if(!SwitchMode(ZipFile::OPEN))
		return false;

	int ret = unzGoToFirstFile(uzFile);
	if(ret != UNZ_OK)
		return false;

	char filename[1024];
	unz_file_info cur_file_info;
	RealArchiveItemCount = 0;

	do
	{
		if(unzGetCurrentFileInfo(uzFile, &cur_file_info, filename, sizeof(filename), NULL, 0, NULL, 0) == UNZ_OK)
		{
			bool isDir = false;
			if(filename[strlen(filename)-1] == '/')
			{
				isDir = true;
				filename[strlen(filename)-1] = '\0';
			}

			int strlength = strlen(filename)+1;

			ArchiveFileStruct * CurArcFile = new ArchiveFileStruct;
			CurArcFile->filename = new char[strlength];
			strcpy(CurArcFile->filename, filename);
			CurArcFile->length = cur_file_info.uncompressed_size;
			CurArcFile->comp_length = cur_file_info.compressed_size;
			CurArcFile->isdir = isDir;
			CurArcFile->fileindex = RealArchiveItemCount;
			CurArcFile->ModTime = (u64) cur_file_info.dosDate;
			CurArcFile->archiveType = ZIP;

			ZipStructure.push_back(CurArcFile);
		}
		++RealArchiveItemCount;
	}
	while(unzGoToNextFile(uzFile) == UNZ_OK);

	PathControl();

	return true;
}

ArchiveFileStruct * ZipFile::GetFileStruct(int ind)
{
	if(ind < 0 || ind >= (int) ZipStructure.size())
		return NULL;

	return ZipStructure[ind];
}

bool ZipFile::SeekFile(int ind)
{
	if(ind < 0 || ind >= (int) ZipStructure.size())
		return false;

	if(!SwitchMode(ZipFile::OPEN))
		return false;

	int ret = unzGoToFirstFile(uzFile);
	if(ret != UNZ_OK)
		return false;

	while(ind > 0)
	{
		if(unzGoToNextFile(uzFile) != UNZ_OK)
			return false;

		--ind;
	}

	return true;
}

void ZipFile::CheckMissingPath(const char * path)
{
	if(!path)
		return;

	u32 i = 0;
	for(i = 0; i < ZipStructure.size(); i++)
	{
		if(strcasecmp(ZipStructure[i]->filename, path) == 0)
			break;
	}

	if(i == ZipStructure.size())
	{
		int strlength = strlen(path)+1;
		ArchiveFileStruct * CurArcFile = new ArchiveFileStruct;
		CurArcFile->filename = new char[strlength];
		strcpy(CurArcFile->filename, path);
		CurArcFile->length = 0;
		CurArcFile->comp_length = 0;
		CurArcFile->isdir = true;
		CurArcFile->fileindex = ZipStructure.size();
		CurArcFile->ModTime = 0;
		CurArcFile->archiveType = ZIP;

		ZipStructure.push_back(CurArcFile);
	}
}

void ZipFile::PathControl()
{
	char missingpath[1024];

	for(u32 n = 0; n < ZipStructure.size(); n++)
	{
		const char * filepath = ZipStructure[n]->filename;
		int strlength = strlen(filepath);

		for(int i = 0; i < strlength; i++)
		{
			if(filepath[i] == '/')
				CheckMissingPath(missingpath);

			missingpath[i] = filepath[i];
			missingpath[i+1] = '\0';
		}
	}
}

int ZipFile::AddFile(const char *filepath, const char *destfilepath, int compresslevel, bool RefreshList)
{
	if(!destfilepath)
		return -1;

	if(OpenMode == ZipFile::OPEN)
	{
		if(!SwitchMode(ZipFile::APPEND))
			return -2;
	}
	else if(!SwitchMode(OpenMode))
		return -3;

	zip_fileinfo file_info;
	memset(&file_info, 0, sizeof(zip_fileinfo));

	if(destfilepath[strlen(destfilepath)-1] == '/')
	{
		int ret = zipOpenNewFileInZip(zFile, destfilepath, &file_info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, compresslevel);
		if(ret != ZIP_OK)
			return -4;

		zipCloseFileInZip(zFile);
		return 1;
	}

	if(!filepath)
		return -5;


	struct stat filestat;
	memset(&filestat, 0, sizeof(struct stat));
	stat(filepath, &filestat);

	u64 filesize = filestat.st_size;

	//! Set up the modified time
	struct tm * ModTime = localtime(&filestat.st_mtime);
	file_info.tmz_date.tm_sec = ModTime->tm_sec;
	file_info.tmz_date.tm_min = ModTime->tm_min;
	file_info.tmz_date.tm_hour = ModTime->tm_hour;
	file_info.tmz_date.tm_mday = ModTime->tm_mday;
	file_info.tmz_date.tm_mon = ModTime->tm_mon;
	file_info.tmz_date.tm_year = ModTime->tm_year;

	FILE * sourceFile = fopen(filepath, "rb");
	if(!sourceFile)
		return -6;

	u32 blocksize = 1024*70;
	u8 * buffer = (u8 *) malloc(blocksize);
	if(!buffer)
	{
		fclose(sourceFile);
		return -7;
	}

	int ret = zipOpenNewFileInZip(zFile, destfilepath, &file_info, NULL, 0, NULL, 0, NULL, Z_DEFLATED, compresslevel);
	if(ret != ZIP_OK)
	{
		free(buffer);
		fclose(sourceFile);
		return -8;
	}

	const char * RealFilename = strrchr(destfilepath, '/');
	if(RealFilename)
		RealFilename += 1;
	else
		RealFilename = destfilepath;

	int res = 0;
	u64 done = 0;

	while(done < filesize)
	{
		if(ProgressWindow::Instance()->IsCanceled())
		{
			free(buffer);
			fclose(sourceFile);
			zipCloseFileInZip(zFile);
			return PROGRESS_CANCELED;
		}

		ShowProgress(done, filesize, RealFilename);

		if(filesize - done < blocksize)
			blocksize = filesize - done;

		ret = fread(buffer, 1, blocksize, sourceFile);
		if(ret <= 0)
			break; //done

		res = zipWriteInFileInZip(zFile, buffer, ret);
		if(res != ZIP_OK)
			break;

		done += ret;
	}

	free(buffer);
	fclose(sourceFile);
	zipCloseFileInZip(zFile);

	// finish up the progress for this file
	FinishProgress(filesize);

	if(RefreshList)
		LoadList();

	//! File is now created the next files need to be appended
	OpenMode = ZipFile::APPEND;

	return (done == filesize) ? 1 : -12;
}


int ZipFile::AddDirectory(const char *dirpath, const char *destfilepath, int compresslevel)
{
	if(!dirpath || !destfilepath)
		return -1;

	int ret = 1;

	DirList dir(dirpath, 0, DirList::Files | DirList::Dirs | DirList::CheckSubfolders);

	for(int i = 0; i < dir.GetFilecount(); ++i)
	{
		if(ProgressWindow::Instance()->IsCanceled())
			return PROGRESS_CANCELED;

		if(dir.IsDir(i))
		{
			std::string newdestpath(destfilepath);
			if(destfilepath[strlen(destfilepath)-1] != '/')
				newdestpath += '/';
			newdestpath += dir.GetFilename(i);

			ret = AddDirectory(dir.GetFilepath(i), newdestpath.c_str(), compresslevel);
		}
		else
		{
			std::string newdestpath(destfilepath);
			if(destfilepath[strlen(destfilepath)-1] != '/')
				newdestpath += '/';
			newdestpath += dir.GetFilename(i);

			ret = AddFile(dir.GetFilepath(i), newdestpath.c_str(), compresslevel, false);
			if(ret < 0)
				break;
		}
	}

	return ret;
}

int ZipFile::ExtractFile(int ind, const char *dest, bool withpath)
{
	if(!SwitchMode(OPEN))
		return -1;

	if(!SeekFile(ind) && ind < RealArchiveItemCount)
		return -1;

	ArchiveFileStruct * CurArcFile = GetFileStruct(ind);

	u32 done = 0;

	char * RealFilename = strrchr(CurArcFile->filename, '/');
	if(RealFilename)
		RealFilename += 1;
	else
		RealFilename = CurArcFile->filename;

	char writepath[1024];
	if(withpath)
		snprintf(writepath, sizeof(writepath), "%s/%s", dest, CurArcFile->filename);
	else
		snprintf(writepath, sizeof(writepath), "%s/%s", dest, RealFilename);

	u32 filesize = CurArcFile->length;

	if(CurArcFile->isdir)
	{
		strncat(writepath, "/", sizeof(writepath));
		CreateSubfolder(writepath);
		return 1;
	}

	int ret = unzOpenCurrentFile(uzFile);

	if(ret != UNZ_OK)
		return -2;

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
	void *buffer = malloc(blocksize);

	FILE *pfile = fopen(writepath, "wb");
	if(!pfile)
	{
		unzCloseCurrentFile(uzFile);
		free(buffer);
		return -3;
	}

	do
	{
		if(ProgressWindow::Instance()->IsCanceled())
		{
			free(buffer);
			fclose(pfile);
			unzCloseCurrentFile(uzFile);
			return PROGRESS_CANCELED;
		}

		ShowProgress(done, filesize, RealFilename);

		if(filesize - done < blocksize)
			blocksize = filesize - done;

		ret = unzReadCurrentFile(uzFile, buffer, blocksize);
		if(ret < 0)
		{
			free(buffer);
			fclose(pfile);
			unzCloseCurrentFile(uzFile);
			return -4;
		}

		fwrite(buffer, 1, blocksize, pfile);

		done += ret;

	} while(done < filesize);

	fclose(pfile);
	unzCloseCurrentFile(uzFile);

	free(buffer);

	// finish up the progress for this file
	FinishProgress(filesize);

	return 1;
}

int ZipFile::ExtractAll(const char *dest)
{
	if(!SwitchMode(OPEN))
		return -1;

	bool Stop = false;

	u32 blocksize = 1024*70;
	void *buffer = malloc(blocksize);

	if(!buffer)
		return -5;

	char writepath[1024];
	char filename[1024];
	memset(writepath, 0, sizeof(writepath));
	memset(filename, 0, sizeof(filename));

	unz_file_info cur_file_info;

	int ret = unzGoToFirstFile(uzFile);
	if(ret != UNZ_OK)
	{
		free(buffer);
		return -6;
	}

	while(!Stop)
	{
		if(unzGetCurrentFileInfo(uzFile, &cur_file_info, filename, sizeof(filename), NULL, 0, NULL, 0) != UNZ_OK)
		{
			Stop = true;
		}

		if(!Stop && filename[strlen(filename)-1] != '/')
		{
			u64 uncompressed_size = cur_file_info.uncompressed_size;

			u64 done = 0;
			char *pointer = NULL;

			ret = unzOpenCurrentFile(uzFile);

			snprintf(writepath, sizeof(writepath), "%s/%s", dest, filename);

			pointer = strrchr(writepath, '/');
			int position = pointer-writepath+2;

			char temppath[strlen(writepath)];
			snprintf(temppath, position, "%s", writepath);

			CreateSubfolder(temppath);

			if(ret == UNZ_OK)
			{
				FILE *pfile = fopen(writepath, "wb");
				if(!pfile)
				{
					free(buffer);
					unzCloseCurrentFile(uzFile);
					return -8;
				}

				do
				{
					if(ProgressWindow::Instance()->IsCanceled())
					{
						free(buffer);
						fclose(pfile);
						unzCloseCurrentFile(uzFile);
						return PROGRESS_CANCELED;
					}

					ShowProgress(done, uncompressed_size, pointer+1);

					if(uncompressed_size - done < blocksize)
						blocksize = uncompressed_size - done;

					ret = unzReadCurrentFile(uzFile, buffer, blocksize);

					if(ret == 0)
						break;

					fwrite(buffer, 1, blocksize, pfile);

					done += ret;

				} while(done < uncompressed_size);

				fclose(pfile);
				unzCloseCurrentFile(uzFile);

				// finish up the progress for this file
				FinishProgress(uncompressed_size);
			}
		}
		if(unzGoToNextFile(uzFile) != UNZ_OK)
		{
			Stop = true;
		}
	}

	free(buffer);
	buffer = NULL;

	return 1;
}
