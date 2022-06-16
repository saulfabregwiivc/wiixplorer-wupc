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
#ifndef GUI_TRIGGER_H_
#define GUI_TRIGGER_H_

#include "stdafx.h"

enum
{
	TRIGGER_SIMPLE,
	TRIGGER_HELD,
	TRIGGER_BUTTON_ONLY,
	TRIGGER_BUTTON_ONLY_HELD
};

typedef struct _paddata {
	u16 btns_d;
	u16 btns_u;
	u16 btns_h;
	s8 stickX;
	s8 stickY;
	s8 substickX;
	s8 substickY;
	u8 triggerL;
	u8 triggerR;
} PADData;

typedef struct _wupcfulldata {
	u32 btns_d;
	u32 btns_u;
	u32 btns_h;
	s16 stickX;
	s16 stickY;
	s16 substickX;
	s16 substickY;
} WUPCFullData;

//!Menu input trigger management. Determine if action is neccessary based on input data by comparing controller input data to a specific trigger element.
class GuiTrigger
{
	public:
		//!Constructor
		GuiTrigger();
		//!Destructor
		virtual ~GuiTrigger();
		//!Sets a simple trigger. Requires: element is selected, and trigger button is pressed
		//!\param ch Controller channel number
		//!\param wiibtns Wii controller trigger button(s) - classic controller buttons are considered separately
		//!\param gcbtns GameCube controller trigger button(s)
		void SetSimpleTrigger(s32 ch, u32 wiibtns, u16 gcbtns);
		//!Sets a held trigger. Requires: element is selected, and trigger button is pressed
		//!\param ch Controller channel number
		//!\param wiibtns Wii controller trigger button(s) - classic controller buttons are considered separately
		//!\param gcbtns GameCube controller trigger button(s)
		void SetHeldTrigger(s32 ch, u32 wiibtns, u16 gcbtns);
		//!Sets a button-only trigger. Requires: Trigger button is pressed
		//!\param ch Controller channel number
		//!\param wiibtns Wii controller trigger button(s) - classic controller buttons are considered separately
		//!\param gcbtns GameCube controller trigger button(s)
		void SetButtonOnlyTrigger(s32 ch, u32 wiibtns, u16 gcbtns);
		void SetButtonOnlyHeldTrigger(s32 ch, u32 wiibtns, u16 gcbtns);
		//!Get X/Y value from Wii Joystick (classic, nunchuk) input
		//!\param right Controller stick (left = 0, right = 1)
		//!\param axis Controller stick axis (x-axis = 0, y-axis = 1)
		//!\return Stick value
		s8 WPAD_Stick(u8 right, int axis);
		//!Move menu selection left (via pad/joystick). Allows scroll delay and button overriding
		//!\return true if selection should be moved left, false otherwise
		bool Left();
		//!Move menu selection right (via pad/joystick). Allows scroll delay and button overriding
		//!\return true if selection should be moved right, false otherwise
		bool Right();
		//!Move menu selection up (via pad/joystick). Allows scroll delay and button overriding
		//!\return true if selection should be moved up, false otherwise
		bool Up();
		//!Move menu selection down (via pad/joystick). Allows scroll delay and button overriding
		//!\return true if selection should be moved down, false otherwise
		bool Down();

		u8 type; //!< trigger type (TRIGGER_SIMPLE,	TRIGGER_HELD, TRIGGER_BUTTON_ONLY, TRIGGER_BUTTON_ONLY_IN_FOCUS)
		s32 chan; //!< Trigger controller channel (0-3, -1 for all)
		WPADData wpad; //!< Wii controller trigger
		PADData pad; //!< GameCube controller trigger data
		WUPCFullData wupc;
};

class SimpleGuiTrigger : public GuiTrigger
{
	public:
		SimpleGuiTrigger(s32 ch, u32 wiibtns, u16 gcbtns)
		{
			SetSimpleTrigger(ch, wiibtns, gcbtns);
		}
};

#endif
