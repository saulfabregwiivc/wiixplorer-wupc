/***************************************************************************
 * Copyright (C) 2009
 * by Tantric
 *
 * Additions and modifications by Dimok
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
 ***************************************************************************/
#include "stdafx.h"
#include "Tools/tools.h"

#define DEFAULT_FIFO_SIZE 256 * 1024
static unsigned int *xfb[2] = { NULL, NULL }; // Double buffered
static int whichfb = 0; // Switch
static GXRModeObj *vmode = NULL; // Menu video mode
static unsigned char gp_fifo[DEFAULT_FIFO_SIZE] ATTRIBUTE_ALIGN (32);
Mtx GXmodelView2D;
int screenwidth = 640;
int screenheight = 480;
u32 frameCount = 0;


/****************************************************************************
 * ResetVideo_Menu
 *
 * Reset the video/rendering mode for the menu
****************************************************************************/
void
ResetVideo_Menu()
{
	Mtx44 p;
	f32 yscale;
	u32 xfbHeight;

	VIDEO_Configure (vmode);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (vmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();
	else
		while (VIDEO_GetNextField())
			VIDEO_WaitVSync();

	// clears the bg to color and clears the z buffer
	GXColor background = {0, 0, 0, 255};
	GX_SetCopyClear (background, 0x00ffffff);

	yscale = GX_GetYScaleFactor(vmode->efbHeight,vmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,vmode->fbWidth,vmode->efbHeight);
	GX_SetDispCopySrc(0,0,vmode->fbWidth,vmode->efbHeight);
	GX_SetDispCopyDst(vmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(vmode->aa,vmode->sample_pattern,GX_TRUE,vmode->vfilter);
	GX_SetFieldMode(vmode->field_rendering,((vmode->viHeight==2*vmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	if (vmode->aa)
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	else
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);

	// setup the vertex descriptor
	// tells the flipper to expect direct data
	GX_ClearVtxDesc();
	GX_InvVtxCache ();
	GX_InvalidateTexAll();

	GX_SetVtxDesc(GX_VA_TEX0, GX_NONE);
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc (GX_VA_CLR0, GX_DIRECT);

	GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt (GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GX_SetZMode (GX_FALSE, GX_LEQUAL, GX_TRUE);

	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

	guMtxIdentity(GXmodelView2D);
	guMtxTransApply (GXmodelView2D, GXmodelView2D, 0.0F, 0.0F, -200.0F);
	GX_LoadPosMtxImm(GXmodelView2D,GX_PNMTX0);

	guOrtho(p,0,screenheight-1,0,screenwidth-1,0,300);
	GX_LoadProjectionMtx(p, GX_ORTHOGRAPHIC);

	GX_SetViewport(0,0,vmode->fbWidth,vmode->efbHeight,0,1);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GX_SetAlphaUpdate(GX_TRUE);
}

/****************************************************************************
 * InitVideo
 *
 * This function MUST be called at startup.
 * - also sets up menu video mode
 ***************************************************************************/

void
InitVideo ()
{
	VIDEO_Init();
	vmode = VIDEO_GetPreferredMode(NULL); // get default video mode

	bool pal = (CONF_GetVideo() == CONF_VIDEO_PAL) && (CONF_GetEuRGB60() == 0);

	if(CONF_GetAspectRatio() == CONF_ASPECT_16_9)
	{
		vmode->viWidth = 708;
		screenwidth = 768;
	}

	if (pal)
	{
		vmode->viXOrigin = (VI_MAX_WIDTH_PAL - vmode->viWidth) / 2;
	}
	else
	{
		vmode->viXOrigin = (VI_MAX_WIDTH_NTSC - vmode->viWidth) / 2;
	}


	// Allocate the video buffers
	xfb[0] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer (vmode));
	xfb[1] = (u32 *) MEM_K0_TO_K1 (SYS_AllocateFramebuffer (vmode));

	VIDEO_Configure (vmode);

	// Clear framebuffers etc.
	VIDEO_ClearFrameBuffer (vmode, xfb[0], COLOR_BLACK);
	VIDEO_ClearFrameBuffer (vmode, xfb[1], COLOR_BLACK);
	VIDEO_SetNextFramebuffer (xfb[0]);

	VIDEO_SetBlack (FALSE);
	VIDEO_Flush ();
	VIDEO_WaitVSync ();
	if (vmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync ();

	// Initialize GX
	GXColor background = { 0, 0, 0, 0xff };
	memset (&gp_fifo, 0, DEFAULT_FIFO_SIZE);
	GX_Init (&gp_fifo, DEFAULT_FIFO_SIZE);
	GX_SetCopyClear (background, 0x00ffffff);
	GX_SetDispCopyGamma (GX_GM_1_0);
	GX_SetCullMode (GX_CULL_NONE);

	ResetVideo_Menu();
	// Finally, the video is up and ready for use :)
}

/****************************************************************************
 * StopGX
 *
 * Stops GX (when exiting)
 ***************************************************************************/
void StopGX()
{
	GX_DrawDone();
	GX_AbortFrame();
	GX_Flush();

	VIDEO_SetBlack(TRUE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (vmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();
}

/****************************************************************************
 * Menu_Render
 *
 * Renders everything current sent to GX, and flushes video
 ***************************************************************************/
void Menu_Render()
{
	whichfb ^= 1; // flip framebuffer
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(xfb[whichfb],GX_TRUE);
	GX_DrawDone();
	VIDEO_SetNextFramebuffer(xfb[whichfb]);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	frameCount++;
}

/****************************************************************************
 * Video_GetFrame
 ***************************************************************************/
u8 * Video_GetFrame(int * width, int * height)
{
	if(width)
		*width = vmode->fbWidth;
	if(height)
		*height = vmode->xfbHeight;

	int size = ALIGN32(2 * vmode->fbWidth * vmode->xfbHeight);

	u8 * buffer = (u8 *) memalign(32, size);
	if(!buffer)
		return NULL;

	memcpy(buffer, xfb[whichfb], size);

	return buffer;
}


//!This is really bad and ugly but i didn't have any other idea on how to solve this.
//!If someone has a better suggestion, i'll listen :D.
static inline
void CalculateCutoff(f32 realwidth, f32 realheight, f32 minwidth, f32 maxwidth,
					 f32 minheight, f32 maxheight, f32 scaleX, f32 scaleY,
					 f32 xpos, f32 ypos, int angle,
					 f32 &w1, f32 &h1, f32 &w2, f32 &h2, f32 &w3, f32 &h3, f32 &w4, f32 &h4,
					 f32 &o1, f32 &o2, f32 &o3, f32 &o4, f32 &u1, f32 &u2, f32 &u3, f32 &u4)
{
	if(angle < 0)
		angle += 360;

	if((angle <= 45 || angle > 315) || (angle > 135 && angle <= 225))
	{
		f32 RealXpos = xpos-(realwidth*scaleX-realwidth)/2.0f;
		f32 RealYpos = ypos-(realheight*scaleY-realheight)/2.0f;

		if(RealXpos < minwidth)
		{
			f32 cutoff = fabs(RealXpos-minwidth)/scaleX;
			f32 factor = cutoff/realwidth;

			if(cutoff > realwidth)
				cutoff = realwidth;
			else if(cutoff < 0.0f)
				cutoff = 0.0f;
			if(factor > 1.0f)
				factor = 1.0f;
			else if(factor < 0.0f)
				factor = 0.0f;
			if(angle <= 45 || angle > 315)
			{
				w1 += cutoff;
				w4 += cutoff;
				o1 += factor;
				u3 += factor;
			}
			else if(angle > 135 && angle <= 225)
			{
				w2 -= cutoff;
				w3 -= cutoff;
				o3 -= factor;
				u1 -= factor;
			}
		}
		if(RealXpos+realwidth*scaleX > maxwidth)
		{
			f32 cutoff = fabs(RealXpos+realwidth*scaleX-maxwidth)/scaleX;
			f32 factor = cutoff/realwidth;
			if(cutoff > realwidth)
				cutoff = realwidth;
			else if(cutoff < 0.0f)
				cutoff = 0.0f;
			if(factor > 1.0f)
				factor = 1.0f;
			else if(factor < 0.0f)
				factor = 0.0f;
			if(angle <= 45 || angle > 315)
			{
				w2 -= cutoff;
				w3 -= cutoff;
				o3 -= factor;
				u1 -= factor;
			}
			else if(angle > 135 && angle <= 225)
			{
				w1 += cutoff;
				w4 += cutoff;
				o1 += factor;
				u3 += factor;
			}
		}
		if(RealYpos < minheight)
		{
			f32 cutoff = fabs(RealYpos-minheight)/scaleY;
			f32 factor = cutoff/realheight;

			if(cutoff > realheight)
				cutoff = realheight;
			else if(cutoff < 0.0f)
				cutoff = 0.0f;
			if(factor > 1.0f)
				factor = 1.0f;
			else if(factor < 0.0f)
				factor = 0.0f;
			if(angle <= 45 || angle > 315)
			{
				h1 += cutoff;
				h2 += cutoff;
				o2 += factor;
				o4 += factor;
			}
			else if(angle > 135 && angle <= 225)
			{
				h3 -= cutoff;
				h4 -= cutoff;
				u2 -= factor;
				u4 -= factor;
			}
		}
		if(RealYpos+realheight*scaleY > maxheight)
		{
			f32 cutoff = fabs(RealYpos+realheight*scaleY-maxheight)/scaleY;
			f32 factor = cutoff/realheight;
			if(cutoff > realheight)
				cutoff = realheight;
			else if(cutoff < 0.0f)
				cutoff = 0.0f;
			if(factor > 1.0f)
				factor = 1.0f;
			else if(factor < 0.0f)
				factor = 0.0f;
			if(angle <= 45 || angle > 315)
			{
				h3 -= cutoff;
				h4 -= cutoff;
				u2 -= factor;
				u4 -= factor;
			}
			else if(angle > 135 && angle <= 225)
			{
				h1 += cutoff;
				h2 += cutoff;
				o2 += factor;
				o4 += factor;
			}
		}
	}
	else if((angle > 45 && angle <= 135) || (angle > 225 && angle <= 315))
	{
		f32 RealXpos = xpos+realwidth/2.0f-realheight/2.0f-(realheight*scaleY-realheight)/2.0f;
		f32 RealYpos = ypos+realheight/2.0f-realwidth/2.0f-(realwidth*scaleX-realwidth)/2.0f;

		if(RealXpos < minwidth)
		{
			f32 cutoff = fabs(RealXpos-minwidth)/scaleY;
			f32 factor = cutoff/realheight;
			if(cutoff > realheight)
				cutoff = realheight;
			else if(cutoff < 0.0f)
				cutoff = 0.0f;
			if(factor > 1.0f)
				factor = 1.0f;
			else if(factor < 0.0f)
				factor = 0.0f;
			if(angle > 45 && angle <= 135)
			{
				h3 -= cutoff;
				h4 -= cutoff;
				u2 -= factor;
				u4 -= factor;
			}
			else if(angle > 225 && angle <= 315)
			{
				h1 += cutoff;
				h2 += cutoff;
				o2 += factor;
				o4 += factor;
			}
		}
		if(RealXpos+realheight*scaleY > maxwidth)
		{
			f32 cutoff = fabs(RealXpos+realheight*scaleY-maxwidth)/scaleY;
			f32 factor = cutoff/realheight;
			if(cutoff > realheight)
				cutoff = realheight;
			else if(cutoff < 0)
				cutoff = 0.0f;
			if(factor > 1.0f)
				factor = 1.0f;
			else if(factor < 0)
				factor = 0.0f;
			if(angle > 45 && angle <= 135)
			{
				h1 += cutoff;
				h2 += cutoff;
				o2 += factor;
				o4 += factor;
			}
			else if(angle > 225 && angle <= 315)
			{
				h3 -= cutoff;
				h4 -= cutoff;
				u2 -= factor;
				u4 -= factor;
			}
		}
		if(RealYpos < minheight)
		{
			f32 cutoff = fabs(RealYpos-minheight)/scaleX;
			f32 factor = cutoff/realwidth;
			if(cutoff > realwidth)
				cutoff = realwidth;
			else if(cutoff < 0.0f)
				cutoff = 0.0f;
			if(factor > 1.0f)
				factor = 1.0f;
			else if(factor < 0.0f)
				factor = 0.0f;
			if(angle > 45 && angle <= 135)
			{
				w1 += cutoff;
				w4 += cutoff;
				o1 += factor;
				u3 += factor;
			}
			else if(angle > 225 && angle <= 315)
			{
				w2 -= cutoff;
				w3 -= cutoff;
				o3 -= factor;
				u1 -= factor;
			}
		}
		if(RealYpos+realwidth*scaleX > maxheight)
		{
			f32 cutoff = fabs(RealYpos+realwidth*scaleX-maxheight)/scaleX;
			f32 factor = cutoff/realwidth;
			if(cutoff > realwidth)
				cutoff = realwidth;
			else if(cutoff < 0.0f)
				cutoff = 0.0f;
			if(factor > 1.0f)
				factor = 1.0f;
			else if(factor < 0.0f)
				factor = 0.0f;
			if(angle > 45 && angle <= 135)
			{
				w2 -= cutoff;
				w3 -= cutoff;
				o3 -= factor;
				u1 -= factor;
			}
			else if(angle > 225 && angle <= 315)
			{
				w1 += cutoff;
				w4 += cutoff;
				o1 += factor;
				u3 += factor;
			}
		}
	}
}

/****************************************************************************
 * Menu_DrawImg
 *
 * Draws the specified image on screen using GX
 ***************************************************************************/
void Menu_DrawImg(u8 data[], u16 width, u16 height, u8 format, f32 xpos, f32 ypos, f32 zpos,
				  f32 degrees, f32 scaleX, f32 scaleY, u8 alpha)
{
	if(data == NULL)
		return;

	GXTexObj texObj;

	GX_InitTexObj(&texObj, data, width, height, format, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_LoadTexObj(&texObj, GX_TEXMAP0);
	GX_InvalidateTexAll();

	GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);

	Mtx m,m1,m2, mv;
	f32 halfwidth = width * 0.5f;
	f32 halfheight = height * 0.5f;
	guMtxIdentity (m1);
	guMtxScaleApply(m1,m1,scaleX,scaleY, 1.0f);
	guMtxRotDeg (m2, 'z', degrees);
	guMtxConcat(m2,m1,m);

	guMtxTransApply(m,m, xpos+halfwidth, ypos+halfheight, zpos);
	guMtxConcat (GXmodelView2D, m, mv);
	GX_LoadPosMtxImm (mv, GX_PNMTX0);

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
	GX_Position3f32(-halfwidth, -halfheight, 0.0f);
	GX_Color4u8(0xFF,0xFF,0xFF, alpha);
	GX_TexCoord2f32(0.0f, 0.0f);

	GX_Position3f32(halfwidth, -halfheight, 0.0f);
	GX_Color4u8(0xFF,0xFF,0xFF, alpha);
	GX_TexCoord2f32(1.0f, 0.0f);

	GX_Position3f32(halfwidth, halfheight, 0.0f);
	GX_Color4u8(0xFF,0xFF,0xFF, alpha);
	GX_TexCoord2f32(1.0f, 1.0f);

	GX_Position3f32(-halfwidth, halfheight, 0.0f);
	GX_Color4u8(0xFF,0xFF,0xFF, alpha);
	GX_TexCoord2f32(0.0f, 1.0f);
	GX_End();

	GX_LoadPosMtxImm (GXmodelView2D, GX_PNMTX0);
}

/****************************************************************************
 * Menu_DrawImg with cut off
 ***************************************************************************/
void Menu_DrawImgCut(u8 data[], u16 width, u16 height, u8 format, f32 xpos, f32 ypos, f32 zpos,
					 f32 degrees, f32 scaleX, f32 scaleY, u8 alpha, f32 minwidth, f32 maxwidth,
					 f32 minheight, f32 maxheight)
{
	if(data == NULL)
		return;

	GXTexObj texObj;

	GX_InitTexObj(&texObj, data, width, height, format, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GX_LoadTexObj(&texObj, GX_TEXMAP0);
	GX_InvalidateTexAll();

	GX_SetVtxDesc (GX_VA_TEX0, GX_DIRECT);

	Mtx m,m1,m2, mv;
	f32 halfwidth = width * 0.5f;
	f32 halfheight = height * 0.5f;
	guMtxIdentity (m1);
	guMtxScaleApply(m1,m1,scaleX,scaleY, 1.0f);
	guMtxRotDeg (m2, 'z', degrees);
	guMtxConcat(m2,m1,m);

	guMtxTransApply(m,m, xpos+halfwidth, ypos+halfheight, zpos);
	guMtxConcat (GXmodelView2D, m, mv);
	GX_LoadPosMtxImm (mv, GX_PNMTX0);

	f32 w1 = (f32) -halfwidth, h1 = (f32) -halfheight;
	f32 w2 = (f32) halfwidth, h2 = (f32) -halfheight;
	f32 w3 = (f32) halfwidth, h3 = (f32) halfheight;
	f32 w4 = (f32) -halfwidth, h4 = (f32) halfheight;
	f32 o1 = 0.0f, o2 = 0.0f;
	f32 o3 = 1.0f, o4 = 0.0f;
	f32 u1 = 1.0f, u2 = 1.0f;
	f32 u3 = 0.0f, u4 = 1.0f;

	CalculateCutoff(width, height, minwidth, maxwidth, minheight, maxheight,
					scaleX, scaleY, xpos, ypos, fmod(degrees, 360),
					w1, h1, w2, h2, w3, h3, w4, h4, o1, o2, o3, o4, u1, u2, u3, u4);

	GX_Begin(GX_QUADS, GX_VTXFMT0,4);
	GX_Position3f32(w1, h1,  0);
	GX_Color4u8(0xFF,0xFF,0xFF,alpha);
	GX_TexCoord2f32(o1, o2);

	GX_Position3f32(w2, h2,  0);
	GX_Color4u8(0xFF,0xFF,0xFF,alpha);
	GX_TexCoord2f32(o3, o4);

	GX_Position3f32(w3, h3,  0);
	GX_Color4u8(0xFF,0xFF,0xFF,alpha);
	GX_TexCoord2f32(u1, u2);

	GX_Position3f32(w4, h4,  0);
	GX_Color4u8(0xFF,0xFF,0xFF,alpha);
	GX_TexCoord2f32(u3, u4);
	GX_End();

	GX_LoadPosMtxImm (GXmodelView2D, GX_PNMTX0);
}

/****************************************************************************
 * Menu_DrawRectangle
 *
 * Draws a rectangle at the specified coordinates using GX
 ***************************************************************************/
void Menu_DrawRectangle(f32 x, f32 y, f32 z, f32 width, f32 height, GXColor * color, bool multicolor, bool filled)
{
	GX_SetTevOp (GX_TEVSTAGE0, GX_PASSCLR);
	GX_SetVtxDesc (GX_VA_TEX0, GX_NONE);

	u8 fmt = GX_TRIANGLEFAN;
	long n = 4;
	f32 x2 = x+width;
	f32 y2 = y+height;
	guVector v[] = {{x,y,z}, {x2,y,z}, {x2,y2,z}, {x,y2,z}, {x,y,z}};

	if(!filled)
	{
		fmt = GX_LINESTRIP;
		n = 5;
	}

	GX_Begin(fmt, GX_VTXFMT0, n);
	for(long i=0; i<n; ++i)
	{
		GX_Position3f32(v[i].x, v[i].y,  v[i].z);

		if(multicolor)
			GX_Color4u8(color[i].r, color[i].g, color[i].b, color[i].a);
		else
			GX_Color4u8(color[0].r, color[0].g, color[0].b, color[0].a);
	}
	GX_End();

	GX_SetTevOp (GX_TEVSTAGE0, GX_MODULATE);
}
