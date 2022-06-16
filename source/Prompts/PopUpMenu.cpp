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
#include "PopUpMenu.h"

static const int ButtonX = 20;
static const u32 ButtonHeight = 32;
static const u32 MaxVisible = 10;

PopUpMenu::PopUpMenu(int x, int y)
	:GuiFrame(0, 0)
{
	width = 0;
	height = 0;
	maxTxtWidth = 0;
	scrollIndex = 0;
	ScrollState = 0;
	hasIcons = false;
	hasSubmenus = false;
	subMenu = NULL;

	PopUpMenuClick = Resources::GetSound("button_click.wav");

	PopUpMenuUpper = Resources::GetImageData("ClickMenuUpper.png");
	PopUpMenuMiddle = Resources::GetImageData("ClickMenuMiddle.png");
	PopUpMenuLower = Resources::GetImageData("ClickMenuLower.png");
	PopUpMenuSelect = Resources::GetImageData("menu_selection.png");
	PopUpMenuScrollUp = Resources::GetImageData("arrow_up.png");
	PopUpMenuScrollDown = Resources::GetImageData("arrow_down.png");
	PopUpMenuExpand = Resources::GetImageData("arrow_right.png");

	PopUpMenuUpperImg = new GuiImage(PopUpMenuUpper);
	PopUpMenuMiddleImg = new GuiImage(PopUpMenuMiddle);
	PopUpMenuLowerImg = new GuiImage(PopUpMenuLower);
	PopUpMenuScrollUpImg = new GuiImage(PopUpMenuScrollUp);
	PopUpMenuScrollUpImg->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	PopUpMenuScrollDownImg = new GuiImage(PopUpMenuScrollDown);
	PopUpMenuScrollDownImg->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);

	trigA = new SimpleGuiTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigAHeld = new GuiTrigger();
	trigAOnly = new GuiTrigger();
	trigB = new GuiTrigger();
	trigUp = new GuiTrigger();
	trigDown = new GuiTrigger();
	trigHome = new GuiTrigger();
	trigAOnly->SetButtonOnlyTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigB->SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);
	trigAHeld->SetHeldTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigUp->SetButtonOnlyHeldTrigger(-1, WiiControls.UpButton | ClassicControls.UpButton << 16, GCControls.UpButton);
	trigDown->SetButtonOnlyHeldTrigger(-1, WiiControls.DownButton | ClassicControls.DownButton << 16, GCControls.DownButton);
	trigHome->SetButtonOnlyTrigger(-1, WiiControls.HomeButton | ClassicControls.HomeButton << 16, GCControls.HomeButton);

	NoBtn = new GuiButton(0, 0);
	NoBtn->SetTrigger(trigAOnly);
	NoBtn->Clicked.connect(this, &PopUpMenu::OnClick);

	BackBtn = new GuiButton(0, 0);
	BackBtn->SetTrigger(trigB);
	BackBtn->Clicked.connect(this, &PopUpMenu::OnClick);

	HomeBtn = new GuiButton(0, 0);
	HomeBtn->SetTrigger(trigHome);
	HomeBtn->Clicked.connect(this, &PopUpMenu::OnClick);

	ScrollUpBtn = new GuiButton(PopUpMenuUpperImg->GetWidth(), PopUpMenuScrollUpImg->GetHeight()+5);
	ScrollUpBtn->SetImage(PopUpMenuScrollUpImg);
	ScrollUpBtn->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	ScrollUpBtn->SetTrigger(trigAHeld);
	ScrollUpBtn->SetVisible(false);
	ScrollUpBtn->SetState(STATE_DISABLED);
	ScrollUpBtn->SetTrigger(trigUp);
	ScrollUpBtn->SetHoldable(true);
	ScrollUpBtn->Held.connect(this, &PopUpMenu::OnScrollUp);

	ScrollDownBtn = new GuiButton(PopUpMenuLowerImg->GetWidth(), PopUpMenuScrollDownImg->GetHeight()+5);
	ScrollDownBtn->SetImage(PopUpMenuScrollDownImg);
	ScrollDownBtn->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	ScrollDownBtn->SetTrigger(trigAHeld);
	ScrollDownBtn->SetVisible(false);
	ScrollDownBtn->SetState(STATE_DISABLED);
	ScrollDownBtn->SetTrigger(trigDown);
	ScrollDownBtn->SetHoldable(true);
	ScrollDownBtn->Held.connect(this, &PopUpMenu::OnScrollDown);

	Append(PopUpMenuUpperImg);
	Append(PopUpMenuMiddleImg);
	Append(PopUpMenuLowerImg);
	Append(NoBtn);
	Append(BackBtn);
	Append(HomeBtn);
	Append(ScrollUpBtn);
	Append(ScrollDownBtn);

	xpos = x;
	ypos = y;
}

PopUpMenu::~PopUpMenu()
{
	RemoveAll();

	Resources::Remove(PopUpMenuMiddle);
	Resources::Remove(PopUpMenuUpper);
	Resources::Remove(PopUpMenuLower);
	Resources::Remove(PopUpMenuSelect);
	Resources::Remove(PopUpMenuScrollUp);
	Resources::Remove(PopUpMenuScrollDown);
	Resources::Remove(PopUpMenuExpand);
	Resources::Remove(PopUpMenuClick);

	for (u32 i = 0; i < Item.size(); i++)
	{
		delete Item[i].ButtonMenuSelect;
		delete Item[i].Button;
		delete Item[i].ButtonTxt;
		delete Item[i].ButtonTxtOver;

		if (Item[i].Icon)
		{
			Resources::Remove(Item[i].Icon);
			delete Item[i].IconImg;
		}

		if (Item[i].ExpandImg)
			delete Item[i].ExpandImg;
	}

	Item.clear();

	delete PopUpMenuMiddleImg;
	delete PopUpMenuUpperImg;
	delete PopUpMenuLowerImg;
	delete PopUpMenuScrollUpImg;
	delete PopUpMenuScrollDownImg;

	delete ScrollUpBtn;
	delete ScrollDownBtn;
	delete NoBtn;
	delete BackBtn;
	delete HomeBtn;

	delete trigA;
	delete trigAHeld;
	delete trigAOnly;
	delete trigB;
	delete trigUp;
	delete trigDown;
	delete trigHome;
}

void PopUpMenu::AddItem(const char *text, const char *icon, bool submenu)
{
	Items item;

	item.ButtonTxt = new GuiText(text, 24, (GXColor){0, 0, 0, 255});
	item.ButtonTxtOver = new GuiText(text, 24, (GXColor){28, 32, 190, 255});
	item.Button = new GuiButton(0, 0);
	item.ButtonMenuSelect = new GuiImage(PopUpMenuSelect);
	item.ButtonTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	item.ButtonTxtOver->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	item.Button->SetLabel(item.ButtonTxt);
	item.Button->SetLabelOver(item.ButtonTxtOver);
	item.Button->SetSoundClick(PopUpMenuClick);
	item.Button->SetImageOver(item.ButtonMenuSelect);
	item.Button->SetTrigger(trigA);
	item.Button->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	item.Button->Clicked.connect(this, &PopUpMenu::OnClick);

	Append(item.Button);

	if (icon)
	{
		item.Icon = Resources::GetImageData(icon);

		item.IconImg = new GuiImage(item.Icon);
		item.IconImg->SetPosition(ButtonX, (Item.size()+1)*ButtonHeight-18);
		item.IconImg->SetScale(0.8);

		hasIcons = true;

		Append(item.IconImg);
	}
	else
	{
		item.Icon = NULL;
		item.IconImg = NULL;
	}

	if (submenu)
	{
		item.ExpandImg = new GuiImage(PopUpMenuExpand);

		hasSubmenus = true;

		Append(item.ExpandImg);
	}
	else
		item.ExpandImg = NULL;

	if (maxTxtWidth < item.ButtonTxt->GetTextWidth())
		maxTxtWidth = item.ButtonTxt->GetTextWidth();

	Item.push_back(item);
}

void PopUpMenu::OpenSubMenu(int position, PopUpMenu *menu)
{
	if(subMenu)
		CloseSubMenu();
	subMenu = menu;
	subMenu->SetPosition(this->GetWidth()-12, position*30);
	this->Append(subMenu);
}

void PopUpMenu::CloseSubMenu(void)
{
	if(subMenu) {
		Remove(subMenu);
		subMenu = NULL;
	}
}

void PopUpMenu::Finish()
{
	u32 x = xpos;
	u32 y = ypos;
	u32 middleheight = ButtonHeight*Item.size();
	int leftmargin = (hasIcons ? ButtonX+40 : ButtonX);
	int rightmargin = (hasSubmenus ? 40 : 20);
	float scale = 1.0f;
	int ButtonsOffset = 0;

	if(Item.size() > MaxVisible)
	{
		ScrollUpBtn->SetVisible(true);
		ScrollUpBtn->SetState(STATE_DEFAULT);
		ScrollDownBtn->SetVisible(true);
		ScrollDownBtn->SetState(STATE_DEFAULT);


		middleheight += PopUpMenuScrollUpImg->GetHeight()+PopUpMenuScrollDownImg->GetHeight();
		/*
		ButtonsOffset += PopUpMenuScrollUpImg->GetHeight();
		*/

		middleheight = ButtonHeight*MaxVisible;

		for (u32 i = MaxVisible; i < Item.size(); i++)
		{
			Item[i].Button->SetVisible(false);
			Item[i].Button->SetState(STATE_DISABLED);

			if (Item[i].IconImg)
				Item[i].IconImg->SetVisible(false);

			if (Item[i].ExpandImg)
				Item[i].ExpandImg->SetVisible(false);
		}
	}

	middleheight = MAX(middleheight, ButtonHeight);
	width  = maxTxtWidth+leftmargin+rightmargin;
	height = PopUpMenuLowerImg->GetHeight()+middleheight+PopUpMenuUpperImg->GetHeight();

	PopUpMenuMiddleImg->SetTileVertical(middleheight/PopUpMenuMiddleImg->GetHeight());

	if (width > PopUpMenuMiddleImg->GetWidth())
		scale = 1.0f * width / PopUpMenuMiddleImg->GetWidth();
	else
		width = PopUpMenuMiddleImg->GetWidth();

	PopUpMenuUpperImg->SetScaleX(scale);
	PopUpMenuMiddleImg->SetScaleX(scale);
	PopUpMenuLowerImg->SetScaleX(scale);

	u32 scaledX = (scale-1)*PopUpMenuMiddleImg->GetWidth()/2;

	PopUpMenuUpperImg->SetPosition(scaledX, 0);
	PopUpMenuMiddleImg->SetPosition(scaledX, PopUpMenuUpperImg->GetHeight());
	PopUpMenuLowerImg->SetPosition(scaledX, PopUpMenuUpperImg->GetHeight()+middleheight);

	ScrollUpBtn->SetPosition(width/2-ScrollUpBtn->GetWidth()/2, 2);
	ScrollDownBtn->SetPosition(width/2-ScrollDownBtn->GetWidth()/2, PopUpMenuLowerImg->GetTop()-ScrollDownBtn->GetHeight()/2);

	if (x + width + 15 > (u32)screenwidth)
		x = screenwidth - width - 15;

	if (y + height + 25 > (u32)screenheight)
		y = screenheight - height - 35;

	for (u32 i = 0; i < Item.size(); i++)
	{
		Item[i].Button->SetSize(maxTxtWidth, ButtonHeight);
		Item[i].Button->SetPosition(leftmargin, i*ButtonHeight+20+ButtonsOffset);

		Item[i].ButtonMenuSelect->SetScaleX(scale);
		Item[i].ButtonMenuSelect->SetPosition(scaledX, -4);

		if (Item[i].ExpandImg)
			Item[i].ExpandImg->SetPosition(width-32, i*ButtonHeight+22+ButtonsOffset);
	}

	SetPosition(x, y);
}

void PopUpMenu::OnClick(GuiButton *sender, int pointer UNUSED, const POINT &p)
{
	if (sender == NoBtn)
	{
		if(!IsInside(p.x, p.y) && (!subMenu || !subMenu->IsInside(p.x, p.y)))
			ItemClicked(this, -10);
		return;
	}
	else if (sender == BackBtn || sender == HomeBtn)
	{
		ItemClicked(this, -10);
		return;
	}

	for (u32 i = 0; i < Item.size(); i++)
	{
		if(sender == Item[i].Button)
		{
			ItemClicked(this, i);
			return;
		}
	}
}

void PopUpMenu::OnScrollUp(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	if(ScrollState < (u32) Settings.ScrollSpeed)
		return;

	Scroll(UP);

	ScrollState = 0;
}

void PopUpMenu::OnScrollDown(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	if(ScrollState < (u32) Settings.ScrollSpeed)
		return;

	Scroll(DOWN);

	ScrollState = 0;
}

void PopUpMenu::Scroll(int direction)
{
	int step = ButtonHeight;

	if (direction == UP)
	{
		if (scrollIndex < 1)
			return;

		scrollIndex--;
		ScrollDownBtn->SetVisible(true);
		ScrollDownBtn->SetState(STATE_DEFAULT);
	}
	else
	{
		if (scrollIndex >= Item.size()-MaxVisible)
			return;

		scrollIndex++;
		ScrollUpBtn->SetVisible(true);
		ScrollUpBtn->SetState(STATE_DEFAULT);
		step *= -1;
	}

	for (u32 i = 0; i < Item.size(); i++)
	{
		bool visible = (i >= scrollIndex && i < scrollIndex+MaxVisible);
		int state = visible ? STATE_DEFAULT : STATE_DISABLED;

		Item[i].Button->SetVisible(visible);
		Item[i].Button->SetPosition(Item[i].Button->GetLeft()-GetLeft(), Item[i].Button->GetTop()-GetTop()+step);
		Item[i].Button->SetState(state);

		if (Item[i].IconImg)
		{
			Item[i].IconImg->SetVisible(visible);
			Item[i].IconImg->SetPosition(Item[i].IconImg->GetLeft()-GetLeft(), Item[i].IconImg->GetTop()-GetTop()+step);
		}

		if (Item[i].ExpandImg)
		{
			Item[i].ExpandImg->SetVisible(visible);
			Item[i].ExpandImg->SetPosition(Item[i].ExpandImg->GetLeft()-GetLeft(), Item[i].ExpandImg->GetTop()-GetTop()+step);
		}
	}

	if (scrollIndex >= Item.size()-MaxVisible)
	{
		ScrollDownBtn->SetVisible(false);
		ScrollDownBtn->SetState(STATE_DISABLED);
	}

	if (scrollIndex < 1)
	{
		ScrollUpBtn->SetVisible(false);
		ScrollUpBtn->SetState(STATE_DISABLED);
	}
}
