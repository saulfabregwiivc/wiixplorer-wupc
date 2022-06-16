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
#ifndef _IMAGE_CONVERTER_GUI_H_
#define _IMAGE_CONVERTER_GUI_H_

#include "GUI/gui_arrowoption.h"
#include "Controls/Task.hpp"
#include "ImageConverter.hpp"

class ImageConverterGui : public GuiFrame, public ImageConverter, public sigslot::has_slots<>
{
	public:
		ImageConverterGui(const char * filepath);
		ImageConverterGui(const u8 * imgBuf, int imgSize);
		virtual ~ImageConverterGui();
		bool LoadImage(const char * filepath);
	protected:
		void OnFinishedImageLoad(u8 *buffer, u32 buffer_size);
		void OnFinishedConvertTask(Task *task);

		void OnButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnOptionLeftClick(GuiElement *sender, int pointer, const POINT &p);
		void OnOptionRightClick(GuiElement *sender, int pointer, const POINT &p);
		void OnOptionButtonClick(GuiElement *sender, int pointer, const POINT &p);
		void SetOptionValues();
		void Setup();

		GuiSound * btnClick;
		GuiSound * btnSoundOver;

		GuiButton * ConvertBtn;
		GuiButton * ResetBtn;
		GuiButton * BackBtn;
		GuiButton * AdressBarInput;
		GuiButton * AdressBarOutput;

		GuiText * TitleTxt;
		GuiText * ConvertBtnTxt;
		GuiText * ResetBtnTxt;
		GuiText * AdressBarInputName;
		GuiText * AdressBarOutputName;
		GuiText * AdressBarInputText;
		GuiText * AdressBarOutputText;

		GuiImage * bgWindowImg;
		GuiImage * ConvertBtnImg;
		GuiImage * ResetBtnImg;
		GuiImage * BackBtnImg;
		GuiImage * BackBtnImgOver;
		GuiImage * AdressBarInputImg;
		GuiImage * AdressBarOutputImg;

		GuiImageData * bgWindow;
		GuiImageData * btnOutline;
		GuiImageData * CloseImgData;
		GuiImageData * CloseImgOverData;
		GuiImageData * AdressBarData;

		SimpleGuiTrigger * trigA;
		GuiTrigger * trigB;

		GuiArrowOption Options;
};

#endif
