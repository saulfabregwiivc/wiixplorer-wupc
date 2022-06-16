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
#ifndef __POPUPMENU_H_
#define __POPUPMENU_H_

#include <vector>

#include <gctypes.h>
#include <unistd.h>

#include "GUI/gui.h"
#include "Memory/Resources.h"

enum
{
	UP,
	DOWN
};

class PopUpMenu : public GuiFrame, public sigslot::has_slots<>
{
	public:
		PopUpMenu(int x, int y);
		virtual ~PopUpMenu();
		void AddItem(const char *text, const char *icon = NULL, bool submenu = false);
		void Finish();
		void OpenSubMenu(int position, PopUpMenu *menu);
		void CloseSubMenu(void);
		PopUpMenu *GetSubMenu() { return subMenu; }
		void SetUserData(void *data) { userData = data; }
		void *GetUserData(void) const { return userData; }
		void Update(GuiTrigger * t) { GuiFrame::Update(t); ++ScrollState; }
		sigslot::signal2<PopUpMenu *, int> ItemClicked;
	private:
		void OnClick(GuiButton *sender, int pointer, const POINT &p);
		void OnScrollUp(GuiButton *sender, int pointer, const POINT &p);
		void OnScrollDown(GuiButton *sender, int pointer, const POINT &p);
		void Scroll(int direction);

		typedef struct
		{
			GuiButton* Button;
			GuiText* ButtonTxt;
			GuiText* ButtonTxtOver;
			GuiImage* ButtonMenuSelect;
			GuiImage* ExpandImg;
			GuiImage* IconImg;
			GuiImageData* Icon;
		} Items;

		std::vector<Items> Item;

		void *userData;
		PopUpMenu *subMenu;

		int maxTxtWidth;

		u32 xpos;
		u32 ypos;
		u32 scrollIndex;
		u32 ScrollState;

		bool hasIcons;
		bool hasSubmenus;

		GuiImage * PopUpMenuUpperImg;
		GuiImage * PopUpMenuMiddleImg;
		GuiImage * PopUpMenuLowerImg;
		GuiImage * PopUpMenuScrollUpImg;
		GuiImage * PopUpMenuScrollDownImg;

		GuiImageData * PopUpMenuUpper;
		GuiImageData * PopUpMenuMiddle;
		GuiImageData * PopUpMenuLower;
		GuiImageData * PopUpMenuSelect;
		GuiImageData * PopUpMenuScrollUp;
		GuiImageData * PopUpMenuScrollDown;
		GuiImageData * PopUpMenuExpand;

		GuiSound * PopUpMenuClick;

		GuiButton * NoBtn;
		GuiButton * BackBtn;
		GuiButton * HomeBtn;
		GuiButton * ScrollUpBtn;
		GuiButton * ScrollDownBtn;

		SimpleGuiTrigger * trigA;
		GuiTrigger * trigAHeld;
		GuiTrigger * trigAOnly;
		GuiTrigger * trigB;
		GuiTrigger * trigUp;
		GuiTrigger * trigDown;
		GuiTrigger * trigHome;
};

#endif
