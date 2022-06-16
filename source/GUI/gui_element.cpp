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
#include "gui_element.h"

/**
 * Constructor for the Object class.
 */
GuiElement::GuiElement()
	: cutBoundsRect(0xBADBABE, 0xBADBABE, 0xBADBABE, 0xBADBABE)
{
	xoffset = 0;
	yoffset = 0;
	zoffset = 0;
	width = 0;
	height = 0;
	alpha = 255;
	scaleX = 1.0f;
	scaleY = 1.0f;
	state = STATE_DEFAULT;
	stateChan = -1;
	parentElement = NULL;
	rumble = true;
	selectable = false;
	clickable = false;
	holdable = false;
	visible = true;
	yoffsetDyn = 0;
	xoffsetDyn = 0;
	alphaDyn = -1;
	scaleDyn = 1;
	effects = EFFECT_NONE;
	effectAmount = 0;
	effectTarget = 0;
	effectsOver = EFFECT_NONE;
	effectAmountOver = 0;
	effectTargetOver = 0;

	// default alignment - align to top left
	alignment = (ALIGN_LEFT | ALIGN_TOP);
}

//!Sets the element's state
//!\param s State (STATE_DEFAULT, STATE_SELECTED, STATE_CLICKED, STATE_DISABLED)
//!\param c Controller channel (0-3, -1 = none)
void GuiElement::SetState(int s, int c )
{
	state = s;
	stateChan = c;
	StateChanged(this, s, c);
}

/**
 * Get the left position of the GuiElement.
 * @see SetLeft()
 * @return Left position in pixel.
 */
int GuiElement::GetLeft()
{
	int pWidth = 0;
	int pLeft = 0;

	if(parentElement)
	{
		pWidth = parentElement->GetWidth();
		pLeft = parentElement->GetLeft();
	}

	pLeft += xoffsetDyn;

	int x = pLeft;

	if(alignment & ALIGN_CENTER)
	{
		x = pLeft + (pWidth/2) - (width/2);
	}
	else if(alignment & ALIGN_RIGHT)
	{
		x = pLeft + pWidth - width;
	}

	return x + xoffset;
}

/**
 * Get the top position of the GuiElement.
 * @see SetTop()
 * @return Top position in pixel.
 */
int GuiElement::GetTop()
{
	int pHeight = 0;
	int pTop = 0;

	if(parentElement)
	{
		pHeight = parentElement->GetHeight();
		pTop = parentElement->GetTop();
	}

	pTop += yoffsetDyn;

	int y = pTop;

	if(alignment & ALIGN_MIDDLE)
	{
		y = pTop + (pHeight/2) - (height/2);
	}
	else if(alignment & ALIGN_BOTTOM)
	{
		y = pTop + pHeight - height;
	}

	return y + yoffset;
}

void GuiElement::SetEffect(int eff, int amount, int target)
{
	if(eff & EFFECT_SLIDE_IN)
	{
		// these calculations overcompensate a little
		if(eff & EFFECT_SLIDE_TOP)
		{
			if(eff & EFFECT_SLIDE_FROM)
				yoffsetDyn = (int) -GetHeight()*scaleY;
			else
				yoffsetDyn = -screenheight;
		}
		else if(eff & EFFECT_SLIDE_LEFT)
		{
			if(eff & EFFECT_SLIDE_FROM)
				xoffsetDyn = (int) -GetWidth()*scaleX;
			else
				xoffsetDyn = -screenwidth;
		}
		else if(eff & EFFECT_SLIDE_BOTTOM)
		{
			if(eff & EFFECT_SLIDE_FROM)
				yoffsetDyn = (int) GetHeight()*scaleY;
			else
				yoffsetDyn = screenheight;
		}
		else if(eff & EFFECT_SLIDE_RIGHT)
		{
			if(eff & EFFECT_SLIDE_FROM)
				xoffsetDyn = (int) GetWidth()*scaleX;
			else
				xoffsetDyn = screenwidth;
		}
	}
	if((eff & EFFECT_FADE) && amount > 0)
	{
		alphaDyn = 0;
	}
	else if((eff & EFFECT_FADE) && amount < 0)
	{
		alphaDyn = alpha;
	}

	effects |= eff;
	effectAmount = amount;
	effectTarget = target;
}

//!Sets an effect to be enabled on wiimote cursor over
//!\param e Effect to enable
//!\param a Amount of the effect (usage varies on effect)
//!\param t Target amount of the effect (usage varies on effect)
void GuiElement::SetEffectOnOver(int e, int a, int t)
{
	effectsOver |= e;
	effectAmountOver = a;
	effectTargetOver = t;
}

void GuiElement::ResetEffects()
{
	yoffsetDyn = 0;
	xoffsetDyn = 0;
	alphaDyn = -1;
	scaleDyn = 1;
	effects = EFFECT_NONE;
	effectAmount = 0;
	effectTarget = 0;
	effectsOver = EFFECT_NONE;
	effectAmountOver = 0;
	effectTargetOver = 0;
}

void GuiElement::UpdateEffects()
{
	if(effects & (EFFECT_SLIDE_IN | EFFECT_SLIDE_OUT | EFFECT_SLIDE_FROM))
	{
		if(effects & EFFECT_SLIDE_IN)
		{
			if(effects & EFFECT_SLIDE_LEFT)
			{
				xoffsetDyn += effectAmount;

				if(xoffsetDyn >= 0)
				{
					xoffsetDyn = 0;
					EffectFinished(this);
					effects = 0;
				}
			}
			else if(effects & EFFECT_SLIDE_RIGHT)
			{
				xoffsetDyn -= effectAmount;

				if(xoffsetDyn <= 0)
				{
					xoffsetDyn = 0;
					EffectFinished(this);
					effects = 0;
				}
			}
			else if(effects & EFFECT_SLIDE_TOP)
			{
				yoffsetDyn += effectAmount;

				if(yoffsetDyn >= 0)
				{
					EffectFinished(this);
					yoffsetDyn = 0;
					effects = 0;
				}
			}
			else if(effects & EFFECT_SLIDE_BOTTOM)
			{
				yoffsetDyn -= effectAmount;

				if(yoffsetDyn <= 0)
				{
					EffectFinished(this);
					yoffsetDyn = 0;
					effects = 0;
				}
			}
		}
		else
		{
			if(effects & EFFECT_SLIDE_LEFT)
			{
				xoffsetDyn -= effectAmount;

				if(xoffsetDyn <= -screenwidth) {
					EffectFinished(this);
					effects = 0; // shut off effect
				}
				else if((effects & EFFECT_SLIDE_FROM) && xoffsetDyn <= -GetWidth()) {
					EffectFinished(this);
					effects = 0; // shut off effect
				}
			}
			else if(effects & EFFECT_SLIDE_RIGHT)
			{
				xoffsetDyn += effectAmount;

				if(xoffsetDyn >= screenwidth) {
					EffectFinished(this);
					effects = 0; // shut off effect
				}
				else if((effects & EFFECT_SLIDE_FROM) && xoffsetDyn >= GetWidth()*scaleX) {
					EffectFinished(this);
					effects = 0; // shut off effect
				}
			}
			else if(effects & EFFECT_SLIDE_TOP)
			{
				yoffsetDyn -= effectAmount;

				if(yoffsetDyn <= -screenheight) {
					EffectFinished(this);
					effects = 0; // shut off effect
				}
				else if((effects & EFFECT_SLIDE_FROM) && yoffsetDyn <= -GetHeight()) {
					EffectFinished(this);
					effects = 0; // shut off effect
				}
			}
			else if(effects & EFFECT_SLIDE_BOTTOM)
			{
				yoffsetDyn += effectAmount;

				if(yoffsetDyn >= screenheight) {
					EffectFinished(this);
					effects = 0; // shut off effect
				}
				else if((effects & EFFECT_SLIDE_FROM) && yoffsetDyn >= GetHeight()) {
					EffectFinished(this);
					effects = 0; // shut off effect
				}
			}
		}
	}
	if(effects & EFFECT_FADE)
	{
		alphaDyn += effectAmount;

		if(effectAmount < 0 && alphaDyn <= 0)
		{
			EffectFinished(this);
			alphaDyn = 0;
			effects = 0; // shut off effect
		}
		else if(effectAmount > 0 && alphaDyn >= alpha)
		{
			EffectFinished(this);
			alphaDyn = alpha;
			effects = 0; // shut off effect
		}
	}
	if(effects & EFFECT_SCALE)
	{
		scaleDyn += effectAmount/100.0;

		if((effectAmount < 0 && scaleDyn <= effectTarget/100.0)
			|| (effectAmount > 0 && scaleDyn >= effectTarget/100.0))
		{
			EffectFinished(this);
			scaleDyn = effectTarget/100.0;
			effects = 0; // shut off effect
		}
	}
}
