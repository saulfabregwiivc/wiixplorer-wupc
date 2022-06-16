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
#include "FreeTypeGX.h"
#include "Memory/mem2.h"

extern const u8 font_ttf[];
extern const u32 font_ttf_size;

FreeTypeGX * fontSystem = NULL;
static FT_Byte * MainFont = (FT_Byte *) font_ttf;
static u32 MainFontSize = font_ttf_size;

extern "C"
{
	//!Default fallback font for PDFs
	void SetupPDFFallbackFont(const u8 * font, int size);
}

void ClearFontData()
{
	if(fontSystem)
		delete fontSystem;
	fontSystem = NULL;

	if(MainFont != (FT_Byte *) font_ttf)
	{
		if(MainFont != NULL)
			MEM2_free(MainFont);
		MainFont = (FT_Byte *) font_ttf;
		MainFontSize = font_ttf_size;
	}
}

bool SetupDefaultFont(const char *path)
{
	bool result = false;
	FILE *pfile = NULL;

	ClearFontData();

	if(path)
		pfile = fopen(path, "rb");

	if(pfile)
	{
		fseek(pfile, 0, SEEK_END);
		MainFontSize = ftell(pfile);
		rewind(pfile);

		MainFont = (FT_Byte *) MEM2_alloc(MainFontSize);
		if(!MainFont)
		{
			MainFont = (FT_Byte *) font_ttf;
			MainFontSize = font_ttf_size;
		}
		else
		{
			fread(MainFont, 1, MainFontSize, pfile);
			result = true;
		}
		fclose(pfile);
	}

	SetupPDFFallbackFont(MainFont, MainFontSize);

	fontSystem = new FreeTypeGX(MainFont, MainFontSize);

	return result;
}
