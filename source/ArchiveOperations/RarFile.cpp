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
#include <ogcsys.h>
#include <string.h>

#include "Language/gettext.h"
#include "Prompts/ProgressWindow.h"
#include "Prompts/PromptWindows.h"
#include "Tools/tools.h"
#include "RarFile.h"
#include "FileOperations/fileops.h"
#include "RarErrHnd.hpp"

ErrorHandler ErrHandler;

RarFile::RarFile(const char *filepath)
{
	RarArc.Open(filepath);
	RarArc.SetExceptions(false);
	this->LoadList();
}

RarFile::~RarFile()
{
	ClearList();
	RarArc.Close();
}

bool RarFile::LoadList()
{
	if (!RarArc.IsArchive(true))
	{
		RarArc.Close();
		return false;
	}

	if (!RarArc.IsOpened())
	{
		RarArc.Close();
		return false;
	}

	while(RarArc.ReadHeader() > 0)
	{
		int HeaderType=RarArc.GetHeaderType();
		if (HeaderType==ENDARC_HEAD)
			break;

		if(HeaderType == FILE_HEAD)
		{
			ArchiveFileStruct * TempStruct = new ArchiveFileStruct;

			int wstrlength = strlenw(RarArc.NewLhd.FileNameW);

			if(wstrlength > 0)
			{
				TempStruct->filename = new char[(wstrlength+1)*2];
				WideToUtf(RarArc.NewLhd.FileNameW, TempStruct->filename, (wstrlength+1)*2);
			}
			else
			{
				TempStruct->filename = new char[strlen(RarArc.NewLhd.FileName)+1];
				strcpy(TempStruct->filename, RarArc.NewLhd.FileName);
			}
			TempStruct->length = (size_t) RarArc.NewLhd.FullUnpSize;
			TempStruct->comp_length = (size_t) RarArc.NewLhd.FullPackSize;
			TempStruct->isdir = RarArc.IsArcDir();
			TempStruct->fileindex = RarStructure.size();
			TempStruct->ModTime = (u64) RarArc.NewLhd.mtime.GetDos();
			TempStruct->archiveType = RAR;

			RarStructure.push_back(TempStruct);
		}
		RarArc.SeekToNext();
	}
	return true;
}

void RarFile::ClearList()
{
	for(u32 i = 0; i < RarStructure.size(); i++)
	{
		if(RarStructure.at(i)->filename != NULL)
		{
			delete [] RarStructure.at(i)->filename;
			RarStructure.at(i)->filename = NULL;
		}
		if(RarStructure.at(i) != NULL)
		{
			delete RarStructure.at(i);
			RarStructure.at(i) = NULL;
		}
	}

	RarStructure.clear();
}

ArchiveFileStruct * RarFile::GetFileStruct(int ind)
{
	if(ind > (int) RarStructure.size() || ind < 0)
		return NULL;

	return RarStructure.at(ind);
}

u32 RarFile::GetItemCount()
{
	return RarStructure.size();
}


bool RarFile::SeekFile(int ind)
{
	if(ind < 0 || ind >= (int) RarStructure.size())
		return false;

	RarArc.Seek(0, SEEK_SET);

	while(RarArc.ReadHeader() > 0)
	{
		int HeaderType=RarArc.GetHeaderType();
		if (HeaderType==ENDARC_HEAD)
			break;

		if(HeaderType == FILE_HEAD && RarArc.NewLhd.FileName)
		{
			if(RarArc.NewLhd.FileNameW && RarArc.NewLhd.FileNameW[0] != 0)
			{
				char UnicodeName[1024];
				WideToUtf(RarArc.NewLhd.FileNameW, UnicodeName, sizeof(UnicodeName));

				if(strcmp(RarStructure[ind]->filename, UnicodeName) == 0)
					return true;
			}
			else
			{
				if(strcmp(RarStructure[ind]->filename, RarArc.NewLhd.FileName) == 0)
					return true;
			}
		}

		RarArc.SeekToNext();
	}

	return false;
}

bool RarFile::CheckPassword()
{
	if((RarArc.NewLhd.Flags & LHD_PASSWORD) && Password.length() == 0)
	{
		int choice = WindowPrompt(tr("Password is needed."), tr("Please enter the password."), tr("OK"), tr("Cancel"));
		if(!choice)
			return false;

		char entered[150];
		memset(entered, 0, sizeof(entered));

		if(OnScreenKeyboard(entered, sizeof(entered)) == 0)
			return false;

		Password.assign(entered);
	}

	return true;
}

class RarFileDataIO : public ComprDataIO
{
public:
	RarFileDataIO(Archive *srcFile) : archive(srcFile) {}
	virtual ~RarFileDataIO() {}
	//! Overload to handle progress and cancel
	int UnpRead(byte *Addr,size_t Count)
	{
		//! cancel progress
		if(ProgressWindow::Instance()->IsCanceled())
			return -1;

		//! internal functionality
		int result = ComprDataIO::UnpRead(Addr, Count);

		//! show progress
		ShowProgress(archive->CurBlockPos+CurUnpRead, UnpArcSize);

		return result;
	}
private:
	Archive *archive;
};

void RarFile::UnstoreFile(ComprDataIO &DataIO, int64 DestUnpSize)
{
	Array<byte> Buffer(0x10000);
	while (1)
	{
		uint Code=DataIO.UnpRead(&Buffer[0],Buffer.Size());
		if (Code==0 || (int)Code==-1)
			break;
		Code=Code<DestUnpSize ? Code:(uint)DestUnpSize;
		DataIO.UnpWrite(&Buffer[0],Code);
		if (DestUnpSize>=0)
			DestUnpSize-=Code;
	}
}

int RarFile::InternalExtractFile(const char * outpath, bool withpath)
{
	if (!RarArc.IsOpened())
		return -1;

	if(ProgressWindow::Instance()->IsCanceled())
		return PROGRESS_CANCELED;

	RarFileDataIO DataIO(&RarArc);
	Unpack Unp(&DataIO);
	Unp.Init(NULL);

	char filepath[MAXPATHLEN];
	char filename[255];

	if(RarArc.NewLhd.FileNameW && RarArc.NewLhd.FileNameW[0] != 0)
		WideToUtf(RarArc.NewLhd.FileNameW, filename, sizeof(filename));
	else
		snprintf(filename, sizeof(filename), "%s", RarArc.NewLhd.FileName);

	char * Realfilename = strrchr(filename, '/');
	if(!Realfilename)
		Realfilename = filename;
	else
		Realfilename++;

	if(withpath)
		snprintf(filepath, sizeof(filepath), "%s/%s", outpath, filename);
	else
		snprintf(filepath, sizeof(filepath), "%s/%s", outpath, Realfilename);

	if(RarArc.IsArcDir())
	{
		CreateSubfolder(filepath);
		return 1;
	}

	char * temppath = strdup(filepath);
	char * pointer = strrchr(temppath, '/');
	if(pointer)
	{
		pointer++;
		pointer[0] = '\0';
	}

	CreateSubfolder(temppath);

	free(temppath);
	temppath = NULL;

	if(!CheckPassword())
		return -2;

	RemoveFile(filepath);

	File CurFile;
	if(!CurFile.Create(filepath))
		return false;

	DataIO.UnpVolume = false;
	DataIO.UnpArcSize = RarArc.NewLhd.FullUnpSize;
	DataIO.CurUnpRead=0;
	DataIO.CurUnpWrite=0;
	DataIO.UnpFileCRC=RarArc.OldFormat ? 0 : 0xffffffff;
	DataIO.PackedCRC=0xffffffff;
	DataIO.SetEncryption(
	(RarArc.NewLhd.Flags & LHD_PASSWORD) ? RarArc.NewLhd.UnpVer:0, Password.c_str(),
	(RarArc.NewLhd.Flags & LHD_SALT) ? RarArc.NewLhd.Salt:NULL,false,
	RarArc.NewLhd.UnpVer>=36);
	DataIO.SetPackedSizeToRead(RarArc.NewLhd.FullPackSize);
	DataIO.SetFiles(&RarArc,&CurFile);
	DataIO.SetTestMode(false);
	DataIO.SetSkipUnpCRC(false);
	DataIO.EnableShowProgress(false);
	ErrHandler.Clean();

	//! Start always Progresstimer
	ShowProgress(0, RarArc.NewLhd.FullUnpSize, Realfilename);

	if (RarArc.NewLhd.Method == 0x30)
	{
		UnstoreFile(DataIO,RarArc.NewLhd.FullUnpSize);
	}
	else
	{
		Unp.SetDestSize(RarArc.NewLhd.FullUnpSize);

		if (RarArc.NewLhd.UnpVer <= 15)
			Unp.DoUnpack(15, false);
		else
			Unp.DoUnpack(RarArc.NewLhd.UnpVer,(RarArc.NewLhd.Flags & LHD_SOLID)!=0);
	}

	CurFile.Close();

	// finish up the progress for this file
	FinishProgress(RarArc.NewLhd.FullUnpSize);

	if(ProgressWindow::Instance()->IsCanceled())
	{
		RemoveFile(filepath);
		return PROGRESS_CANCELED;
	}

	if(ErrHandler.GetErrorCode() != 0 || ErrHandler.GetErrorCount() > 0)
	{
		RemoveFile(filepath);
		ThrowMsg(tr("Error:"), "%s %i", tr("Extract error code:"), ErrHandler.GetErrorCode());
		return -3;
	}

	if((!RarArc.OldFormat && UINT32(DataIO.UnpFileCRC) != UINT32(RarArc.NewLhd.FileCRC^0xffffffff)) ||
		(RarArc.OldFormat && UINT32(DataIO.UnpFileCRC) != UINT32(RarArc.NewLhd.FileCRC)))
	{
		ThrowMsg(tr("Error:"), tr("CRC of extracted file does not match. Wrong password?"));
		RemoveFile(filepath);
		return -4;
	}

	return 1;
}

int RarFile::ExtractFile(int fileindex, const char * outpath, bool withpath)
{
	if(!SeekFile(fileindex))
		return -6;

	return InternalExtractFile(outpath, withpath);
}

int RarFile::ExtractAll(const char * destpath)
{
	RarArc.Seek(0, SEEK_SET);

	while(RarArc.ReadHeader() > 0)
	{
		int HeaderType=RarArc.GetHeaderType();
		if (HeaderType==ENDARC_HEAD)
			break;

		if(HeaderType == FILE_HEAD)
		{
			int ret = InternalExtractFile(destpath, true);
			if(ret < 0)
				return ret;
		}
		RarArc.SeekToNext();
	}

	return 1;
}
