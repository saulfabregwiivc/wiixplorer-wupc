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
#include "Memory/Resources.h"
#include "HorizontalScrollbar.hpp"
#include "Tools/tools.h"
#include "GUI/gui_trigger.h"
#include "input.h"

HorizontalScrollbar::HorizontalScrollbar(int w, u8 m)
{
	SelItem = 0;
	EntrieCount = 0;
	ScrollSpeed = 15;
	ScrollState = 0;
	Mode = m;
	AllowDPad = true;
	listChanged.connect(this, &HorizontalScrollbar::setScrollboxPosition);

	btnSoundOver = Resources::GetSound("button_over.wav");
	btnSoundClick = Resources::GetSound("button_click.wav");
	scrollbarTop = Resources::GetImageData("scrollBarTop.png");
	scrollbarBottom = Resources::GetImageData("scrollBarBottom.png");
	scrollbarTile = Resources::GetImageData("scrollBarTile.png");
	arrowDown = Resources::GetImageData("scrollbar_arrowdown.png");
	arrowDownOver = Resources::GetImageData("scrollbar_arrowdown_over.png");
	arrowUp = Resources::GetImageData("scrollbar_arrowup.png");
	arrowUpOver = Resources::GetImageData("scrollbar_arrowup_over.png");
	scrollbarBox = Resources::GetImageData("scrollbar_box.png");
	scrollbarBoxOver = Resources::GetImageData("scrollbar_box_over.png");
	oneButtonScrollImgData = Resources::GetImageData("oneButtonScroll.png");

	width = w;
	height = MAX(scrollbarBox->GetWidth(), scrollbarTile->GetWidth());

	MinWidth = arrowUp->GetHeight();
	MaxWidth = width-scrollbarBox->GetHeight()-arrowDown->GetHeight() + 10;

	trigHeldA = new GuiTrigger;
	trigHeldA->SetHeldTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	int Tiles = (width-scrollbarTop->GetHeight()-scrollbarBottom->GetHeight())/4;
	int PositionX = 10;
	ButtonPositionY = 0;

	scrollbarTopImg = new GuiImage(scrollbarTop);
	scrollbarTopImg->SetParent(this);
	scrollbarTopImg->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	scrollbarTopImg->SetPosition(PositionX, 0);
	scrollbarTopImg->SetAngle(270);
	PositionX += scrollbarTop->GetHeight() / 2 - scrollbarTile->GetHeight();

	scrollbarTileImg = new GuiImage(scrollbarTile);
	scrollbarTileImg->SetParent(this);
	scrollbarTileImg->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	scrollbarTileImg->SetPosition(PositionX, 0);
	scrollbarTileImg->SetAngle(270);
	scrollbarTileImg->SetTileHorizontal(Tiles);
	PositionX += Tiles*scrollbarTile->GetHeight() + scrollbarTop->GetHeight() / 2 - scrollbarTile->GetHeight();

	scrollbarBottomImg = new GuiImage(scrollbarBottom);
	scrollbarBottomImg->SetParent(this);
	scrollbarBottomImg->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	scrollbarBottomImg->SetPosition(PositionX, 0);
	scrollbarBottomImg->SetAngle(270);

	arrowDownImg = new GuiImage(arrowDown);
	arrowDownOverImg = new GuiImage(arrowDownOver);
	arrowUpImg = new GuiImage(arrowUp);
	arrowUpOverImg = new GuiImage(arrowUpOver);
	scrollbarBoxImg = new GuiImage(scrollbarBox);
	scrollbarBoxOverImg = new GuiImage(scrollbarBoxOver);

	arrowUpBtn = new GuiButton(arrowUpImg->GetWidth(), arrowUpImg->GetHeight());
	arrowUpBtn->SetParent(this);
	arrowUpBtn->SetImage(arrowUpImg);
	arrowUpBtn->SetImageOver(arrowUpOverImg);
	arrowUpBtn->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	arrowUpBtn->SetPosition(0, 3);
	arrowUpBtn->SetHoldable(true);
	arrowUpBtn->SetTrigger(trigHeldA);
	arrowUpBtn->SetSoundOver(btnSoundOver);
	arrowUpBtn->SetSoundClick(btnSoundClick);
	arrowUpBtn->Held.connect(this, &HorizontalScrollbar::OnUpButtonHold);

	arrowDownBtn = new GuiButton(arrowDownImg->GetWidth(), arrowDownImg->GetHeight());
	arrowDownBtn->SetParent(this);
	arrowDownBtn->SetImage(arrowDownImg);
	arrowDownBtn->SetImageOver(arrowDownOverImg);
	arrowDownBtn->SetAlignment(ALIGN_RIGHT | ALIGN_MIDDLE);
	arrowDownBtn->SetPosition(0, -5);
	arrowDownBtn->SetHoldable(true);
	arrowDownBtn->SetTrigger(trigHeldA);
	arrowDownBtn->SetSoundOver(btnSoundOver);
	arrowDownBtn->SetSoundClick(btnSoundClick);
	arrowDownBtn->Held.connect(this, &HorizontalScrollbar::OnDownButtonHold);

	scrollbarBoxImg->SetAngle(270);
	scrollbarBoxOverImg->SetAngle(270);
	scrollbarBoxBtn = new GuiButton(scrollbarBoxImg->GetWidth(), scrollbarBoxImg->GetHeight());
	scrollbarBoxBtn->SetParent(this);
	scrollbarBoxBtn->SetImage(scrollbarBoxImg);
	scrollbarBoxBtn->SetImageOver(scrollbarBoxOverImg);
	scrollbarBoxBtn->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	scrollbarBoxBtn->SetPosition(MinWidth, 0);
	scrollbarBoxBtn->SetHoldable(true);
	scrollbarBoxBtn->SetTrigger(trigHeldA);
	scrollbarBoxBtn->Held.connect(this, &HorizontalScrollbar::OnBoxButtonHold);
}

HorizontalScrollbar::~HorizontalScrollbar()
{
	Resources::Remove(btnSoundOver);
	Resources::Remove(btnSoundClick);
	Resources::Remove(scrollbarTop);
	Resources::Remove(scrollbarBottom);
	Resources::Remove(scrollbarTile);
	Resources::Remove(arrowDown);
	Resources::Remove(arrowDownOver);
	Resources::Remove(arrowUp);
	Resources::Remove(arrowUpOver);
	Resources::Remove(scrollbarBox);
	Resources::Remove(scrollbarBoxOver);
	Resources::Remove(oneButtonScrollImgData);

	delete arrowUpBtn;
	delete arrowDownBtn;
	delete scrollbarBoxBtn;

	delete scrollbarTopImg;
	delete scrollbarBottomImg;
	delete scrollbarTileImg;
	delete arrowDownImg;
	delete arrowDownOverImg;
	delete arrowUpImg;
	delete arrowUpOverImg;
	delete scrollbarBoxImg;
	delete scrollbarBoxOverImg;

	delete trigHeldA;
}

void HorizontalScrollbar::ScrollOneUp()
{
	if(SelItem > 0)
	{
		// move list left by 1
		--SelItem;
	}
}

void HorizontalScrollbar::ScrollOneDown()
{
	if(SelItem + 1 < EntrieCount)
	{
		// move list right by 1
		SelItem++;
	}
}

void HorizontalScrollbar::OnUpButtonHold(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	if(ScrollState < ScrollSpeed)
		return;

	ScrollOneUp();

	ScrollState = 0;
	listChanged(SelItem);
}

void HorizontalScrollbar::OnDownButtonHold(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	if(ScrollState < ScrollSpeed)
		return;

	ScrollOneDown();

	ScrollState = 0;
	listChanged(SelItem);
}

void HorizontalScrollbar::OnBoxButtonHold(GuiButton *sender UNUSED, int pointer, const POINT &p)
{
	if(ScrollState < ScrollSpeed)
		return;

	if(!userInput[pointer].wpad.ir.valid)
		return;

	int x = p.x - this->GetLeft()-scrollbarBox->GetHeight()/2;

	int positionWiimote = LIMIT(x-MinWidth, 0, MaxWidth-MinWidth);

	int newSelected = ( positionWiimote * (EntrieCount-1) ) / (MaxWidth-MinWidth);

	SelItem = LIMIT(newSelected, 0, EntrieCount-1);

	ScrollState = 0;
	listChanged(SelItem);
}

void HorizontalScrollbar::SetSelectedItem(int pos)
{
	SelItem = LIMIT(pos, 0, EntrieCount-1);
	listChanged(SelItem);
}

void HorizontalScrollbar::SetEntrieCount(int cnt)
{
	if(EntrieCount == cnt)
		return;

	EntrieCount = cnt;
	listChanged(SelItem);
}

void HorizontalScrollbar::setScrollboxPosition(int SelItem)
{
	int position = MinWidth + ( (MaxWidth-MinWidth) * SelItem ) / (EntrieCount-1);

	if(position < MinWidth)
		position = MinWidth;
	else if(position > MaxWidth || (SelItem >= EntrieCount-1))
		position = MaxWidth;

	scrollbarBoxBtn->SetPosition(position, ButtonPositionY);
}

void HorizontalScrollbar::CheckDPadControls(GuiTrigger *t)
{
	if(t->Left())
	{
		ScrollOneUp();
		listChanged(SelItem);
	}
	else if(t->Right())
	{
		ScrollOneDown();
		listChanged(SelItem);
	}
}

void HorizontalScrollbar::Draw()
{
	scrollbarTileImg->Draw();
	scrollbarTopImg->Draw();
	scrollbarBottomImg->Draw();
	arrowUpBtn->Draw();
	arrowDownBtn->Draw();
	scrollbarBoxBtn->Draw();

	UpdateEffects();
}

void HorizontalScrollbar::Update(GuiTrigger * t)
{
	if(!t || state == STATE_DISABLED)
		return;

	arrowUpBtn->Update(t);
	arrowDownBtn->Update(t);
	scrollbarBoxBtn->Update(t);
	if(AllowDPad)
		CheckDPadControls(t);

	++ScrollState;
}

void HorizontalScrollbar::SetBounds(iRect w)
{
	GuiElement::SetBounds(w);
	scrollbarTopImg->SetBounds(w);
	scrollbarBottomImg->SetBounds(w);
	scrollbarTileImg->SetBounds(w);
	arrowDownImg->SetBounds(w);
	arrowDownOverImg->SetBounds(w);
	arrowUpImg->SetBounds(w);
	arrowUpOverImg->SetBounds(w);
	scrollbarBoxImg->SetBounds(w);
	scrollbarBoxOverImg->SetBounds(w);
}
