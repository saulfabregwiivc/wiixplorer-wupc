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
#ifndef GUI_TEXT_H_
#define GUI_TEXT_H_

#include "gui_element.h"

#define PAGESIZE	 			8
#define MAX_LINES_TO_DRAW		12

//!Forward declaration
class FreeTypeGX;

//!Display, manage, and manipulate text in the GUI
class GuiText : public GuiElement
{
	public:
		//!Constructor
		//!\param t Text
		//!\param s Font size
		//!\param c Font color
		GuiText(const char * t, int s, GXColor c);
		//!\overload
		//!\param t Text
		//!\param s Font size
		//!\param c Font color
		GuiText(const wchar_t * t, int s, GXColor c);
		//!\overload
		//!\Assumes SetPresets() has been called to setup preferred text attributes
		//!\param t Text
		GuiText(const char * t);
		//!Destructor
		virtual ~GuiText();
		//!Sets the text of the GuiText element
		//!\param t Text
		virtual void SetText(const char * t);
		virtual void SetText(const wchar_t * t);
		virtual void SetTextf(const char *format, ...) __attribute__((format(printf,2,3)));
		//!Sets up preset values to be used by GuiText(t)
		//!Useful when printing multiple text elements, all with the same attributes set
		//!\param sz Font size
		//!\param c Font color
		//!\param w Maximum width of texture image (for text wrapping)
		//!\param wrap Wrapmode when w>0
		//!\param a Text alignment
		void SetPresets(int sz, GXColor c, int w, int a);
		//!Sets the font size
		//!\param s Font size
		void SetFontSize(int s);
		//!Sets the maximum width of the drawn texture image
		//!If the text exceeds this, it is wrapped to the next line
		//!\param w Maximum width
		//!\param m WrapMode
		void SetMaxWidth(int w = 0, int m = WRAP);
		//!Sets the font color
		//!\param c Font color
		void SetColor(GXColor c);
		//!Get the original text as char
		virtual const wchar_t * GetText() const { return text; }
		virtual std::string toUTF8(void) const;
		//!Get the Horizontal Size of Text
		int GetTextWidth() { return textWidth; }
		int GetTextWidth(int ind);
		//!Get the max textwidth
		int GetTextMaxWidth() { return maxWidth; }
		//!Gets the total line number
		virtual int GetLinesCount() { return 1; };
		//!Get fontsize
		int GetFontSize() { return size; };
		//!Set max lines to draw
		void SetLinesToDraw(int l) { linestodraw = l; }
		//!Get current Textline (for position calculation)
		const wchar_t * GetDynText(int ind = 0);
		virtual const wchar_t * GetTextLine(int ind) { return GetDynText(ind); };
		//!Change the font
		//!\param font bufferblock
		//!\param font filesize
		bool SetFont(const u8 *font, const u32 filesize);
		//! virtual function used in child classes
		virtual int GetStartWidth() { return 0; };
		//!Constantly called to draw the text
		void Draw();
	protected:
		//!Clear the dynamic text
		void ClearDynamicText();
		//!Create a dynamic dotted text if the text is too long
		void MakeDottedText();
		//!Scroll the text once
		void ScrollText();
		//!Wrap the text to several lines
		void WrapText();

		wchar_t *text;
		std::vector<wchar_t *> textDyn;
		int wrapMode; //!< Wrapping toggle
		int textScrollPos; //!< Current starting index of text string for scrolling
		int textScrollInitialDelay; //!< Delay to wait before starting to scroll
		int textScrollDelay; //!< Scrolling speed
		int size; //!< Font size
		int maxWidth; //!< Maximum width of the generated text object (for text wrapping)
		GXColor color; //!< Font color
		FreeTypeGX *font;
		int textWidth;
		int currentSize;
		int linestodraw;
};

#endif
