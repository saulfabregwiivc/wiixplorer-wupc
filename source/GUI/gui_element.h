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
#ifndef GUI_ELEMENT_H_
#define GUI_ELEMENT_H_

#include "stdafx.h"
#include "Tools/Rect.h"

enum
{
	EFFECT_NONE				= 0x00,
	EFFECT_SLIDE_TOP		= 0x01,
	EFFECT_SLIDE_BOTTOM		= 0x02,
	EFFECT_SLIDE_RIGHT		= 0x04,
	EFFECT_SLIDE_LEFT		= 0x08,
	EFFECT_SLIDE_IN			= 0x10,
	EFFECT_SLIDE_OUT		= 0x20,
	EFFECT_SLIDE_FROM		= 0x40,
	EFFECT_FADE				= 0x80,
	EFFECT_SCALE			= 0x100,
	EFFECT_COLOR_TRANSITION	= 0x200
};

enum
{
	ALIGN_LEFT			= 0x01,
	ALIGN_CENTER		= 0x02,
	ALIGN_RIGHT			= 0x04,
	ALIGN_TOP			= 0x10,
	ALIGN_MIDDLE		= 0x20,
	ALIGN_BOTTOM		= 0x40,
	ALIGN_TOP_LEFT		= ALIGN_LEFT | ALIGN_TOP,
	ALIGN_TOP_CENTER	= ALIGN_CENTER | ALIGN_TOP,
	ALIGN_TOP_RIGHT		= ALIGN_RIGHT | ALIGN_TOP,
	ALIGN_CENTERED		= ALIGN_CENTER | ALIGN_MIDDLE,
};

enum
{
	STATE_DEFAULT,
	STATE_SELECTED,
	STATE_CLICKED,
	STATE_HELD,
	STATE_DISABLED,
	STATE_CLOSED
};

enum
{
	WRAP,
	DOTTED,
	SCROLL_HORIZONTAL,
	SCROLL_NONE
};

typedef struct _POINT {
	s32 x;
	s32 y;
} POINT;

//!Forward declaration
class GuiTrigger;

//!Primary GUI class. Most other classes inherit from this class.
class GuiElement
{
	public:
		//!Constructor
		GuiElement();
		//!Destructor
		virtual ~GuiElement() {}
		//!Set the element's parent
		//!\param e Pointer to parent element
		virtual void SetParent(GuiElement * e) { parentElement = e; }
		//!Gets the element's parent
		//!\return Pointer to parent element
		virtual GuiElement * GetParent() { return parentElement; }
		//!Gets the current leftmost coordinate of the element
		//!Considers horizontal alignment, x offset, width, and parent element's GetLeft() / GetWidth() values
		//!\return left coordinate
		virtual int GetLeft();
		//!Gets the current topmost coordinate of the element
		//!Considers vertical alignment, y offset, height, and parent element's GetTop() / GetHeight() values
		//!\return top coordinate
		virtual int GetTop();
		//!Gets the current Z coordinate of the element
		//!\return Z coordinate
		virtual int GetZPosition()
		{
			int zParent = 0;

			if(parentElement)
				zParent = parentElement->GetZPosition();

			return zParent+zoffset;
		}
		//!Gets elements xoffset
		virtual int GetLeftPos() { return xoffset; }
		//!Gets elements yoffset
		virtual int GetTopPos() { return yoffset; }
		//!Gets the current width of the element. Does not currently consider the scale
		//!\return width
		virtual int GetWidth() { return width; };
		//!Gets the height of the element. Does not currently consider the scale
		//!\return height
		virtual int GetHeight() { return height; }
		//!Sets the size (width/height) of the element
		//!\param w Width of element
		//!\param h Height of element
		virtual void SetSize(int w, int h)
		{
			width = w;
			height = h;
		}
		//!Sets the element's visibility
		//!\param v Visibility (true = visible)
		virtual void SetVisible(bool v)
		{
			visible = v;
			VisibleChanged(this, v);
		}
		//!Checks whether or not the element is visible
		//!\return true if visible, false otherwise
		virtual bool IsVisible() { return visible; };
		//!Checks whether or not the element is selectable
		//!\return true if selectable, false otherwise
		virtual bool IsSelectable()
		{
			return (	state != STATE_DISABLED
					&&	state != STATE_CLICKED
					&&	selectable);
		}
		//!Checks whether or not the element is clickable
		//!\return true if clickable, false otherwise
		virtual bool IsClickable()
		{
			return (	state != STATE_DISABLED
					&&	state != STATE_CLICKED
					&&	state != STATE_HELD
					&&	clickable);
		}
		//!Checks whether or not the element is holdable
		//!\return true if holdable, false otherwise
		virtual bool IsHoldable() { return (state != STATE_DISABLED) && holdable; }
		//!Sets whether or not the element is selectable
		//!\param s Selectable
		virtual void SetSelectable(bool s) { selectable = s; }
		//!Sets whether or not the element is clickable
		//!\param c Clickable
		virtual void SetClickable(bool c) { clickable = c; }
		//!Sets whether or not the element is holdable
		//!\param c Holdable
		virtual void SetHoldable(bool d) { holdable = d; }
		//!Sets the element's state
		//!\param s State (STATE_DEFAULT, STATE_SELECTED, STATE_CLICKED, STATE_DISABLED)
		//!\param c Controller channel (0-3, -1 = none)
		virtual void SetState(int s, int c = -1);
		//!Gets the element's current state
		//!\return state
		virtual int GetState() { return state; };
		//!Gets the controller channel that last changed the element's state
		//!\return Channel number (0-3, -1 = no channel)
		virtual int GetStateChan() { return stateChan; };
		//!Resets the element's state to STATE_DEFAULT
		virtual void ResetState()
		{
			if(state != STATE_DISABLED)
			{
				state = STATE_DEFAULT;
				stateChan = -1;
			}
		}
		//!Sets the element's alpha value
		//!\param a alpha value
		virtual void SetAlpha(int a) { alpha = a; }
		//!Gets the element's alpha value
		//!Considers alpha, alphaDyn, and the parent element's GetAlpha() value
		//!\return alpha
		virtual int GetAlpha()
		{
			int a;

			if(alphaDyn >= 0)
				a = alphaDyn;
			else
				a = alpha;

			if(parentElement)
				a = (a * parentElement->GetAlpha()) / 255;

			return a;
		}
		//!Sets the element's scale
		//!\param s scale (1 is 100%)
		virtual void SetScale(float s)
		{
			scaleX = s;
			scaleY = s;
		}
		//!Sets the element's scale
		//!\param s scale (1 is 100%)
		virtual void SetScaleX(float s) { scaleX = s; }
		//!Sets the element's scale
		//!\param s scale (1 is 100%)
		virtual void SetScaleY(float s) { scaleY = s; }
		//!Gets the element's current scale
		//!Considers scale, scaleDyn, and the parent element's GetScale() value
		virtual float GetScale()
		{
			float s = 0.5f * (scaleX+scaleY) * scaleDyn;

			if(parentElement)
				s *= parentElement->GetScale();

			return s;
		}
		//!Gets the element's current scale
		//!Considers scale, scaleDyn, and the parent element's GetScale() value
		virtual float GetScaleX()
		{
			float s = scaleX * scaleDyn;

			if(parentElement)
				s *= parentElement->GetScaleX();

			return s;
		}
		//!Gets the element's current scale
		//!Considers scale, scaleDyn, and the parent element's GetScale() value
		virtual float GetScaleY()
		{
			float s = scaleY * scaleDyn;

			if(parentElement)
				s *= parentElement->GetScaleY();

			return s;
		}
		//!Checks whether rumble was requested by the element
		//!\return true is rumble was requested, false otherwise
		virtual bool Rumble() { return rumble; }
		//!Sets whether or not the element is requesting a rumble event
		//!\param r true if requesting rumble, false if not
		virtual void SetRumble(bool r) { rumble = r; }
		//!Set an effect for the element
		//!\param e Effect to enable
		//!\param a Amount of the effect (usage varies on effect)
		//!\param t Target amount of the effect (usage varies on effect)
		virtual void SetEffect(int e, int a, int t=0);
		//!Sets an effect to be enabled on wiimote cursor over
		//!\param e Effect to enable
		//!\param a Amount of the effect (usage varies on effect)
		//!\param t Target amount of the effect (usage varies on effect)
		virtual void SetEffectOnOver(int e, int a, int t=0);
		//!Shortcut to SetEffectOnOver(EFFECT_SCALE, 4, 110)
		virtual void SetEffectGrow() { SetEffectOnOver(EFFECT_SCALE, 4, 110); }
		//!Reset all applied effects
		virtual void ResetEffects();
		//!Gets the current element effects
		//!\return element effects
		virtual int GetEffect() const { return effects; }
		//!\return true if element animation is on going
		virtual bool IsAnimated() const { return (parentElement != 0) && (GetEffect() > 0); }
		//!The Element's cutoff bounds
		virtual void SetBounds(iRect r) { cutBoundsRect = r; }
		//!Checks whether the specified coordinates are within the element's boundaries
		//!\param x X coordinate
		//!\param y Y coordinate
		//!\return true if contained within, false otherwise
		virtual bool IsInside(int x, int y)
		{
			return (	x > this->GetLeft()
					&&	x < (this->GetLeft()+width)
					&&	y > this->GetTop()
					&&	y < (this->GetTop()+height));
		}
		//!Sets the element's position
		//!\param x X coordinate
		//!\param y Y coordinate
		virtual void SetPosition(int x, int y)
		{
			xoffset = x;
			yoffset = y;
		}
		//!Sets the element's position
		//!\param x X coordinate
		//!\param y Y coordinate
		//!\param z Z coordinate
		virtual void SetPosition(int x, int y, int z)
		{
			xoffset = x;
			yoffset = y;
			zoffset = z;
		}
		//!Gets whether or not the element is in STATE_SELECTED
		//!\return true if selected, false otherwise
		virtual int GetSelected() { return -1; }
		//!Sets the element's alignment respective to its parent element
		//!Bitwise ALIGN_LEFT | ALIGN_RIGHT | ALIGN_CENTRE, ALIGN_TOP, ALIGN_BOTTOM, ALIGN_MIDDLE)
		//!\param align Alignment
		virtual void SetAlignment(int a) { alignment = a; }
		//!Gets the element's alignment
		virtual int GetAlignment() const { return alignment; }
		//!Called constantly to allow the element to respond to the current input data
		//!\param t Pointer to a GuiTrigger, containing the current input data from PAD/WPAD
		virtual void Update(GuiTrigger * t UNUSED) { }
		//!Called constantly to redraw the element
		virtual void Draw() { }
		//!Updates the element's effects (dynamic values)
		//!Called by Draw(), used for animation purposes
		virtual void UpdateEffects();
		//! Switch pointer from control to screen position
		POINT PtrToScreen(POINT p)
		{
			POINT r = { p.x + GetLeft(), p.y + GetTop() };
			return r;
		}
		//! Switch pointer screen to control position
		POINT PtrToControl(POINT p)
		{
			POINT r = { p.x - GetLeft(), p.y - GetTop() };
			return r;
		}
		//! Signals
		sigslot::signal2<GuiElement *, bool> VisibleChanged;
		sigslot::signal3<GuiElement *, int, int> StateChanged;
		sigslot::signal1<GuiElement *> EffectFinished;
	protected:
		bool rumble; //!< Wiimote rumble (on/off) - set to on when this element requests a rumble event
		bool visible; //!< Visibility of the element. If false, Draw() is skipped
		bool selectable; //!< Whether or not this element selectable (can change to SELECTED state)
		bool clickable; //!< Whether or not this element is clickable (can change to CLICKED state)
		bool holdable; //!< Whether or not this element is holdable (can change to HELD state)
		int width; //!< Element width
		int height; //!< Element height
		int xoffset; //!< Element X offset
		int yoffset; //!< Element Y offset
		int zoffset; //!< Element Z offset
		int alpha; //!< Element alpha value (0-255)
		f32 scaleX; //!< Element scale (1 = 100%)
		f32 scaleY; //!< Element scale (1 = 100%)
		int alignment; //!< Horizontal element alignment, respective to parent element
		int state; //!< Element state (DEFAULT, SELECTED, CLICKED, DISABLED)
		int stateChan; //!< Which controller channel is responsible for the last change in state
		iRect cutBoundsRect;
		GuiElement * parentElement; //!< Parent element

		//! TODO: Move me to some Animator class
		int xoffsetDyn; //!< Element X offset, dynamic (added to xoffset value for animation effects)
		int yoffsetDyn; //!< Element Y offset, dynamic (added to yoffset value for animation effects)
		int alphaDyn; //!< Element alpha, dynamic (multiplied by alpha value for blending/fading effects)
		f32 scaleDyn; //!< Element scale, dynamic (multiplied by alpha value for blending/fading effects)
		int effects; //!< Currently enabled effect(s). 0 when no effects are enabled
		int effectAmount; //!< Effect amount. Used by different effects for different purposes
		int effectTarget; //!< Effect target amount. Used by different effects for different purposes
		int effectsOver; //!< Effects to enable when wiimote cursor is over this element. Copied to effects variable on over event
		int effectAmountOver; //!< EffectAmount to set when wiimote cursor is over this element
		int effectTargetOver; //!< EffectTarget to set when wiimote cursor is over this element
};

#endif
