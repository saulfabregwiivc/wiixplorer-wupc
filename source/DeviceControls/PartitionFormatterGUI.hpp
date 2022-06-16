/***************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 * for WiiXplorer 2010
 ***************************************************************************/
#ifndef _PARTITION_FORMATTER_GUI_H_
#define _PARTITION_FORMATTER_GUI_H_

#include "GUI/gui.h"
#include "GUI/gui_arrowoption.h"
#include "DeviceControls/DeviceHandler.hpp"

class PartitionFormatterGui : public GuiFrame, public sigslot::has_slots<>
{
	public:
		PartitionFormatterGui();
		virtual ~PartitionFormatterGui();
		void MainUpdate();
	protected:
		void OnActiveButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnBackButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnFormatButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnOptionLeftClick(GuiElement *sender, int pointer, const POINT &p);
		void OnOptionRightClick(GuiElement *sender, int pointer, const POINT &p);
		void OnFormatFinish(int result);
		void SetDevice();
		void ListPartitions();

		int currentState;
		int CurDevice;
		int CurPart;
		int formatResult;

		PartitionHandle * Device;

		GuiSound * btnClick;
		GuiSound * btnSoundOver;

		GuiButton * FormatBtn;
		GuiButton * ActiveBtn;
		GuiButton * BackBtn;

		GuiText * TitleTxt;
		GuiText * MountNameTxt;
		GuiText * PartitionTxt;
		GuiText * PartActiveTxt;
		GuiText * PartTypeTxt;
		GuiText * PartSizeTxt;
		GuiText * MountNameValTxt;
		GuiText * PartitionValTxt;
		GuiText * PartActiveValTxt;
		GuiText * PartTypeValTxt;
		GuiText * PartSizeValTxt;
		GuiText * FormatBtnTxt;
		GuiText * ActiveBtnTxt;

		GuiImage * bgWindowImg;
		GuiImage * ActiveBtnImg;
		GuiImage * FormatBtnImg;
		GuiImage * BackBtnImg;
		GuiImage * BackBtnImgOver;

		GuiImageData * bgWindow;
		GuiImageData * btnOutline;
		GuiImageData * CloseImgData;
		GuiImageData * CloseImgOverData;

		SimpleGuiTrigger * trigA;
		GuiTrigger * trigB;

		GuiArrowOption DeviceOption;
};

#endif
