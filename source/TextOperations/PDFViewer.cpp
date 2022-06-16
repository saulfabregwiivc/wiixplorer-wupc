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
#include <gccore.h>
#include <malloc.h>
#include "Controls/Application.h"
#include "Controls/Taskbar.h"
#include "PDFViewer.hpp"
#include "sys.h"

PDFViewer::PDFViewer(const char * filepath, const char * password)
	: ImageViewer(NULL)
{
	OutputImage = NULL;
	loadPage = -1;
	currentPage = 1;
	drawzoom = Settings.PDFLoadZoom;
	drawrotate = 0;
	drawcache = (fz_glyphcache*) nil;
	drawpage = (pdf_page*) nil;
	//!the TrashButton from imageviewer isn't needed
	Remove(trashButton);

	fz_cpudetect();
	fz_accelerate();
	closexref();

	OpenFile(filepath, password);
	LoadPage(currentPage);
}

PDFViewer::~PDFViewer()
{
	bExitRequested = true;
	shutdownThread();
	CloseFile();
}

void PDFViewer::OpenFile(const char * filepath, const char * password)
{
	CloseFile();
	drawcache = fz_newglyphcache();
	openxref((char *) filepath, (char *) password, 0);
	if(xref)
		PageCount = pdf_getpagecount(xref);
}

void PDFViewer::CloseFile()
{
	FreePage();

	if (xref && xref->store)
	{
		pdf_dropstore(xref->store);
		xref->store = (pdf_store_s*) nil;
	}
	if (drawcache)
	{
		fz_freeglyphcache(drawcache);
		drawcache = (fz_glyphcache*) nil;
	}

	closexref();
}

int PDFViewer::PreparePage(int pagenum)
{
	fz_error error;
	fz_obj * pageobj = pdf_getpageobject(xref, pagenum);
	error = pdf_loadpage(&drawpage, xref, pageobj);
	if (error)
	{
		ThrowMsg(tr("Error:"), tr("Can't load page."));
		return -1;
	}

	return 0;
}

void PDFViewer::executeThread(void)
{
	//! the thread is started inside image viewer class
	//! wait here till consturctor is finished of pdf viewer
	suspendThread();

	while(!bExitRequested)
	{
		if(loadPage == -1)
			suspendThread();

		if(!bExitRequested)
		{
			int ret = PreparePage(loadPage);
			if(ret < 0) {
				continue;
			}
			ret = PageToTexture();
			if(ret <= 0)
				continue;

			image->SetImage(OutputImage, imagewidth, imageheight, GX_TF_RGB565);
			SetStartUpImageSize();

			if(SlideShowStart > 0)
				image->SetEffect(EFFECT_FADE, Settings.ImageFadeSpeed);

			loadPage = -1;
		}
	}
}

bool PDFViewer::LoadPage(int pagenum)
{
	if(!xref)
		return false;

	FreePage();

	loadPage = pagenum;
	resumeThread();

	return true;
}

bool PDFViewer::NextPage()
{
	++currentPage;

	if(currentPage > PageCount)
		currentPage = 1;

	return LoadPage(currentPage);
}

bool PDFViewer::PreviousPage()
{
	--currentPage;
	if(currentPage < 1)
		currentPage = PageCount;

	return LoadPage(currentPage);
}

void PDFViewer::FreePage()
{
	if(image && OutputImage && SlideShowStart > 0)
	{
		image->SetEffect(EFFECT_FADE, -Settings.ImageFadeSpeed);
		Application::Instance()->SetGuiInputUpdate(false);

		while(image->GetEffect() > 0)
			Application::Instance()->updateEvents();

		Application::Instance()->SetGuiInputUpdate(true);
	}

	image->SetImage(NULL, 0, 0);

	if(OutputImage)
		free(OutputImage);
	OutputImage = NULL;

	if(drawpage != nil)
		pdf_droppage(drawpage);
	drawpage = (pdf_page *) nil;
	flushxref();
	if (xref && xref->store)
	{
		pdf_agestoreditems(xref->store);
		pdf_evictageditems(xref->store);
	}
}

int PDFViewer::PageToTexture()
{
	fz_error error;
	fz_matrix ctm;
	fz_bbox bbox;
	fz_pixmap *pix;

	ctm = fz_identity();
	ctm = fz_concat(ctm, fz_translate(0, -drawpage->mediabox.y1));
	ctm = fz_concat(ctm, fz_scale(drawzoom, -drawzoom));
	ctm = fz_concat(ctm, fz_rotate(drawrotate + drawpage->rotate));

	bbox = fz_roundrect(fz_transformrect(ctm, drawpage->mediabox));
	int w = ALIGN(bbox.x1 - bbox.x0);
	int h = ALIGN(bbox.y1 - bbox.y0);

	pix = fz_newpixmap(pdf_devicergb, bbox.x0, bbox.y0, w, h);
	if(!pix)
	{
		FreePage();
		ThrowMsg(tr("Error:"), tr("Not enough memory."));
		return -1;
	}

	// initialize white page
	memset(pix->samples, 0xff, pix->h * pix->w * pix->n);

	fz_device *dev = fz_newdrawdevice(drawcache, pix);
	error = pdf_runcontentstream(dev, ctm, xref, drawpage->resources, drawpage->contents);
	fz_freedevice(dev);
	if (error)
	{
		fz_droppixmap(pix);
		FreePage();
		return -1;
	}

	// convert it to texture
	int len = (w * h) << 1;

	OutputImage = (u8 *) memalign(32, len);
	if(!OutputImage)
	{
		fz_droppixmap(pix);
		FreePage();
		ThrowMsg(tr("Error:"), tr("Not enough memory."));
		return -1;
	}

	int x, y;
	int x1, y1;
	int iv;

	for(iv = 0, y1 = 0; y1 < h; y1 += 4)
	{
		for(x1 = 0; x1 < w; x1 += 4)
		{
			for(y = y1; y < (y1 + 4); y++)
			{
				for(x = x1; x < (x1 + 4); x++)
				{
					if(x < pix->w && y < pix->h)
					{
						unsigned char *src = pix->samples + y * pix->w * 4;

						u8 r = src[x * 4 + 1] >> 3;
						u8 g = src[x * 4 + 2] >> 2;
						u8 b = src[x * 4 + 3] >> 3;

						*(u16*)(OutputImage + ((iv++) << 1)) = (r << 11) | (g << 5) | (b);
					}
					else {
						*(u16*)(OutputImage + ((iv++) << 1)) = 0xFFFF; // white
					}
				}
			}
		}
	}

	imagewidth = w;
	imageheight = h;

	DCFlushRange(OutputImage, len);

	fz_droppixmap(pix);

	if(drawpage != nil)
		pdf_droppage(drawpage);
	drawpage = (pdf_page *) nil;
	flushxref();
	if (xref && xref->store)
	{
		pdf_agestoreditems(xref->store);
		pdf_evictageditems(xref->store);
	}

	return len;
}
