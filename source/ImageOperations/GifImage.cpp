/***************************************************************************
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
#include <string.h>
#include "VideoOperations/video.h"
#include "Language/gettext.h"
#include "Tools/tools.h"
#include "GifImage.hpp"

#define LZW_BUFF_SIZE   (17*1024)

typedef struct _GIFLSDtag
{
	u16 ScreenWidth;
	u16 ScreenHeight;
	u8 PackedFields;
	u8 Background;
	u8 PixelAspectRatio;
} __attribute__((__packed__)) GIFLSDtag;

typedef struct _GIFGCEtag
{
	u8 BlockSize;	   // Block Size: 4 bytes
	u8 PackedFields;
	u16 Delay;		  // Delay Time (1/100 seconds)
	u8 Transparent;	 // Transparent Color Index
} __attribute__((__packed__)) GIFGCEtag;

// Read Image Descriptor
typedef struct _GIFIDtag
{
	u16 xPos;
	u16 yPos;
	u16 Width;
	u16 Height;
	u8 PackedFields;
} __attribute__((__packed__)) GIFIDtag;

typedef struct
{
	u8 b,g,r,a;
} __attribute__((__packed__)) GIFCOLOR;


GifImage::GifImage(const u8 * img, int imgSize)
{
	currentFrame = 0;
	lastTimer = 0.0f;
	LoadImage(img, imgSize);
}

GifImage::~GifImage()
{
	for(u32 i = 0; i < Frames.size(); i++)
	{
		if(Frames[i].image)
			free(Frames[i].image);
	}
	Frames.clear();
}

u8 * GifImage::GetFrameImage(int pos)
{
	if(pos < 0 || pos >= GetFrameCount())
		return NULL;

	return Frames[pos].image;
}

// ****************************************************************************
// * LZWDecoder (C/C++)													   *
// * Codec to perform LZW (GIF Variant) decompression.						*
// *						 (c) Nov2000, Juan Soulie <jsoulie@cplusplus.com> *
// ****************************************************************************
//
// Parameter description:
//  - bufIn: Input buffer containing a "de-blocked" GIF/LZW compressed image.
//  - bufOut: Output buffer where result will be stored.
//  - InitCodeSize: Initial CodeSize to be Used
//	(GIF files include this as the first byte in a picture block)
//  - AlignedWidth : Width of a row in memory (including alignment if needed)
//  - Width, Height: Physical dimensions of image.
//  - Interlace: 1 for Interlaced GIFs.
//
static int LZWDecoder (char * bufIn, char * bufOut,
				short InitCodeSize, int AlignedWidth,
				int Width, int Height, const int Interlace)
{
	int n;
	int row=0,col=0;				// used to point output if Interlaced
	int nPixels, maxPixels;			// Output pixel counter

	short CodeSize;					// Current CodeSize (size in bits of codes)
	short ClearCode;				// Clear code : resets decompressor
	short EndCode;					// End code : marks end of information

	long whichBit;					// Index of next bit in bufIn
	long LongCode;					// Temp. var. from which Code is retrieved
	short Code;						// Code extracted
	short PrevCode = 0;				// Previous Code
	short OutCode;					// Code to output

	// Translation Table:
	short *Prefix = new short[LZW_BUFF_SIZE+1];				// Prefix: index of another Code
	unsigned char *Suffix = new unsigned char[LZW_BUFF_SIZE+1];		// Suffix: terminating character
	short FirstEntry;				// Index of first free entry in table
	short NextEntry;				// Index of next free entry in table

	unsigned char *OutStack = new unsigned char[LZW_BUFF_SIZE+1];	// Output buffer
	int OutIndex;					// Characters in OutStack

	int RowOffset;					// Offset in output buffer for current row

	// Set up values that depend on InitCodeSize Parameter.
	CodeSize = InitCodeSize+1;
	ClearCode = (1 << InitCodeSize);
	EndCode = ClearCode + 1;
	NextEntry = FirstEntry = ClearCode + 2;

	whichBit=0;
	nPixels = 0;
	maxPixels = Width*Height;
	RowOffset =0;

	while (nPixels<maxPixels) {
		OutIndex = 0;							// Reset Output Stack

		// GET NEXT CODE FROM bufIn:
		// LZW compression uses code items longer than a single byte.
		// For GIF Files, code sizes are variable between 9 and 12 bits
		// That's why we must read data (Code) this way:
		LongCode=le32(*((u32*)(bufIn+whichBit/8)));	// Get some bytes from bufIn
		LongCode>>=(whichBit&7);				// Discard too low bits
		Code =(LongCode & ((1<<CodeSize)-1) );	// Discard too high bits
		whichBit += CodeSize;					// Increase Bit Offset

		// SWITCH, DIFFERENT POSIBILITIES FOR CODE:
		if (Code == EndCode)					// END CODE
			break;								// Exit LZW Decompression loop

		if (Code == ClearCode) {				// CLEAR CODE:
			CodeSize = InitCodeSize+1;			// Reset CodeSize
			NextEntry = FirstEntry;				// Reset Translation Table
			PrevCode=Code;				// Prevent next to be added to table.
			continue;							// restart, to get another code
		}
		if (Code < NextEntry)					// CODE IS IN TABLE
			OutCode = Code;						// Set code to output.

		else {									// CODE IS NOT IN TABLE:
			OutIndex++;			// Keep "first" character of previous output.
			OutCode = PrevCode;					// Set PrevCode to be output
		}

		// EXPAND OutCode IN OutStack
		// - Elements up to FirstEntry are Raw-Codes and are not expanded
		// - Table Prefices contain indexes to other codes
		// - Table Suffices contain the raw codes to be output
		while (OutCode >= FirstEntry)
		{
			if (OutIndex > LZW_BUFF_SIZE)
			{
				delete [] OutStack;
				delete [] Suffix;
				delete [] Prefix;
				return 0;
			}
			OutStack[OutIndex++] = Suffix[OutCode];	// Add suffix to Output Stack
			OutCode = Prefix[OutCode];				// Loop with preffix
		}

		// NOW OutCode IS A RAW CODE, ADD IT TO OUTPUT STACK.
		if (OutIndex > LZW_BUFF_SIZE)
		{
			delete [] OutStack;
			delete [] Suffix;
			delete [] Prefix;
			return 0;
		}
		OutStack[OutIndex++] = (unsigned char) OutCode;

		// ADD NEW ENTRY TO TABLE (PrevCode + OutCode)
		// (EXCEPT IF PREVIOUS CODE WAS A CLEARCODE)
		if (PrevCode!=ClearCode) {
			Prefix[NextEntry] = PrevCode;
			Suffix[NextEntry] = (unsigned char) OutCode;
			NextEntry++;

			// Prevent Translation table overflow:
			if (NextEntry>=LZW_BUFF_SIZE)
			{
				delete [] OutStack;
				delete [] Suffix;
				delete [] Prefix;
				return 0;
			}

			// INCREASE CodeSize IF NextEntry IS INVALID WITH CURRENT CodeSize
			if (NextEntry >= (1<<CodeSize)) {
				if (CodeSize < 12) CodeSize++;
				else {}				// Do nothing. Maybe next is Clear Code.
			}
		}

		PrevCode = Code;

		// Avoid the possibility of overflow on 'bufOut'.
		if (nPixels + OutIndex > maxPixels) OutIndex = maxPixels-nPixels;

		// OUTPUT OutStack (LAST-IN FIRST-OUT ORDER)
		for (n=OutIndex-1; n>=0; n--) {
			if (col==Width)						// Check if new row.
			{
				if (Interlace) {				// If interlaced::
						 if ((row&7)==0) {row+=8; if (row>=Height) row=4;}
					else if ((row&3)==0) {row+=8; if (row>=Height) row=2;}
					else if ((row&1)==0) {row+=4; if (row>=Height) row=1;}
					else row+=2;
				}
				else							// If not interlaced:
					row++;

				RowOffset=row*AlignedWidth;		// Set new row offset
				col=0;
			}
			bufOut[RowOffset+col]=OutStack[n];	// Write output
			col++;	nPixels++;					// Increase counters.
		}

	}	// while (main decompressor loop)

	delete [] OutStack;
	delete [] Suffix;
	delete [] Prefix;

	return whichBit;
}

void GifImage::LoadImage(const u8 * img, int imgSize)
{
	if(memcmp(img, "GIF", 3) != 0)
		return;

	int pos = 6;

	GIFLSDtag GifLSD;

	memcpy(&GifLSD, &img[pos], sizeof(GIFLSDtag));
	pos += sizeof(GIFLSDtag);

	int GlobalBPP = (GifLSD.PackedFields & 0x07) + 1;

	GifLSD.ScreenWidth = le16(GifLSD.ScreenWidth);
	GifLSD.ScreenHeight = le16(GifLSD.ScreenHeight);

	MainWidth = GifLSD.ScreenWidth;
	MainHeight = GifLSD.ScreenHeight;

	GIFCOLOR * GlobalColorMap = new (std::nothrow) GIFCOLOR[1 << GlobalBPP];
	if(!GlobalColorMap)
	{
		ShowError(tr("Not enough memory."));
		return;
	}

	if (GifLSD.PackedFields & 0x80)	// File has global color map?
	{
		for(int n = 0; n < (1 << GlobalBPP); n++)
		{
			GlobalColorMap[n].r = img[pos++];
			GlobalColorMap[n].g = img[pos++];
			GlobalColorMap[n].b = img[pos++];
		}
	}
	else	// GIF standard says to provide an internal default Palette:
	{
		for(int n = 0; n < 256; n++)
			GlobalColorMap[n].r = GlobalColorMap[n].g = GlobalColorMap[n].b = n;
	}

	int GraphicExtensionFound = 0;

	GIFGCEtag GifGCE;
	memset(&GifGCE, 0, sizeof(GIFGCEtag));

	do
	{
		int charGot = img[pos++];

		if (charGot == 0x21)
		{
			switch (img[pos++])
			{
				case 0xF9:
					memcpy(&GifGCE, &img[pos], sizeof(GIFGCEtag));
					GifGCE.Delay = le16(GifGCE.Delay);
					pos += sizeof(GIFGCEtag)+1; // Block Terminator (always 0)
					GraphicExtensionFound++;
					break;
				case 0xFE:
				case 0x01:
				case 0xFF:
				default:
					// read (and ignore) data sub-blocks
					while(int BlockLength = img[pos++])
					{
						for (int n = 0; n < BlockLength; n++)
							++pos;
					}
					break;
			}
		}
		else if (charGot == 0x2c)
		{
			GifFrame NextImage;
			GIFIDtag GifID;
			memcpy(&GifID, &img[pos], sizeof(GIFIDtag));
			pos += sizeof(GIFIDtag);

			GifID.xPos = le16(GifID.xPos);
			GifID.yPos = le16(GifID.yPos);
			GifID.Width = le16(GifID.Width);
			GifID.Height = le16(GifID.Height);

			NextImage.offsetx = GifID.xPos;
			NextImage.offsety = GifID.yPos;
			NextImage.width = ALIGN(GifID.Width);
			NextImage.height = ALIGN(GifID.Height);
			NextImage.Delay = GifGCE.Delay;
			NextImage.Disposal = (GifGCE.PackedFields & 0x1c) >> 2;
			NextImage.Transparent = (GifID.PackedFields & 0x01) != 0;
			NextImage.image = NULL;

			int LocalColorMap = (GifID.PackedFields & 0x08)? 1 : 0;
			int BPP = LocalColorMap ? (GifID.PackedFields & 7) + 1 : GlobalBPP;
			int BytesPerRow = GifID.Width;
			if(BPP == 24)
				BytesPerRow *= 3;

			BytesPerRow = ALIGN(BytesPerRow);

			u8 * Raster = new (std::nothrow) u8 [BytesPerRow*GifID.Height];
			if(!Raster)
			{
				ShowError(tr("Not enough memory."));
				break;
			}

			bool Transparent = false;
			int Transparency = -1;
			if (GraphicExtensionFound)
			{
				Transparent = (GifGCE.PackedFields & 0x01) ? true : false;
				Transparency = Transparent ? GifGCE.Transparent : -1;
			}

			GIFCOLOR * CurrentColorMap = new (std::nothrow) GIFCOLOR[sizeof(GIFCOLOR)*(1<<BPP)];
			if(!CurrentColorMap)
			{
				delete [] Raster;
				ShowError(tr("Not enough memory."));
				break;
			}

			if (LocalColorMap)
			{
				memcpy(CurrentColorMap, &img[pos], sizeof(GIFCOLOR)*(1<<BPP));
				pos += (sizeof(GIFCOLOR)-1)*(1 << BPP);
			}
			else
			{
				memcpy(CurrentColorMap, GlobalColorMap, sizeof(GIFCOLOR)*(1<<BPP));
			}

			short firstbyte = img[pos++];	// 1st byte of img block (CodeSize)

			// Calculate compressed image block size
			long ImgStart,ImgEnd;
			ImgEnd = ImgStart = pos;
			while(int n = img[pos++])
			{
				ImgEnd += n+1;
				pos = ImgEnd;
			}
			pos = ImgStart;

			char * pCompressedImage = new  (std::nothrow) char [ImgEnd-ImgStart+4];
			if(!pCompressedImage)
			{
				delete [] Raster;
				delete [] CurrentColorMap;
				ShowError(tr("Not enough memory."));
				break;
			}

			char * pTemp = pCompressedImage;
			while (int nBlockLength = img[pos++])
			{
				memcpy(pTemp, &img[pos], nBlockLength);
				pos += nBlockLength;
				pTemp+=nBlockLength;
			}

			int ret = LZWDecoder(pCompressedImage, (char *) Raster, firstbyte, BytesPerRow, GifID.Width,
							   GifID.Height, ((GifID.PackedFields & 0x40)?1:0));

			if (ret)
			{
				int len =  datasizeRGBA8(NextImage.width, NextImage.height);

				NextImage.image = (u8 *) memalign(32, len);

				if(!NextImage.image)
				{
					delete [] pCompressedImage;
					delete [] Raster;
					delete [] CurrentColorMap;
					ShowError(tr("Not enough memory."));
					break;
				}

				u32 offset;
				u8 r, g, b;

				for(int x = 0; x < NextImage.width; ++x)
				{
					for(int y = 0; y < NextImage.height; ++y)
					{
						offset = coordsRGBA8(x, y, NextImage.width);

						if(x < GifID.Width && y < GifID.Height)
						{
							if(BPP == 24)
							{
								u32 * pixel = (u32 *) &Raster[y*BytesPerRow+x*3];
								b = (*pixel & 0x00FF0000) >> 16;
								g = (*pixel & 0x0000FF00) >> 8;
								r = (*pixel & 0x000000FF);
							}
							else
							{
								b = Raster[y*BytesPerRow+x];
								g = Raster[y*BytesPerRow+x];
								r = Raster[y*BytesPerRow+x];
							}

							if(r == Transparency || g == Transparency || b == Transparency)
							{
								NextImage.image[offset] = 0;
								NextImage.image[offset+1] = 255;
								NextImage.image[offset+32] = 255;
								NextImage.image[offset+33] = 255;
							}
							else
							{
								NextImage.image[offset] = 255;
								NextImage.image[offset+1] = CurrentColorMap[r].r;
								NextImage.image[offset+32] = CurrentColorMap[g].g;
								NextImage.image[offset+33] = CurrentColorMap[b].b;
							}
						}
						else
						{
							NextImage.image[offset] = 0;
							NextImage.image[offset+1] = 255;
							NextImage.image[offset+32] = 255;
							NextImage.image[offset+33] = 255;
						}
					}
				}
				DCFlushRange(NextImage.image, len);

				Frames.push_back(NextImage);
			}

			delete [] pCompressedImage;
			GraphicExtensionFound = 0;
			delete [] Raster;
			delete [] CurrentColorMap;
		}
		else if (charGot == 0x3b)
			break; // Ok. Standard End.
	}
	while (pos < imgSize);

	delete [] GlobalColorMap;

}

void GifImage::Draw(int x, int y, int z, int degrees, float scaleX, float scaleY, int alpha, int minwidth, int maxwidth, int minheight, int maxheight)
{
	if(Frames.size() == 0)
		return;

	float OffX, OffY;

	for(u32 i = 0; i < RedrawQueue.size(); i++)
	{
		 //!Correcting scale position
		OffX = x+RedrawQueue[i].offsetx*scaleX+(RedrawQueue[i].width*scaleX-RedrawQueue[i].width)/2.0f-(MainWidth*scaleX-MainWidth)/2.0f;
		OffY = y+RedrawQueue[i].offsety*scaleY+(RedrawQueue[i].height*scaleY-RedrawQueue[i].height)/2.0f-(MainHeight*scaleY-MainHeight)/2.0f;

		Menu_DrawImgCut(RedrawQueue[i].image, RedrawQueue[i].width, RedrawQueue[i].height,
						GX_TF_RGBA8, OffX, OffY, z, degrees, scaleX, scaleY, alpha, minwidth,
						maxwidth, minheight, maxheight);
	}

	//!Correcting scale position
	OffX = x+Frames[currentFrame].offsetx*scaleX+(Frames[currentFrame].width*scaleX-Frames[currentFrame].width)/2.0f-(MainWidth*scaleX-MainWidth)/2.0f;
	OffY = y+Frames[currentFrame].offsety*scaleY+(Frames[currentFrame].height*scaleY-Frames[currentFrame].height)/2.0f-(MainHeight*scaleY-MainHeight)/2.0f;

	Menu_DrawImgCut(Frames[currentFrame].image, Frames[currentFrame].width, Frames[currentFrame].height,
					GX_TF_RGBA8, OffX, OffY, z, degrees, scaleX, scaleY, alpha, minwidth, maxwidth, minheight,
					maxheight);

	if(DelayTimer.elapsed()-lastTimer >= Frames[currentFrame].Delay/100.0f)
	{
		if((Frames[currentFrame].Disposal == 0 && !Frames[currentFrame].Transparent) ||
			Frames[currentFrame].Disposal == 1)
		{
			RedrawQueue.push_back(Frames[currentFrame]);
		}
		else if(Frames[currentFrame].Disposal == 2 && RedrawQueue.size() > 0)
		{
			RedrawQueue.pop_back();
		}

		++currentFrame;

		lastTimer = DelayTimer.elapsed();

		if(currentFrame >= (int) Frames.size())
		{
			currentFrame = 0;
			RedrawQueue.clear();
		}
	}
}
