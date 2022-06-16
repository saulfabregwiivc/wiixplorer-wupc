/****************************************************************************
 * Copyright (C) 2009-2012 Dimok
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
#ifndef _HOMEMENU_H
#define _HOMEMENU_H

#include "GUI/gui.h"

class HomeMenu : public GuiFrame, public sigslot::has_slots<>
{
	public:
		HomeMenu();
		virtual ~HomeMenu();
		void hide();
		bool IsAnimated() const {
			return (GuiFrame::IsAnimated() || ExitBtn->IsAnimated() || HomeOutSnd->IsPlaying());
		}
		void Draw();
	private:
		void OnStateChange(GuiElement *sender, int state, int stateChan);
		void OnButtonClick(GuiButton *sender, int pointer, const POINT &p);

		GuiImageData * TopBtnImgData;
		GuiImageData * TopBtnOverImgData;
		GuiImageData * BottomBtnImgData;
		GuiImageData * BottomBtnOverImgData;
		GuiImageData * CloseBtnImgData;
		GuiImageData * StandardBtnImgData;
		GuiImageData * WiimoteBtnImgData;
		GuiImageData * BatteryImgData;
		GuiImageData * BatteryBarImgData;

		GuiImage * TopBtnImg;
		GuiImage * TopBtnOverImg;
		GuiImage * BottomBtnImg;
		GuiImage * BottomBtnOverImg;
		GuiImage * CloseBtnImg;
		GuiImage * ExitBtnImg;
		GuiImage * ShutdownBtnImg;
		GuiImage * WiimoteBtnImg;
		GuiImage * BatteryImg[4];
		GuiImage * BatteryBarImg[4];

		GuiText * TitleText;
		GuiText * CloseBtnText;
		GuiText * ExitBtnText;
		GuiText * ShutdownBtnText;
		GuiText * PlayerText[4];

		GuiButton * TopBtn;
		GuiButton * BottomBtn;
		GuiButton * CloseBtn;
		GuiButton * ExitBtn;
		GuiButton * ShutdownBtn;
		GuiButton * WiimoteBtn;
		GuiButton * BatteryBtn[4];

		SimpleGuiTrigger * trigA;
		GuiTrigger * trigHome;

		GuiSound * ButtonOverSnd;
		GuiSound * ButtonClickSnd;
		GuiSound * HomeInSnd;
		GuiSound * HomeOutSnd;
};

#endif // _HOMEMENU_H
