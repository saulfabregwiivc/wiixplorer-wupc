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
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiiuse/wpad.h>
#include <ogc/pad.h>
#include "AppControls.hpp"
#include "FileOperations/fileops.h"
#include "Language/gettext.h"

static inline const char * GetWPAD_ButtonName(u32 button)
{
	switch(button)
	{
		case 0x0000:
			return tr("WPAD NONE BUTTON");
		case WPAD_BUTTON_2:
			return tr("WPAD BUTTON 2");
		case WPAD_BUTTON_1:
			return tr("WPAD BUTTON 1");
		case WPAD_BUTTON_B:
			return tr("WPAD BUTTON B");
		case WPAD_BUTTON_A:
			return tr("WPAD BUTTON A");
		case WPAD_BUTTON_MINUS:
			return tr("WPAD BUTTON MINUS");
		case WPAD_BUTTON_HOME:
			return tr("WPAD BUTTON HOME");
		case WPAD_BUTTON_LEFT:
			return tr("WPAD BUTTON LEFT");
		case WPAD_BUTTON_RIGHT:
			return tr("WPAD BUTTON RIGHT");
		case WPAD_BUTTON_DOWN:
			return tr("WPAD BUTTON DOWN");
		case WPAD_BUTTON_UP:
			return tr("WPAD BUTTON UP");
		case WPAD_BUTTON_PLUS:
			return tr("WPAD BUTTON PLUS");
		case WPAD_NUNCHUK_BUTTON_Z:
			return tr("WPAD NUNCHUCK BUTTON Z");
		case WPAD_NUNCHUK_BUTTON_C:
			return tr("WPAD NUNCHUCK BUTTON C");
		default:
			return tr("Unknown");
	}
}

static inline const char * GetClassic_ButtonName(u32 button)
{
	switch(button)
	{
		case 0x0000:
			return tr("WPAD CLASSIC NONE BUTTON");
		case WPAD_CLASSIC_BUTTON_UP:
			return tr("WPAD CLASSIC BUTTON UP");
		case WPAD_CLASSIC_BUTTON_LEFT:
			return tr("WPAD CLASSIC BUTTON LEFT");
		case WPAD_CLASSIC_BUTTON_ZR:
			return tr("WPAD CLASSIC BUTTON ZR");
		case WPAD_CLASSIC_BUTTON_X:
			return tr("WPAD CLASSIC BUTTON X");
		case WPAD_CLASSIC_BUTTON_A:
			return tr("WPAD CLASSIC BUTTON A");
		case WPAD_CLASSIC_BUTTON_Y:
			return tr("WPAD CLASSIC BUTTON Y");
		case WPAD_CLASSIC_BUTTON_B:
			return tr("WPAD CLASSIC BUTTON B");
		case WPAD_CLASSIC_BUTTON_ZL:
			return tr("WPAD CLASSIC BUTTON ZL");
		case WPAD_CLASSIC_BUTTON_FULL_R:
			return tr("WPAD CLASSIC BUTTON FULL R");
		case WPAD_CLASSIC_BUTTON_PLUS:
			return tr("WPAD CLASSIC BUTTON PLUS");
		case WPAD_CLASSIC_BUTTON_HOME:
			return tr("WPAD CLASSIC BUTTON HOME");
		case WPAD_CLASSIC_BUTTON_MINUS:
			return tr("WPAD CLASSIC BUTTON MINUS");
		case WPAD_CLASSIC_BUTTON_FULL_L:
			return tr("WPAD CLASSIC BUTTON FULL L");
		case WPAD_CLASSIC_BUTTON_DOWN:
			return tr("WPAD CLASSIC BUTTON DOWN");
		case WPAD_CLASSIC_BUTTON_RIGHT:
			return tr("WPAD CLASSIC BUTTON RIGHT");
		default:
			return tr("Unknown");
	}
}

static inline const char * GetPAD_ButtonName(u32 button)
{
	switch(button)
	{
		case 0x0000:
			return tr("GC PAD BUTTON NONE");
		case PAD_BUTTON_LEFT:
			return tr("GC PAD BUTTON LEFT");
		case PAD_BUTTON_RIGHT:
			return tr("GC PAD BUTTON RIGHT");
		case PAD_BUTTON_DOWN:
			return tr("GC PAD BUTTON DOWN");
		case PAD_BUTTON_UP:
			return tr("GC PAD BUTTON UP");
		case PAD_TRIGGER_Z:
			return tr("GC PAD TRIGGER Z");
		case PAD_TRIGGER_R:
			return tr("GC PAD TRIGGER R");
		case PAD_TRIGGER_L:
			return tr("GC PAD TRIGGER L");
		case PAD_BUTTON_A:
			return tr("GC PAD BUTTON A");
		case PAD_BUTTON_B:
			return tr("GC PAD BUTTON B");
		case PAD_BUTTON_X:
			return tr("GC PAD BUTTON X");
		case PAD_BUTTON_Y:
			return tr("GC PAD BUTTON Y");
		case PAD_BUTTON_START:
			return tr("GC PAD BUTTON START");
		default:
			return tr("Unknown");
	}
}

AppControls::AppControls()
{
	SetDefault();
}

AppControls::~AppControls()
{
}

std::string AppControls::ControlButtonsToString(int type, u32 button)
{
	std::string controlText;

	if(button == 0)
	{
		if(type == TypeWiiMote)
			controlText = GetWPAD_ButtonName( button );

		else if(type == TypeWiiClassic)
			controlText = GetClassic_ButtonName( button );

		else if(type == TypeGCPad)
			controlText = GetPAD_ButtonName( button );

		return controlText;
	}

	for(int i = 0; i < 32; i++)
	{
		u32 Bit = (1 << i);

		if(button & Bit)
		{
			if(!controlText.empty())
				controlText += " + ";

			if(type == TypeWiiMote)
				controlText += GetWPAD_ButtonName( Bit );

			else if(type == TypeWiiClassic)
				controlText += GetClassic_ButtonName( Bit );

			else if(type == TypeGCPad)
				controlText += GetPAD_ButtonName( Bit );
		}
	}

	return controlText;
}

void AppControls::SetDefault()
{
	ScreenshotHoldButton = WPAD_BUTTON_1;
	ScreenshotClickButton = WPAD_BUTTON_B;

	WiiControls.ClickButton = WPAD_BUTTON_A;
	WiiControls.BackButton = WPAD_BUTTON_B;
	WiiControls.UpButton = WPAD_BUTTON_UP;
	WiiControls.DownButton = WPAD_BUTTON_DOWN;
	WiiControls.LeftButton = WPAD_BUTTON_LEFT;
	WiiControls.RightButton = WPAD_BUTTON_RIGHT;
	WiiControls.ContextMenuButton = WPAD_BUTTON_PLUS;
	WiiControls.MarkItemButton = WPAD_BUTTON_1;
	WiiControls.DeMarkItemButton = WPAD_BUTTON_2;
	WiiControls.DeMarkAllButton = WPAD_BUTTON_MINUS;
	WiiControls.HomeButton = WPAD_BUTTON_HOME;
	WiiControls.EditTextLine = WPAD_BUTTON_PLUS;
	WiiControls.SlideShowButton = WPAD_BUTTON_1;
	WiiControls.KeyShiftButton = 0x0000;
	WiiControls.KeyBackspaceButton = WPAD_BUTTON_B;
	WiiControls.ZoomIn = WPAD_BUTTON_PLUS;
	WiiControls.ZoomOut = WPAD_BUTTON_MINUS;
	WiiControls.UpInDirectory = WPAD_BUTTON_B;
	WiiControls.OneButtonScroll = 0x0000;

	ClassicControls.ClickButton = WPAD_CLASSIC_BUTTON_A >> 16;
	ClassicControls.BackButton = WPAD_CLASSIC_BUTTON_B >> 16;
	ClassicControls.UpButton = WPAD_CLASSIC_BUTTON_UP >> 16;
	ClassicControls.DownButton = WPAD_CLASSIC_BUTTON_DOWN >> 16;
	ClassicControls.LeftButton = WPAD_CLASSIC_BUTTON_LEFT >> 16;
	ClassicControls.RightButton = WPAD_CLASSIC_BUTTON_RIGHT >> 16;
	ClassicControls.ContextMenuButton = WPAD_CLASSIC_BUTTON_ZR >> 16;
	ClassicControls.MarkItemButton = WPAD_CLASSIC_BUTTON_PLUS >> 16;
	ClassicControls.DeMarkItemButton = WPAD_CLASSIC_BUTTON_MINUS >> 16;
	ClassicControls.DeMarkAllButton = WPAD_CLASSIC_BUTTON_B >> 16;
	ClassicControls.HomeButton = WPAD_CLASSIC_BUTTON_HOME >> 16;
	ClassicControls.EditTextLine = WPAD_CLASSIC_BUTTON_PLUS >> 16;
	ClassicControls.SlideShowButton = WPAD_CLASSIC_BUTTON_X >> 16;
	ClassicControls.KeyShiftButton = 0x0000;
	ClassicControls.KeyBackspaceButton = WPAD_CLASSIC_BUTTON_B >> 16;
	ClassicControls.ZoomIn = WPAD_CLASSIC_BUTTON_PLUS >> 16;
	ClassicControls.ZoomOut = WPAD_CLASSIC_BUTTON_MINUS >> 16;
	ClassicControls.UpInDirectory = WPAD_CLASSIC_BUTTON_B >> 16;
	ClassicControls.OneButtonScroll = 0x0000;

	GCControls.ClickButton = PAD_BUTTON_A;
	GCControls.BackButton = PAD_BUTTON_B;
	GCControls.UpButton = PAD_BUTTON_UP;
	GCControls.DownButton = PAD_BUTTON_DOWN;
	GCControls.LeftButton = PAD_BUTTON_LEFT;
	GCControls.RightButton = PAD_BUTTON_RIGHT;
	GCControls.ContextMenuButton = PAD_TRIGGER_Z;
	GCControls.MarkItemButton = PAD_TRIGGER_R;
	GCControls.DeMarkItemButton = PAD_TRIGGER_L;
	GCControls.DeMarkAllButton = PAD_BUTTON_B;
	GCControls.HomeButton = PAD_BUTTON_MENU;
	GCControls.EditTextLine = PAD_TRIGGER_Z;
	GCControls.SlideShowButton = PAD_BUTTON_X;
	GCControls.KeyShiftButton = 0x0000;
	GCControls.KeyBackspaceButton = PAD_BUTTON_B;
	GCControls.ZoomIn = PAD_TRIGGER_R;
	GCControls.ZoomOut = PAD_TRIGGER_L;
	GCControls.UpInDirectory = PAD_BUTTON_B;
	GCControls.OneButtonScroll = 0x0000;
}

bool AppControls::Load(const char * filepath)
{
	if(!filepath)
		return false;

	char line[1024];
	snprintf(FilePath, sizeof(FilePath), "%s/WiiXplorer_Controls.cfg", filepath);

	if(!CheckFile(FilePath))
		return false;

	FILE * file = fopen(FilePath, "r");
	if (!file)
	{
		fclose(file);
		return false;
	}

	while (fgets(line, sizeof(line), file))
	{
		if (line[0] == '#') continue;

		this->ParseLine(line);
	}
	fclose(file);

	return true;
}

bool AppControls::Save()
{
	char destpath[1024];
	snprintf(destpath, sizeof(destpath), "%s", FilePath);

	char * ptr = strrchr(destpath, '/');
	if(ptr)
	{
		ptr++;
		ptr[0] = '\0';
	}

	if(!CreateSubfolder(destpath))
		return false;

	FILE * file = fopen(FilePath, "w");
	if(!file)
	{
		fclose(file);
		return false;
	}

	fprintf(file, "# WiiXplorer Controls\n");
	fprintf(file, "# Note: This file is automatically generated\n\n");
	fprintf(file, "# Wii Controls\n\n");
	fprintf(file, "WiiControls.ClickButton = %d\n", WiiControls.ClickButton);
	fprintf(file, "WiiControls.BackButton = %d\n", WiiControls.BackButton);
	fprintf(file, "WiiControls.UpButton = %d\n", WiiControls.UpButton);
	fprintf(file, "WiiControls.DownButton = %d\n", WiiControls.DownButton);
	fprintf(file, "WiiControls.LeftButton = %d\n", WiiControls.LeftButton);
	fprintf(file, "WiiControls.RightButton = %d\n", WiiControls.RightButton);
	fprintf(file, "WiiControls.ContextMenuButton = %d\n", WiiControls.ContextMenuButton);
	fprintf(file, "WiiControls.MarkItemButton = %d\n", WiiControls.MarkItemButton);
	fprintf(file, "WiiControls.DeMarkItemButton = %d\n", WiiControls.DeMarkItemButton);
	fprintf(file, "WiiControls.DeMarkAllButton = %d\n", WiiControls.DeMarkAllButton);
	fprintf(file, "WiiControls.HomeButton = %d\n", WiiControls.HomeButton);
	fprintf(file, "WiiControls.EditTextLine = %d\n", WiiControls.EditTextLine);
	fprintf(file, "WiiControls.SlideShowButton = %d\n", WiiControls.SlideShowButton);
	fprintf(file, "WiiControls.KeyShiftButton = %d\n", WiiControls.KeyShiftButton);
	fprintf(file, "WiiControls.KeyBackspaceButton = %d\n", WiiControls.KeyBackspaceButton);
	fprintf(file, "WiiControls.ZoomIn = %d\n", WiiControls.ZoomIn);
	fprintf(file, "WiiControls.ZoomOut = %d\n", WiiControls.ZoomOut);
	fprintf(file, "WiiControls.UpInDirectory = %d\n", WiiControls.UpInDirectory);

	fprintf(file, "\n# Wii Classic Controls\n\n");
	fprintf(file, "ClassicControls.ClickButton = %d\n", ClassicControls.ClickButton);
	fprintf(file, "ClassicControls.BackButton = %d\n", ClassicControls.BackButton);
	fprintf(file, "ClassicControls.UpButton = %d\n", ClassicControls.UpButton);
	fprintf(file, "ClassicControls.DownButton = %d\n", ClassicControls.DownButton);
	fprintf(file, "ClassicControls.LeftButton = %d\n", ClassicControls.LeftButton);
	fprintf(file, "ClassicControls.RightButton = %d\n", ClassicControls.RightButton);
	fprintf(file, "ClassicControls.ContextMenuButton = %d\n", ClassicControls.ContextMenuButton);
	fprintf(file, "ClassicControls.MarkItemButton = %d\n", ClassicControls.MarkItemButton);
	fprintf(file, "ClassicControls.DeMarkItemButton = %d\n", ClassicControls.DeMarkItemButton);
	fprintf(file, "ClassicControls.DeMarkAllButton = %d\n", ClassicControls.DeMarkAllButton);
	fprintf(file, "ClassicControls.HomeButton = %d\n", ClassicControls.HomeButton);
	fprintf(file, "ClassicControls.EditTextLine = %d\n", ClassicControls.EditTextLine);
	fprintf(file, "ClassicControls.SlideShowButton = %d\n", ClassicControls.SlideShowButton);
	fprintf(file, "ClassicControls.KeyShiftButton = %d\n", ClassicControls.KeyShiftButton);
	fprintf(file, "ClassicControls.KeyBackspaceButton = %d\n", ClassicControls.KeyBackspaceButton);
	fprintf(file, "ClassicControls.ZoomIn = %d\n", ClassicControls.ZoomIn);
	fprintf(file, "ClassicControls.ZoomOut = %d\n", ClassicControls.ZoomOut);
	fprintf(file, "ClassicControls.UpInDirectory = %d\n", ClassicControls.UpInDirectory);

	fprintf(file, "\n# GC Controls\n\n");
	fprintf(file, "GCControls.ClickButton = %d\n", GCControls.ClickButton);
	fprintf(file, "GCControls.BackButton = %d\n", GCControls.BackButton);
	fprintf(file, "GCControls.UpButton = %d\n", GCControls.UpButton);
	fprintf(file, "GCControls.DownButton = %d\n", GCControls.DownButton);
	fprintf(file, "GCControls.LeftButton = %d\n", GCControls.LeftButton);
	fprintf(file, "GCControls.RightButton = %d\n", GCControls.RightButton);
	fprintf(file, "GCControls.ContextMenuButton = %d\n", GCControls.ContextMenuButton);
	fprintf(file, "GCControls.MarkItemButton = %d\n", GCControls.MarkItemButton);
	fprintf(file, "GCControls.DeMarkItemButton = %d\n", GCControls.DeMarkItemButton);
	fprintf(file, "GCControls.DeMarkAllButton = %d\n", GCControls.DeMarkAllButton);
	fprintf(file, "GCControls.HomeButton = %d\n", GCControls.HomeButton);
	fprintf(file, "GCControls.EditTextLine = %d\n", GCControls.EditTextLine);
	fprintf(file, "GCControls.SlideShowButton = %d\n", GCControls.SlideShowButton);
	fprintf(file, "GCControls.KeyShiftButton = %d\n", GCControls.KeyShiftButton);
	fprintf(file, "GCControls.KeyBackspaceButton = %d\n", GCControls.KeyBackspaceButton);
	fprintf(file, "GCControls.ZoomIn = %d\n", GCControls.ZoomIn);
	fprintf(file, "GCControls.ZoomOut = %d\n", GCControls.ZoomOut);
	fprintf(file, "GCControls.UpInDirectory = %d\n", GCControls.UpInDirectory);

	fprintf(file, "\n# Screenshot combination buttons\n\n");
	fprintf(file, "ScreenshotHoldButton = %d\n", ScreenshotHoldButton);
	fprintf(file, "ScreenshotClickButton = %d\n", ScreenshotClickButton);

	fclose(file);

	return true;
}

bool AppControls::SetControl(char *name, char *value)
{
	int i = 0;

	if (strcmp(name, "ScreenshotHoldButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ScreenshotHoldButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ScreenshotClickButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ScreenshotClickButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.ClickButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.ClickButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.BackButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.BackButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.UpButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.UpButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.DownButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.DownButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.LeftButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.LeftButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.RightButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.RightButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.ContextMenuButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.ContextMenuButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.MarkItemButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.MarkItemButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.DeMarkItemButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.DeMarkItemButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.DeMarkAllButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.DeMarkAllButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.HomeButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.HomeButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.EditTextLine") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.EditTextLine = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.SlideShowButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.SlideShowButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.KeyShiftButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.KeyShiftButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.KeyBackspaceButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.KeyBackspaceButton = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.ZoomIn") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.ZoomIn = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.ZoomOut") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.ZoomOut = i;
		}
		return true;
	}
	else if (strcmp(name, "WiiControls.UpInDirectory") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			WiiControls.UpInDirectory = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.ClickButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.ClickButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.BackButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.BackButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.UpButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.UpButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.DownButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.DownButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.LeftButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.LeftButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.RightButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.RightButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.ContextMenuButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.ContextMenuButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.MarkItemButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.MarkItemButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.DeMarkItemButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.DeMarkItemButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.DeMarkAllButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.DeMarkAllButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.HomeButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.HomeButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.EditTextLine") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.EditTextLine = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.SlideShowButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.SlideShowButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.KeyShiftButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.KeyShiftButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.KeyBackspaceButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.KeyBackspaceButton = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.ZoomIn") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.ZoomIn = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.ZoomOut") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.ZoomOut = i;
		}
		return true;
	}
	else if (strcmp(name, "ClassicControls.UpInDirectory") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			ClassicControls.UpInDirectory = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.ClickButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.ClickButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.BackButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.BackButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.UpButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.UpButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.DownButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.DownButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.LeftButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.LeftButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.RightButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.RightButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.ContextMenuButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.ContextMenuButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.MarkItemButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.MarkItemButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.DeMarkItemButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.DeMarkItemButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.DeMarkAllButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.DeMarkAllButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.HomeButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.HomeButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.EditTextLine") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.EditTextLine = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.SlideShowButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.SlideShowButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.KeyShiftButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.KeyShiftButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.KeyBackspaceButton") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.KeyBackspaceButton = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.ZoomIn") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.ZoomIn = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.ZoomOut") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.ZoomOut = i;
		}
		return true;
	}
	else if (strcmp(name, "GCControls.UpInDirectory") == 0) {
		if (sscanf(value, "%d", &i) == 1) {
			GCControls.UpInDirectory = i;
		}
		return true;
	}

	return false;
}

void AppControls::ParseLine(char *line)
{
	char temp[200], name[200], value[200];

	strncpy(temp, line, sizeof(temp));

	char * eq = strchr(temp, '=');

	if(!eq) return;

	*eq = 0;

	this->TrimLine(name, temp, sizeof(name));
	this->TrimLine(value, eq+1, sizeof(value));

	this->SetControl(name, value);
}

void AppControls::TrimLine(char *dest, char *src, int size)
{
	int len;
	while (*src == ' ') src++;
	len = strlen(src);
	while (len > 0 && strchr(" \r\n", src[len-1])) len--;
	if (len >= size) len = size-1;
	strncpy(dest, src, len);
	dest[len] = 0;
}
