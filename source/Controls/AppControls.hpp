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
#ifndef APP_CONTROLS_HPP_
#define APP_CONTROLS_HPP_

#include <gccore.h>
#include <string>

typedef struct _Controls
{
	u16	 ClickButton;
	u16	 BackButton;
	u16	 UpButton;
	u16	 DownButton;
	u16	 LeftButton;
	u16	 RightButton;
	u16	 ContextMenuButton;
	u16	 MarkItemButton;
	u16	 DeMarkItemButton;
	u16	 DeMarkAllButton;
	u16	 HomeButton;
	u16	 EditTextLine;
	u16	 SlideShowButton;
	u16	 KeyShiftButton;
	u16	 KeyBackspaceButton;
	u16	 ZoomIn;
	u16	 ZoomOut;
	u16	 UpInDirectory;
	u16	 OneButtonScroll;
} ControlItems;

class AppControls
{
	public:
		AppControls();
		~AppControls();
		void SetDefault();
		bool Load(const char * filepath);
		bool Save();
		bool SetControl(char *name, char *value);

		ControlItems WiiControls;
		ControlItems ClassicControls;
		ControlItems GCControls;
		u16 ScreenshotHoldButton;
		u16 ScreenshotClickButton;

		static std::string ControlButtonsToString(int type, u32 button);

		enum
		{
			TypeWiiMote,
			TypeWiiClassic,
			TypeWiiUPro,
			TypeGCPad,
		};
	protected:
		char FilePath[150];
		void ParseLine(char *line);
		void TrimLine(char *dest, char *src, int size);
};

#endif
