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
#define BIG_ENDIAN
#include <libunrar/rar.hpp>
#include "Prompts/PromptWindows.h"
#include "Language/gettext.h"
#include "Tools/tools.h"
#include "sys.h"

ErrorHandler::ErrorHandler()
{
	Clean();
}

void ErrorHandler::Clean()
{
	ExitCode=SUCCESS;
	ErrCount=0;
	EnableBreak=true;
	Silent=false;
	DoShutdown=false;
}


void ErrorHandler::MemoryError()
{
	ThrowMsg(tr("Error:"), tr("Not enought memory."));
	ExitCode = MEMORY_ERROR;
}


void ErrorHandler::OpenError(const char *FileName)
{
	ThrowMsg(tr("Error:"), tr("Cannot open file %s"), FileName);
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::CloseError(const char *FileName)
{
	ThrowMsg(tr("Error:"), tr("Cannot close file %s"), FileName);
	ExitCode = FATAL_ERROR;
}

void ErrorHandler::ReadError(const char *FileName)
{
	ThrowMsg(tr("Error:"), tr("Read Error in %s"), FileName);
	ExitCode = FATAL_ERROR;
}

bool ErrorHandler::AskRepeatRead(const char *FileName)
{
	char output[200];
	snprintf(output, sizeof(output), tr("Read Error in file: %s"), FileName);

	int choice = WindowPrompt(output, tr("Retry?"), tr("Yes"), tr("Cancel"));

	return (choice != 0);
}

void ErrorHandler::WriteError(const char *ArcName,const char *FileName)
{
	ThrowMsg(tr("Error:"), tr("Write Error from %s to %s"), ArcName, FileName);
	ExitCode = FATAL_ERROR;
}

bool ErrorHandler::AskRepeatWrite(const char *FileName,bool DiskFull)
{
	char output[200];
	snprintf(output, sizeof(output), tr("Write Error in file: %s. %s"), FileName, DiskFull ? tr("Disk is Full") : "");

	int choice = WindowPrompt(output, tr("Retry?"), tr("Yes"), tr("Cancel"));

	return (choice != 0);
}


void ErrorHandler::SeekError(const char *FileName)
{
	ThrowMsg(tr("Error:"), tr("Seek Error in %s"), FileName);
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::GeneralErrMsg(const char *Msg)
{
	ThrowMsg(tr("Error:"), Msg);
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::MemoryErrorMsg()
{
	ExitCode = MEMORY_ERROR;
}


void ErrorHandler::OpenErrorMsg(const char *FileName)
{
	ThrowMsg(tr("Error:"), tr("Error opening %s"), FileName);
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::OpenErrorMsg(const char *ArcName,const char *FileName)
{
	ThrowMsg(tr("Error:"), tr("Error opening %s in %s"), FileName, ArcName);
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::CreateErrorMsg(const char *FileName)
{
	CreateErrorMsg(NULL,FileName);
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::CreateErrorMsg(const char *ArcName,const char *FileName)
{
	ThrowMsg(tr("Error:"), tr("Error received from %s in %s"), FileName, ArcName);
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::ReadErrorMsg(const char *ArcName,const char *FileName)
{
	ThrowMsg(tr("Error:"), tr("Read error from %s in %s"), FileName, ArcName);
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::WriteErrorMsg(const char *ArcName,const char *FileName)
{
	ThrowMsg(tr("Error:"), tr("Write error to %s in %s"), FileName, ArcName);
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::Exit(int ExitCode)
{
	Throw(ExitCode);
}

void ErrorHandler::ErrMsg(const char *ArcName,const char *fmt,...)
{
	safebuf char Msg[NM+1024];
	va_list argptr;
	va_start(argptr,fmt);
	vsprintf(Msg,fmt,argptr);
	va_end(argptr);
	if (*Msg)
	{
		ThrowMsg(tr("Error:"), "%s: %s", ArcName, Msg);
	}
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::SetErrorCode(int Code)
{
  switch(Code)
  {
	case WARNING:
	case USER_BREAK:
	  if (ExitCode==SUCCESS)
		ExitCode=Code;
	  break;
	case FATAL_ERROR:
	  if (ExitCode==SUCCESS || ExitCode==WARNING)
		ExitCode=FATAL_ERROR;
	  break;
	default:
	  ExitCode=Code;
	  break;
  }
  ErrCount++;
}


void ErrorHandler::SetSignalHandlers(bool Enable UNUSED)
{
//  EnableBreak=Enable;
//#if !defined(GUI) && !defined(_SFX_RTL_)
//#ifdef _WIN_32
//  SetConsoleCtrlHandler(Enable ? ProcessSignal:NULL,TRUE);
////  signal(SIGBREAK,Enable ? ProcessSignal:SIG_IGN);
//#else
//  signal(SIGINT,Enable ? ProcessSignal:SIG_IGN);
//  signal(SIGTERM,Enable ? ProcessSignal:SIG_IGN);
//#endif
//#endif
}

void ErrorHandler::Throw(int Code)
{
	if (Code==USER_BREAK && !EnableBreak)
		return;

	ThrowMsg(tr("Error:"), tr("Fatal error: %i."), Code);
	ExitCode = FATAL_ERROR;
}


void ErrorHandler::SysErrMsg()
{
	ThrowMsg(tr("Error:"), tr("System error received"));
	ExitCode = FATAL_ERROR;
}

