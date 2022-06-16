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
#ifndef COLORSETPROMPT_H
#define COLORSETPROMPT_H

#include "Prompts/PromptWindows.h"
#include "GUI/gui_arrowoption.h"

class ColorSetPrompt : public PromptWindow
{
	public:
		ColorSetPrompt(const char * title, GXColor * c, int pos);
		static void Show(const char * title, GXColor * c, int pos);
	protected:
		void UpdateOptionValues();
		void OnOptionLeftClick(GuiElement *sender, int pointer, const POINT &p);
		void OnOptionRightClick(GuiElement *sender, int pointer, const POINT &p);
		void OnOptionButtonClick(GuiElement *sender, int pointer, const POINT &p);

		int ColorPos;
		int ColorChange;
		GXColor OrigColor[4];
		GXColor * color;
		GuiArrowOption arrowOption;
};

#endif
