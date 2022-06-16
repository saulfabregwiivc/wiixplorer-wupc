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
#ifndef _FILEOPS_H_
#define _FILEOPS_H_

#include <vector>
#include <string>
#include <gctypes.h>

bool CreateSubfolder(const char * fullpath);
bool CheckFile(const char * filepath);
u64 FileSize(const char * filepath);
int LoadFileToMem(const char * filepath, u8 **buffer, u32 *size);
int LoadFileToMemWithProgress(const char *progressText, const char *filePath, u8 **buffer, u32 *size);
int CopyFile(const char * src, const char * dest);
int MoveFile(const char *srcpath, const char *destdir);
int RemoveDirectory(const char * dirpath);
bool RenameFile(const char * srcpath, const char * destpath);
bool RemoveFile(const char * filepath);
void GetFolderSize(const char * folderpath, u64 &foldersize, u32 &filenumber, const bool &bCancel);
bool CompareDevices(const char *src, const char *dest);
void ResetReplaceChoice();

#endif
