/****************************************************************************
 * Copyright (C) 2009-2013 Dimok
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
#ifndef HORIZONTALSCROLLBAR_HPP_
#define HORIZONTALSCROLLBAR_HPP_

#include "GUI/gui_element.h"
#include "GUI/gui_button.h"

class HorizontalScrollbar : public GuiElement, public sigslot::has_slots<>
{
	public:
		HorizontalScrollbar(int height, u8 mode = LISTMODE);
		virtual ~HorizontalScrollbar();
		void ScrollOneUp();
		void ScrollOneDown();
		int GetSelectedItem() { return SelItem; }
		int GetEntrieCount() { return EntrieCount; }
		void SetDPadControl(bool a) { AllowDPad = a; }
		void SetScrollSpeed(u16 speed) { ScrollSpeed = speed; }
		void SetBounds(iRect w);
		void Draw();
		void Update(GuiTrigger * t);
		enum
		{
			ICONMODE = 0,
			LISTMODE
		};
		//! Signals
		sigslot::signal1<int> listChanged;
		//! Slots
		void SetSelectedItem(int pos);
		void SetEntrieCount(int cnt);
	protected:
		void setScrollboxPosition(int SelItem);
		void OnUpButtonHold(GuiButton *sender, int pointer, const POINT &p);
		void OnDownButtonHold(GuiButton *sender, int pointer, const POINT &p);
		void OnBoxButtonHold(GuiButton *sender, int pointer, const POINT &p);
		void CheckDPadControls(GuiTrigger *t);

		u8 Mode;
		u32 ScrollState;
		u16 ScrollSpeed;

		int MinWidth;
		int MaxWidth;
		int SelItem;
		int EntrieCount;
		int ButtonPositionY;
		bool listchanged;
		bool AllowDPad;

		GuiButton * arrowUpBtn;
		GuiButton * arrowDownBtn;
		GuiButton * scrollbarBoxBtn;
		GuiImage * scrollbarTopImg;
		GuiImage * scrollbarBottomImg;
		GuiImage * scrollbarTileImg;
		GuiImage * arrowDownImg;
		GuiImage * arrowDownOverImg;
		GuiImage * arrowUpImg;
		GuiImage * arrowUpOverImg;
		GuiImage * scrollbarBoxImg;
		GuiImage * scrollbarBoxOverImg;
		GuiImageData * scrollbarTop;
		GuiImageData * scrollbarBottom;
		GuiImageData * scrollbarTile;
		GuiImageData * arrowDown;
		GuiImageData * arrowDownOver;
		GuiImageData * arrowUp;
		GuiImageData * arrowUpOver;
		GuiImageData * scrollbarBox;
		GuiImageData * scrollbarBoxOver;
		GuiImageData * oneButtonScrollImgData;
		GuiSound * btnSoundOver;
		GuiSound * btnSoundClick;
		GuiTrigger * trigHeldA;
};

#endif
