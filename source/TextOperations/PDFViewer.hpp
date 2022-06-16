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
#ifndef PDFVIEWER_HPP_
#define PDFVIEWER_HPP_

#include <mupdf/pdftool.h>
#include "ImageOperations/ImageViewer.h"

class PDFViewer : public ImageViewer
{
	public:
		PDFViewer(const char * filepath, const char * password = NULL);
		virtual ~PDFViewer();
		void OpenFile(const char * filename, const char * password = NULL);
		void CloseFile();
		bool LoadPage(int pagenum);
		bool NextPage();
		bool PreviousPage();
		//! Virtual overloads to adjust to imageviewer
		bool NextImage(bool silent UNUSED = false) { return NextPage(); };
		bool PreviousImage(bool silent UNUSED = false) { return PreviousPage(); };
	protected:
		void executeThread(void);
		int PreparePage(int pagenum);
		int PageToTexture();
		void FreePage();
		//! Virtual overloads which are not needed
		bool LoadImage(int index, bool silent UNUSED = false) { return LoadPage(index); };
		bool LoadImageList(const char * filepath UNUSED) { return true; };

		u8 * OutputImage;
		fz_glyphcache *drawcache;
		pdf_page *drawpage;
		float drawzoom;
		int LoopMode;
		int currentPage;
		int PageCount;
		int drawrotate;
		int imagewidth;
		int imageheight;
		int loadPage;
};

#endif
