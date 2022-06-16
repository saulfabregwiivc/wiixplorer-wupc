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
#include "gui_tooltip.h"
#include "Memory/Resources.h"
#include "TextOperations/wstring.hpp"

GuiTooltip::GuiTooltip(const char *t)
{
	text = NULL;
	FontSize = 22;
	color = (GXColor){0, 0, 0, 255};
	oldParentState = STATE_DEFAULT;
	bIsHidden = true;

	tooltipLeft = Resources::GetImageData("tooltip_left.png");
	tooltipTile = Resources::GetImageData("tooltip_tile.png");
	tooltipRight = Resources::GetImageData("tooltip_right.png");
	leftImage = new GuiImage(tooltipLeft);
	tileImage = new GuiImage(tooltipTile);
	rightImage = new GuiImage(tooltipRight);

	height = leftImage->GetHeight();

	leftImage->SetParent(this);
	tileImage->SetParent(this);
	rightImage->SetParent(this);

	SetText(t);
}

GuiTooltip::~GuiTooltip()
{
	if(text)
		delete text;

	delete leftImage;
	delete tileImage;
	delete rightImage;

	Resources::Remove(tooltipLeft);
	Resources::Remove(tooltipTile);
	Resources::Remove(tooltipRight);
}

void GuiTooltip::SetText(const char * t)
{
	if(text)
		delete text;
	text = NULL;

	int tile_cnt = 0;
	if(t)
	{
		text = new GuiText(t, FontSize, color);
		text->SetParent(this);
		text->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
		tile_cnt = (int) ((float) (text->GetTextWidth()-12) / (float) tileImage->GetWidth());
		if(tile_cnt < 0)
			tile_cnt = 0;
	}
	tileImage->SetPosition(leftImage->GetWidth(), 0);
	tileImage->SetTileHorizontal(tile_cnt);
	rightImage->SetPosition(leftImage->GetWidth() + tile_cnt * tileImage->GetWidth(), 0);
	width = leftImage->GetWidth() + tile_cnt * tileImage->GetWidth() + rightImage->GetWidth();
	timer.reset();
}

void GuiTooltip::SetFontSize(int size)
{
	FontSize = size;

	if(text)
	{
		wString wText(text->GetText());
		SetText(wText.toUTF8().c_str());
	}
}

void GuiTooltip::SetColor(GXColor c)
{
	color = c;

	if(text)
	{
		wString wText(text->GetText());
		SetText(wText.toUTF8().c_str());
	}
}

void GuiTooltip::Draw()
{
	if(!IsVisible())
		return;

	if(parentElement->GetState() != oldParentState)
	{
		timer.reset();
		oldParentState = parentElement->GetState();
	}

	if(  (parentElement->GetState() == STATE_SELECTED)
	   && bIsHidden
	   && timer.elapsedMilliSecs() >= (u32)Settings.TooltipDelay)
	{
		SetEffect(EFFECT_FADE, 50);
		bIsHidden = false;
		timer.reset();
	}
	else if(   !bIsHidden
			&& (parentElement->GetState() != STATE_SELECTED))
	{
		SetEffect(EFFECT_FADE, -50);
		bIsHidden = true;
		timer.reset();
	}

	if(!bIsHidden || (GetEffect() != 0))
	{
		leftImage->Draw();
		tileImage->Draw();
		rightImage->Draw();
		if(text)
			text->Draw();
	}

	this->UpdateEffects();
}
