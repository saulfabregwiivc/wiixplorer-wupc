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
#ifndef _RAR_ERRHANDLER_
#define _RAR_ERRHANDLER_


enum { SUCCESS,WARNING,FATAL_ERROR,CRC_ERROR,LOCK_ERROR,WRITE_ERROR,
	   OPEN_ERROR,USER_ERROR,MEMORY_ERROR,CREATE_ERROR,USER_BREAK=255};

class ErrorHandler
{
  private:
	void ErrMsg(const char *ArcName,const char *fmt,...);

	int ExitCode;
	int ErrCount;
	bool EnableBreak;
	bool Silent;
	bool DoShutdown;
  public:
	ErrorHandler();
	void Clean();
	void MemoryError();
	void OpenError(const char *FileName);
	void CloseError(const char *FileName);
	void ReadError(const char *FileName);
	bool AskRepeatRead(const char *FileName);
	void WriteError(const char *ArcName,const char *FileName);
	void WriteErrorFAT(const char *FileName);
	bool AskRepeatWrite(const char *FileName,bool DiskFull);
	void SeekError(const char *FileName);
	void GeneralErrMsg(const char *Msg);
	void MemoryErrorMsg();
	void OpenErrorMsg(const char *FileName);
	void OpenErrorMsg(const char *ArcName,const char *FileName);
	void CreateErrorMsg(const char *FileName);
	void CreateErrorMsg(const char *ArcName,const char *FileName);
	void ReadErrorMsg(const char *ArcName,const char *FileName);
	void WriteErrorMsg(const char *ArcName,const char *FileName);
	void Exit(int ExitCode);
	void SetErrorCode(int Code);
	int GetErrorCode() {return(ExitCode);}
	int GetErrorCount() {return(ErrCount);}
	void SetSignalHandlers(bool Enable);
	void Throw(int Code);
	void SetSilent(bool Mode) {Silent=Mode;};
	void SetShutdown(bool Mode) {DoShutdown=Mode;};
	void SysErrMsg();
};

#endif
