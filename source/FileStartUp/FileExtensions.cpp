/****************************************************************************
 * Copyright (C) 2010
 * by Dimok
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
 * for WiiXplorer 2010
 ***************************************************************************/
#include <string.h>
#include <malloc.h>
#include "FileExtensions.h"
#include "Tools/StringTools.h"

FilesExtensions::FilesExtensions()
{
	VideoFiles = NULL;
	AudioFiles = NULL;
	ImageFiles = NULL;
	ArchiveFiles = NULL;
	HomebrewFiles = NULL;
	FontFiles = NULL;
	WiiBinaryFiles = NULL;
	LanguageFiles = NULL;
	PDFFiles = NULL;
	WiiXplorerMovies = NULL;

	SetDefault();
}

FilesExtensions::~FilesExtensions()
{
	Clear();
}

void FilesExtensions::Clear()
{
	if(VideoFiles)
		free(VideoFiles);
	VideoFiles = NULL;

	if(AudioFiles)
		free(AudioFiles);
	AudioFiles = NULL;

	if(ImageFiles)
		free(ImageFiles);
	ImageFiles = NULL;

	if(ArchiveFiles)
		free(ArchiveFiles);
	ArchiveFiles = NULL;

	if(HomebrewFiles)
		free(HomebrewFiles);
	HomebrewFiles = NULL;

	if(FontFiles)
		free(FontFiles);
	FontFiles = NULL;

	if(LanguageFiles)
		free(LanguageFiles);
	LanguageFiles = NULL;

	if(WiiBinaryFiles)
		free(WiiBinaryFiles);
	WiiBinaryFiles = NULL;

	if(PDFFiles)
		free(PDFFiles);
	PDFFiles = NULL;

	if(WiiXplorerMovies)
		free(WiiXplorerMovies);
	WiiXplorerMovies = NULL;
}

void FilesExtensions::SetDefault()
{
	Clear();

	VideoFiles = strdup(".asf,.avi,.fli,.flc,.trp,.divx,.mpg,.mpeg,.m1v,.flv,.mkv,.nuv,.ogm,.mov,.qt,.mp4,.rm,.rmvb,.ty,.viv,.vob,.wmv,.thp,.mth");
	AudioFiles = strdup(".mp3,.ogg,.wav,.pcm,.aif");
	ImageFiles = strdup(".png,.jpg,.jpeg,.jpe,.jfif,.bmp,.gif,.tga,.tif,.gd,.tpl");
	ArchiveFiles = strdup(".7z,.zip,.rar,.bnr,.arc");
	HomebrewFiles = strdup(".elf,.dol,.bak");
	FontFiles = strdup(".ttf");
	LanguageFiles = strdup(".lang");
	WiiBinaryFiles = strdup(".bin");
	PDFFiles = strdup(".pdf");
	WiiXplorerMovies = strdup(".thp,.mth");
}

void FilesExtensions::SetVideo(const char * ext)
{
	if(VideoFiles)
		free(VideoFiles);

	if(!ext)
	{
		VideoFiles = NULL;
		return;
	}

	VideoFiles = strdup(ext);
}

void FilesExtensions::SetAudio(const char * ext)
{
	if(AudioFiles)
		free(AudioFiles);

	if(!ext)
	{
		AudioFiles = NULL;
		return;
	}

	AudioFiles = strdup(ext);
}

void FilesExtensions::SetImage(const char * ext)
{
	if(ImageFiles)
		free(ImageFiles);

	if(!ext)
	{
		ImageFiles = NULL;
		return;
	}

	ImageFiles = strdup(ext);
}

void FilesExtensions::SetArchive(const char * ext)
{
	if(ArchiveFiles)
		free(ArchiveFiles);

	if(!ext)
	{
		ArchiveFiles = NULL;
		return;
	}

	ArchiveFiles = strdup(ext);
}

void FilesExtensions::SetHomebrew(const char * ext)
{
	if(HomebrewFiles)
		free(HomebrewFiles);

	if(!ext)
	{
		HomebrewFiles = NULL;
		return;
	}

	HomebrewFiles = strdup(ext);
}

void FilesExtensions::SetFont(const char * ext)
{
	if(FontFiles)
		free(FontFiles);

	if(!ext)
	{
		FontFiles = NULL;
		return;
	}

	FontFiles = strdup(ext);
}

void FilesExtensions::SetLanguageFiles(const char * ext)
{
	if(LanguageFiles)
		free(LanguageFiles);

	if(!ext)
	{
		LanguageFiles = NULL;
		return;
	}

	LanguageFiles = strdup(ext);
}

void FilesExtensions::SetWiiBinary(const char * ext)
{
	if(WiiBinaryFiles)
		free(WiiBinaryFiles);

	if(!ext)
	{
		WiiBinaryFiles = NULL;
		return;
	}

	WiiBinaryFiles = strdup(ext);
}

void FilesExtensions::SetPDF(const char * ext)
{
	if(PDFFiles)
		free(PDFFiles);

	if(!ext)
	{
		PDFFiles = NULL;
		return;
	}

	PDFFiles = strdup(ext);
}

void FilesExtensions::SetWiiXplorerMovies(const char * ext)
{
	if(WiiXplorerMovies)
		free(WiiXplorerMovies);

	if(!ext)
	{
		WiiXplorerMovies = NULL;
		return;
	}

	WiiXplorerMovies = strdup(ext);
}

int FilesExtensions::CompareVideo(const char * ext)
{
	if(!ext || !VideoFiles)
		return -1;

	return strtokcmp(ext, VideoFiles, ",");
}

int FilesExtensions::CompareAudio(const char * ext)
{
	if(!ext || !AudioFiles)
		return -1;

	return strtokcmp(ext, AudioFiles, ",");
}

int FilesExtensions::CompareImage(const char * ext)
{
	if(!ext || !ImageFiles)
		return -1;

	return strtokcmp(ext, ImageFiles, ",");
}

int FilesExtensions::CompareArchive(const char * ext)
{
	if(!ext || !ArchiveFiles)
		return -1;

	return strtokcmp(ext, ArchiveFiles, ",");
}

int FilesExtensions::CompareHomebrew(const char * ext)
{
	if(!ext || !HomebrewFiles)
		return -1;

	return strtokcmp(ext, HomebrewFiles, ",");
}

int FilesExtensions::CompareFont(const char * ext)
{
	if(!ext || !FontFiles)
		return -1;

	return strtokcmp(ext, FontFiles, ",");
}

int FilesExtensions::CompareLanguageFiles(const char * ext)
{
	if(!ext || !LanguageFiles)
		return -1;

	return strtokcmp(ext, LanguageFiles, ",");
}

int FilesExtensions::CompareWiiBinary(const char * ext)
{
	if(!ext || !WiiBinaryFiles)
		return -1;

	return strtokcmp(ext, WiiBinaryFiles, ",");
}

int FilesExtensions::ComparePDF(const char * ext)
{
	if(!ext || !PDFFiles)
		return -1;

	return strtokcmp(ext, PDFFiles, ",");
}

int FilesExtensions::CompareWiiXplorerMovies(const char * ext)
{
	if(!ext || !WiiXplorerMovies)
		return -1;

	return strtokcmp(ext, WiiXplorerMovies, ",");
}
