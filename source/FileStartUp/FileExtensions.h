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
#ifndef FILE_EXTENSIONS_H_
#define FILE_EXTENSIONS_H_

class FilesExtensions
{
	public:
		FilesExtensions();
		~FilesExtensions();
		void SetDefault();
		//!< Set the extensions
		void SetVideo(const char * ext);
		void SetAudio(const char * ext);
		void SetImage(const char * ext);
		void SetArchive(const char * ext);
		void SetHomebrew(const char * ext);
		void SetFont(const char * ext);
		void SetLanguageFiles(const char * ext);
		void SetWiiBinary(const char * ext);
		void SetPDF(const char * ext);
		void SetWiiXplorerMovies(const char * ext);
		//!< Get the extensions
		const char * GetVideo() { return VideoFiles; };
		const char * GetAudio() { return AudioFiles; };
		const char * GetImage() { return ImageFiles; };
		const char * GetArchive() { return ArchiveFiles; };
		const char * GetHomebrew() { return HomebrewFiles; };
		const char * GetFont() { return FontFiles; };
		const char * GetLanguageFiles() { return LanguageFiles; };
		const char * GetWiiBinary() { return WiiBinaryFiles; };
		const char * GetPDF() { return PDFFiles; };
		const char * GetWiiXplorerMovies() { return WiiXplorerMovies; };
		//!< Compare the extensions to one given extension
		int CompareVideo(const char * ext);
		int CompareAudio(const char * ext);
		int CompareImage(const char * ext);
		int CompareArchive(const char * ext);
		int CompareHomebrew(const char * ext);
		int CompareFont(const char * ext);
		int CompareLanguageFiles(const char * ext);
		int CompareWiiBinary(const char * ext);
		int ComparePDF(const char * ext);
		int CompareWiiXplorerMovies(const char * ext);
	protected:
		void Clear();
		char * VideoFiles;
		char * AudioFiles;
		char * ImageFiles;
		char * ArchiveFiles;
		char * HomebrewFiles;
		char * FontFiles;
		char * WiiBinaryFiles;
		char * LanguageFiles;
		char * PDFFiles;
		char * WiiXplorerMovies;
};

#endif
