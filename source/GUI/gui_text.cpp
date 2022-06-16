/****************************************************************************
 * Copyright (C) 2009-2011 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include "gui_text.h"
#include "FreeTypeGX.h"
#include "TextOperations/wstring.hpp"
#include "VideoOperations/video.h"
#include "Tools/StringTools.h"
#include "TextOperations/wstring.hpp"

extern FreeTypeGX * fontSystem;

static int presetSize = 18;
static int presetMaxWidth = 0xFFFF;
static int presetAlignment = ALIGN_CENTER | ALIGN_MIDDLE;
static GXColor presetColor = (GXColor){255, 255, 255, 255};

#define TEXT_SCROLL_DELAY			5
#define	TEXT_SCROLL_INITIAL_DELAY	8

/**
 * Constructor for the GuiText class.
 */

GuiText::GuiText(const char * t, int s, GXColor c)
{
	text = NULL;
	size = s;
	currentSize = size;
	color = c;
	alpha = c.a;
	alignment = ALIGN_CENTER | ALIGN_MIDDLE;
	maxWidth = presetMaxWidth;
	wrapMode = 0;
	textWidth = 0;
	font = NULL;
	linestodraw = MAX_LINES_TO_DRAW;
	textScrollPos = 0;
	textScrollInitialDelay = TEXT_SCROLL_INITIAL_DELAY;
	textScrollDelay = TEXT_SCROLL_DELAY;

	if(t)
	{
		text = charToWideChar(t);
		if(!text)
			return;

		textWidth = fontSystem->getWidth(text, currentSize);
	}
}

GuiText::GuiText(const wchar_t * t, int s, GXColor c)
{
	text = NULL;
	size = s;
	currentSize = size;
	color = c;
	alpha = c.a;
	alignment = ALIGN_CENTER | ALIGN_MIDDLE;
	maxWidth = presetMaxWidth;
	wrapMode = 0;
	textWidth = 0;
	font = NULL;
	linestodraw = MAX_LINES_TO_DRAW;
	textScrollPos = 0;
	textScrollInitialDelay = TEXT_SCROLL_INITIAL_DELAY;
	textScrollDelay = TEXT_SCROLL_DELAY;

	if(t)
	{
		text = new (std::nothrow) wchar_t[wcslen(t)+1];
		if(!text)
			return;

		wcscpy(text, t);

		textWidth = fontSystem->getWidth(text, currentSize);
	}
}

/**
 * Constructor for the GuiText class, uses presets
 */
GuiText::GuiText(const char * t)
{
	text = NULL;
	size = presetSize;
	currentSize = size;
	color = presetColor;
	alpha = presetColor.a;
	alignment = presetAlignment;
	maxWidth = presetMaxWidth;
	wrapMode = 0;
	textWidth = 0;
	font = NULL;
	linestodraw = MAX_LINES_TO_DRAW;
	textScrollPos = 0;
	textScrollInitialDelay = TEXT_SCROLL_INITIAL_DELAY;
	textScrollDelay = TEXT_SCROLL_DELAY;

	if(t)
	{
		text = charToWideChar(t);
		if(!text)
			return;

		textWidth = fontSystem->getWidth(text, currentSize);
	}
}


/**
 * Destructor for the GuiText class.
 */
GuiText::~GuiText()
{
	if(text)
		delete [] text;
	text = NULL;

	if(font)
	{
		delete font;
		font = NULL;
	}

	ClearDynamicText();
}

void GuiText::SetText(const char * t)
{
	if(text)
		delete [] text;
	text = NULL;

	ClearDynamicText();

	textScrollPos = 0;
	textScrollInitialDelay = TEXT_SCROLL_INITIAL_DELAY;

	if(t)
	{
		text = charToWideChar(t);
		if(!text)
			return;

		textWidth = fontSystem->getWidth(text, currentSize);
	}
}

void GuiText::SetTextf(const char *format, ...)
{
	if(!format)
		SetText((char *) NULL);

	char *tmp=0;
	va_list va;
	va_start(va, format);
	if((vasprintf(&tmp, format, va)>=0) && tmp)
	{
		SetText(tmp);
	}
	va_end(va);

	if(tmp)
		free(tmp);
}


void GuiText::SetText(const wchar_t * t)
{
	if(text)
		delete [] text;
	text = NULL;

	ClearDynamicText();

	textScrollPos = 0;
	textScrollInitialDelay = TEXT_SCROLL_INITIAL_DELAY;

	if(t)
	{
		text = new (std::nothrow) wchar_t[wcslen(t)+1];
		if(!text)
			return;

		wcscpy(text, t);

		textWidth = fontSystem->getWidth(text, currentSize);
	}
}

void GuiText::ClearDynamicText()
{
	for(u32 i = 0; i < textDyn.size(); i++)
	{
		if(textDyn[i])
			delete [] textDyn[i];
	}
	textDyn.clear();
}

void GuiText::SetPresets(int sz, GXColor c, int w, int a)
{
	presetSize = sz;
	presetColor = c;
	presetMaxWidth = w;
	presetAlignment = a;
}

void GuiText::SetFontSize(int s)
{
	size = s;
}

void GuiText::SetMaxWidth(int width, int w)
{
	maxWidth = width;
	wrapMode = w;

	if(w == SCROLL_HORIZONTAL)
	{
		textScrollPos = 0;
		textScrollInitialDelay = TEXT_SCROLL_INITIAL_DELAY;
		textScrollDelay = TEXT_SCROLL_DELAY;
	}

	ClearDynamicText();
}

void GuiText::SetColor(GXColor c)
{
	color = c;
	alpha = c.a;
}

int GuiText::GetTextWidth(int ind)
{
	if(ind < 0 || ind >= (int) textDyn.size())
		return this->GetTextWidth();

	return fontSystem->getWidth(textDyn[ind], currentSize);
}

const wchar_t * GuiText::GetDynText(int ind)
{
	if(ind < 0 || ind >= (int) textDyn.size())
		return text;

	return textDyn[ind];
}

/**
 * Change font
 */
bool GuiText::SetFont(const u8 *fontbuffer, const u32 filesize)
{
	if(!fontbuffer || !filesize)
		return false;

	if(font)
	{
		delete font;
		font = NULL;
	}
	font = new FreeTypeGX(fontbuffer, filesize);
	textWidth = font->getWidth(text, currentSize);

	return true;
}

std::string GuiText::toUTF8(void) const
{
	if(!text)
		return std::string();

	return wString(text).toUTF8();
}

void GuiText::MakeDottedText()
{
	int pos = textDyn.size();
	textDyn.resize(pos + 1);

	int i = 0, currentWidth = 0;
	textDyn[pos] = new (std::nothrow) wchar_t[maxWidth];
	if(!textDyn[pos]) {
		textDyn.resize(pos);
		return;
	}

	while (text[i])
	{
		currentWidth += (font ? font : fontSystem)->getCharWidth(text[i], currentSize, i > 0 ? text[i - 1] : 0);
		if (currentWidth >= maxWidth && i > 2)
		{
			textDyn[pos][i - 2] = '.';
			textDyn[pos][i - 1] = '.';
			textDyn[pos][i] = '.';
			i++;
			break;
		}

		textDyn[pos][i] = text[i];

		i++;
	}
	textDyn[pos][i] = 0;
}

void GuiText::ScrollText()
{
	if (textDyn.size() == 0)
	{
		int pos = textDyn.size();
		int i = 0, currentWidth = 0;
		textDyn.resize(pos + 1);

		textDyn[pos] = new (std::nothrow) wchar_t[maxWidth];
		if(!textDyn[pos]) {
			textDyn.resize(pos);
			return;
		}

		while (text[i] && currentWidth < maxWidth)
		{
			textDyn[pos][i] = text[i];

			currentWidth += (font ? font : fontSystem)->getCharWidth(text[i], currentSize, i > 0 ? text[i - 1] : 0);

			++i;
		}
		textDyn[pos][i] = 0;

		return;
	}

	if (frameCount % textScrollDelay != 0)
	{
		return;
	}

	if (textScrollInitialDelay)
	{
		--textScrollInitialDelay;
		return;
	}

	int stringlen = wcslen(text);

	++textScrollPos;
	if (textScrollPos > stringlen)
	{
		textScrollPos = 0;
		textScrollInitialDelay = TEXT_SCROLL_INITIAL_DELAY;
	}

	int ch = textScrollPos;
	int pos = textDyn.size() - 1;

	if (!textDyn[pos])
		textDyn[pos] = new (std::nothrow) wchar_t[maxWidth];

	if(!textDyn[pos]) {
		textDyn.resize(pos);
		return;
	}

	int i = 0, currentWidth = 0;

	while (currentWidth < maxWidth)
	{
		if (ch > stringlen - 1)
		{
			textDyn[pos][i++] = ' ';
			currentWidth += (font ? font : fontSystem)->getCharWidth(L' ', currentSize, ch > 0 ? text[ch - 1] : 0);
			textDyn[pos][i++] = ' ';
			currentWidth += (font ? font : fontSystem)->getCharWidth(L' ', currentSize, L' ');
			textDyn[pos][i++] = ' ';
			currentWidth += (font ? font : fontSystem)->getCharWidth(L' ', currentSize, L' ');
			ch = 0;

			if(currentWidth >= maxWidth)
				break;
		}

		textDyn[pos][i] = text[ch];
		currentWidth += (font ? font : fontSystem)->getCharWidth(text[ch], currentSize, ch > 0 ? text[ch - 1] : 0);
		++ch;
		++i;
	}
	textDyn[pos][i] = 0;
}

void GuiText::WrapText()
{
	if (textDyn.size() > 0) return;

	int i = 0;
	int ch = 0;
	int linenum = 0;
	int lastSpace = -1;
	int lastSpaceIndex = -1;
	int currentWidth = 0;

	while (text[ch] && linenum < linestodraw)
	{
		if (linenum >= (int) textDyn.size())
		{
			textDyn.resize(linenum + 1);
			textDyn[linenum] = new (std::nothrow) wchar_t[maxWidth];
			if(!textDyn[linenum]) {
				textDyn.resize(linenum);
				break;
			}
		}

		textDyn[linenum][i] = text[ch];
		textDyn[linenum][i + 1] = 0;

		currentWidth += (font ? font : fontSystem)->getCharWidth(text[ch], currentSize, ch > 0 ? text[ch - 1] : 0x0000);

		if (currentWidth >= maxWidth)
		{
			if (lastSpace >= 0)
			{
				textDyn[linenum][lastSpaceIndex] = 0; // discard space, and everything after
				ch = lastSpace; // go backwards to the last space
				lastSpace = -1; // we have used this space
				lastSpaceIndex = -1;
			}

			if (linenum + 1 == linestodraw && text[ch + 1] != 0x0000)
			{
				textDyn[linenum][i - 2] = '.';
				textDyn[linenum][i - 1] = '.';
				textDyn[linenum][i] = '.';
				textDyn[linenum][i + 1] = 0;
			}

			currentWidth = 0;
			++linenum;
			i = -1;
		}
		if (text[ch] == ' ' && i >= 0)
		{
			lastSpace = ch;
			lastSpaceIndex = i;
		}
		++ch;
		++i;
	}
}

/**
 * Draw the text on screen
 */
void GuiText::Draw()
{
	if(!text)
		return;

	if(!IsVisible())
		return;

	GXColor c = color;
	c.a = GetAlpha();

	int newSize = size*GetScale();

	if(newSize != currentSize)
	{
		currentSize = newSize;

		if(text)
			textWidth = (font ? font : fontSystem)->getWidth(text, currentSize);
	}

	if(maxWidth > 0 && maxWidth <= textWidth)
	{
		if(wrapMode == DOTTED) // text dotted
		{
			if(textDyn.size() == 0)
				MakeDottedText();

			if(textDyn.size() > 0)
				(font ? font : fontSystem)->drawText(this->GetLeft(), this->GetTop(), GetZPosition(), textDyn[textDyn.size()-1], currentSize, c, alignment);
		}

		else if(wrapMode == SCROLL_HORIZONTAL)
		{
			ScrollText();

			if(textDyn.size() > 0)
				(font ? font : fontSystem)->drawText(this->GetLeft(), this->GetTop(), GetZPosition(), textDyn[textDyn.size()-1], currentSize, c, alignment);
		}
		else if(wrapMode == WRAP)
		{
			int lineheight = currentSize + 6;
			int voffset = 0;
			if(alignment & ALIGN_MIDDLE)
				voffset = -(lineheight*textDyn.size())/2 + lineheight/2;

			if(textDyn.size() == 0)
				WrapText();

			for(u32 i = 0; i < textDyn.size(); i++)
			{
				(font ? font : fontSystem)->drawText(this->GetLeft(), this->GetTop()+voffset+i*lineheight, GetZPosition(), textDyn[i], currentSize, c, alignment);
			}
		}
	}
	else
	{
		(font ? font : fontSystem)->drawText(this->GetLeft(), this->GetTop(), GetZPosition(), text, currentSize, c, alignment, textWidth);
	}
	this->UpdateEffects();
}
