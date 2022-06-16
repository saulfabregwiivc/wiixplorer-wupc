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
#include "gui_button.h"
#include "input.h"

/**
 * Constructor for the GuiButton class.
 */

GuiButton::GuiButton(int w, int h)
{
	ClickAndHold = false;
	width = w;
	height = h;
	Init();
}

/**
 * Destructor for the GuiButton class.
 */
GuiButton::~GuiButton()
{
}

void GuiButton::Init()
{
	image = NULL;
	imageOver = NULL;
	imageHold = NULL;
	imageClick = NULL;
	icon = NULL;
	iconOver = NULL;
	iconHold = NULL;
	iconClick = NULL;
	tooltip = NULL;

	for(int i = 0; i < 3; i++)
	{
		label[i] = NULL;
		labelOver[i] = NULL;
		labelHold[i] = NULL;
		labelClick[i] = NULL;
	}

	soundOver = NULL;
	soundHold = NULL;
	soundClick = NULL;
	selectable = true;
	holdable = false;
	clickable = true;

	for(int i = 0; i < 4; ++i)
		trigger[i] = NULL;
}

void GuiButton::SetImage(GuiImage* img)
{
	image = img;
	if(img) img->SetParent(this);
}
void GuiButton::SetImageOver(GuiImage* img)
{
	imageOver = img;
	if(img) img->SetParent(this);
}
void GuiButton::SetImageHold(GuiImage* img)
{
	imageHold = img;
	if(img) img->SetParent(this);
}
void GuiButton::SetImageClick(GuiImage* img)
{
	imageClick = img;
	if(img) img->SetParent(this);
}
void GuiButton::SetIcon(GuiImage* img)
{
	icon = img;
	if(img) img->SetParent(this);
}
void GuiButton::SetIconOver(GuiImage* img)
{
	iconOver = img;
	if(img) img->SetParent(this);
}
void GuiButton::SetIconHold(GuiImage* img)
{
	iconHold = img;
	if(img) img->SetParent(this);
}
void GuiButton::SetIconClick(GuiImage* img)
{
	iconClick = img;
	if(img) img->SetParent(this);
}
void GuiButton::SetLabel(GuiText* txt, int n)
{
	label[n] = txt;
	if(txt) txt->SetParent(this);
}
void GuiButton::SetLabelOver(GuiText* txt, int n)
{
	labelOver[n] = txt;
	if(txt) txt->SetParent(this);
}
void GuiButton::SetLabelHold(GuiText* txt, int n)
{
	labelHold[n] = txt;
	if(txt) txt->SetParent(this);
}
void GuiButton::SetLabelClick(GuiText* txt, int n)
{
	labelClick[n] = txt;
	if(txt) txt->SetParent(this);
}
void GuiButton::SetSoundOver(GuiSound * snd)
{
	soundOver = snd;
}
void GuiButton::SetSoundHold(GuiSound * snd)
{
	soundHold = snd;
}

void GuiButton::SetSoundClick(GuiSound * snd)
{
	soundClick = snd;
}

void GuiButton::SetTrigger(GuiTrigger * t)
{
	if(!trigger[0])
		trigger[0] = t;
	else if(!trigger[1])
		trigger[1] = t;
	else if(!trigger[2])
		trigger[2] = t;
	else if(!trigger[3])
		trigger[3] = t;
	else // both were assigned, so we'll just overwrite the first one
		trigger[0] = t;
}

void GuiButton::SetTrigger(u8 i, GuiTrigger * t)
{
	trigger[i] = t;
}

void GuiButton::SetTooltip(GuiTooltip * t)
{
	tooltip = t;
	if(t)
		tooltip->SetParent(this);
}

void GuiButton::SetBounds(iRect w)
{
	if(image)
		image->SetBounds(w);
	if(imageOver)
		imageOver->SetBounds(w);
	if(imageHold)
		imageHold->SetBounds(w);
	if(imageClick)
		imageClick->SetBounds(w);
	if(icon)
		icon->SetBounds(w);
	if(iconOver)
		iconOver->SetBounds(w);
	if(iconHold)
		iconHold->SetBounds(w);
	if(iconClick)
		iconClick->SetBounds(w);
	if(tooltip)
		tooltip->SetBounds(w);
}

/**
 * Draw the button on screen
 */
void GuiButton::Draw()
{
	if(!this->IsVisible())
		return;

	// draw image
	if((state == STATE_SELECTED || state == STATE_HELD) && imageOver)
		imageOver->Draw();
	else if(image)
		image->Draw();
	// draw icon
	if((state == STATE_SELECTED || state == STATE_HELD) && iconOver)
		iconOver->Draw();
	else if(icon)
		icon->Draw();
	// draw text
	for(int i=0; i<3; i++)
	{
		if((state == STATE_SELECTED || state == STATE_HELD) && labelOver[i])
			labelOver[i]->Draw();
		else if(label[i])
			label[i]->Draw();
	}

	if(tooltip)
		tooltip->Draw();

	this->UpdateEffects();
}

void GuiButton::Update(GuiTrigger * t)
{
	if(state == STATE_DISABLED || !t)
		return;
	else if(parentElement && parentElement->GetState() == STATE_DISABLED)
		return;

	#ifdef HW_RVL
	// cursor
	if(t->wpad.ir.valid && t->chan >= 0)
	{
		if(this->IsInside(t->wpad.ir.x, t->wpad.ir.y))
		{
			if(state == STATE_DEFAULT || (state == STATE_SELECTED && t->chan != stateChan)) // we weren't on the button before!
			{
				this->SetState(STATE_SELECTED, t->chan);

				if(this->Rumble())
					RequestRumble(t->chan);

				if(soundOver)
					soundOver->Play();

				if(effectsOver && !effects)
				{
					// initiate effects
					effects = effectsOver;
					effectAmount = effectAmountOver;
					effectTarget = effectTargetOver;
				}
			}
		}
		else
		{
			if(state == STATE_SELECTED && (stateChan == t->chan || stateChan == -1))
				this->ResetState();

			if(effectTarget == effectTargetOver && effectAmount == effectAmountOver)
			{
				// initiate effects (in reverse)
				effects = effectsOver;
				effectAmount = -effectAmountOver;
				effectTarget = 100;
			}
		}
	}
	#endif

	// button triggers
	if(clickable)
	{
		for(int i = 0; i < 4; i++)
		{
			if(trigger[i] && (trigger[i]->chan == -1 || trigger[i]->chan == t->chan))
			{
				if((t->wpad.btns_d & trigger[i]->wpad.btns_d) || (t->pad.btns_d & trigger[i]->pad.btns_d))
				{
					if(state == STATE_SELECTED)
					{
						if(!t->wpad.ir.valid || this->IsInside(t->wpad.ir.x, t->wpad.ir.y))
						{
							if(soundClick)
								soundClick->Play();

							POINT p = {0, 0};
							if (t->wpad.ir.valid)
							{
								p.x = t->wpad.ir.x;
								p.y = t->wpad.ir.y;
							}
							Clicked(this, t->chan, p);
							ClickAndHold = true;
							return;
						}
					}
					else if(trigger[i]->type == TRIGGER_BUTTON_ONLY)
					{
						POINT p = {0, 0};
						if (t->wpad.ir.valid)
						{
							p.x = t->wpad.ir.x;
							p.y = t->wpad.ir.y;
						}

						Clicked(this, t->chan, p);
						return;
					}
				}
			}
		}
	}

	if(holdable)
	{
		bool held = false;

		for(int i = 0; i < 4; i++)
		{
			//! if already found a trigger that takes effect stop searching
			if(held || !trigger[i])
				continue;

			if((t->wpad.btns_h & trigger[i]->wpad.btns_h) || (t->pad.btns_h & trigger[i]->pad.btns_h))
			{
				//! TRIGGER_BUTTON_ONLY_HELD is executed on every chan
				if(trigger[i]->type == TRIGGER_BUTTON_ONLY_HELD)
				{
					held = true;
					if(state != STATE_HELD)
						this->SetState(STATE_HELD, t->chan);
				}

				//! TRIGGER_HELD is executed only if holding the button
				else if(   (trigger[i]->type == TRIGGER_HELD)
						&& (   (trigger[i]->chan == -1)
							|| (trigger[i]->chan == t->chan)))
				{
					held = ClickAndHold;
					if(held && (state != STATE_HELD) && (state == STATE_SELECTED))
						this->SetState(STATE_HELD, t->chan);
				}
			}
		}

		if(held && (state == STATE_HELD) && (stateChan == t->chan))
		{
			POINT p = {0, 0};
			if (t->wpad.ir.valid)
			{
				p.x = t->wpad.ir.x;
				p.y = t->wpad.ir.y;
			}
			Held(this, t->chan, p);
			return;
		}
		else if(!held && (state == STATE_HELD) && (stateChan == t->chan))
		{
			POINT p = {0, 0};
			if (t->wpad.ir.valid)
			{
				p.x = t->wpad.ir.x;
				p.y = t->wpad.ir.y;
			}
			this->ResetState();
			Released(this, t->chan, p);
			ClickAndHold = false;
			return;
		}
	}
}
