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
#include "Controls/Application.h"
#include "Memory/Resources.h"
#include "Controls/ThreadedTaskHandler.hpp"
#include "Controls/Taskbar.h"
#include "FileOperations/FileLoadTask.h"
#include "Prompts/ProgressWindow.h"
#include "Prompts/PromptWindows.h"
#include "TextOperations/wstring.hpp"
#include "ImageConverterGUI.hpp"
#include "ImageConvertTask.h"


ImageConverterGui::ImageConverterGui(const char * filepath)
	: GuiFrame(0,0), ImageConverter(NULL)
{
	Setup();
	LoadImage(filepath);
}


ImageConverterGui::ImageConverterGui(const u8 * imgBuf, int imgSize)
	: GuiFrame(0,0), ImageConverter(NULL, 0)
{
	Setup();
	ImageConverter::LoadImage(imgBuf, imgSize);
}

ImageConverterGui::~ImageConverterGui()
{
	RemoveAll();

	delete AdressBarInput;
	delete AdressBarOutput;
	delete ConvertBtn;
	delete ResetBtn;
	delete BackBtn;

	delete TitleTxt;
	delete ConvertBtnTxt;
	delete ResetBtnTxt;
	delete AdressBarInputText;
	delete AdressBarOutputText;
	delete AdressBarInputName;
	delete AdressBarOutputName;

	delete bgWindowImg;
	delete ConvertBtnImg;
	delete ResetBtnImg;
	delete BackBtnImg;
	delete BackBtnImgOver;
	delete AdressBarInputImg;
	delete AdressBarOutputImg;

	Resources::Remove(btnClick);
	Resources::Remove(btnSoundOver);

	Resources::Remove(bgWindow);
	Resources::Remove(btnOutline);
	Resources::Remove(CloseImgData);
	Resources::Remove(CloseImgOverData);
	Resources::Remove(AdressBarData);

	delete trigA;
	delete trigB;
}

bool ImageConverterGui::LoadImage(const char *filepath)
{
	ClearMemory();

	if(!filepath)
		return false;

	ImagePath = filepath;
	SetOutPath(ImagePath.c_str());

	FileLoadTask *task = new FileLoadTask(ImagePath, false);
	task->LoadingComplete.connect(this, &ImageConverterGui::OnFinishedImageLoad);
	task->SetAutoDelete(true);
	Taskbar::Instance()->AddTask(task);
	ThreadedTaskHandler::Instance()->AddTask(task);

	return true;
}

void ImageConverterGui::OnFinishedImageLoad(u8 *buffer, u32 buffer_size)
{
	bool result = ImageConverter::LoadImage(buffer, buffer_size);

	if(buffer)
		free(buffer);

	if(result)
		SetOptionValues();
}

void ImageConverterGui::OnFinishedConvertTask(Task *task)
{
	std::string imgPathCpy = ImagePath;
	std::string outPathCpy = OutPath;
	//reset Image
	LoadImage(imgPathCpy.c_str());
	SetOutPath(outPathCpy.c_str());
	SetOptionValues();

	ConvertBtn->SetClickable(true);
	BackBtn->SetClickable(true);

	Application::Instance()->PushForDelete(task);
}

void ImageConverterGui::Setup()
{
	btnClick = Resources::GetSound("button_click.wav");
	btnSoundOver = Resources::GetSound("button_over.wav");

	bgWindow = Resources::GetImageData("bg_properties.png");
	bgWindowImg = new GuiImage(bgWindow);
	bgWindowImg->SetScaleY(1.21);
	width = bgWindow->GetWidth();
	height = bgWindow->GetHeight();
	Append(bgWindowImg);

	AdressBarData = Resources::GetImageData("addressbar_small.png");
	btnOutline = Resources::GetImageData("small_button.png");
	CloseImgData = Resources::GetImageData("close.png");
	CloseImgOverData = Resources::GetImageData("close_over.png");

	trigA = new SimpleGuiTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigB = new GuiTrigger();
	trigB->SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);

	TitleTxt = new GuiText(tr("Image Converter"), 20, (GXColor){0, 0, 0, 255});
	TitleTxt->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	TitleTxt->SetPosition(0, -25);
	Append(TitleTxt);

	ConvertBtnTxt = new GuiText(tr("Convert"), 16, (GXColor){0, 0, 0, 255});
	ConvertBtnImg = new GuiImage(btnOutline);
	ConvertBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	ConvertBtn->SetLabel(ConvertBtnTxt);
	ConvertBtn->SetImage(ConvertBtnImg);
	ConvertBtn->SetSoundOver(btnSoundOver);
	ConvertBtn->SetSoundClick(btnClick);
	ConvertBtn->SetTrigger(trigA);
	ConvertBtn->SetPosition(-100, 365);
	ConvertBtn->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	ConvertBtn->SetEffectGrow();
	ConvertBtn->Clicked.connect(this, &ImageConverterGui::OnButtonClick);
	Append(ConvertBtn);

	ResetBtnTxt = new GuiText(tr("Reset"), 16, (GXColor){0, 0, 0, 255});
	ResetBtnImg = new GuiImage(btnOutline);
	ResetBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	ResetBtn->SetLabel(ResetBtnTxt);
	ResetBtn->SetImage(ResetBtnImg);
	ResetBtn->SetSoundOver(btnSoundOver);
	ResetBtn->SetSoundClick(btnClick);
	ResetBtn->SetTrigger(trigA);
	ResetBtn->SetPosition(100, 365);
	ResetBtn->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	ResetBtn->SetEffectGrow();
	ResetBtn->Clicked.connect(this, &ImageConverterGui::OnButtonClick);
	Append(ResetBtn);

	BackBtnImg = new GuiImage(CloseImgData);
	BackBtnImgOver = new GuiImage(CloseImgOverData);
	BackBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	BackBtn->SetImage(BackBtnImg);
	BackBtn->SetImageOver(BackBtnImgOver);
	BackBtn->SetSoundOver(btnSoundOver);
	BackBtn->SetSoundClick(btnClick);
	BackBtn->SetTrigger(trigA);
	BackBtn->SetTrigger(trigB);
	BackBtn->SetPosition(390, -31);
	BackBtn->SetEffectGrow();
	BackBtn->Clicked.connect(this, &ImageConverterGui::OnButtonClick);
	Append(BackBtn);

	/** AdressBarInput **/
	AdressBarInputName = new GuiText(tr("Input File"), 18, (GXColor){0, 0, 0, 255});
	AdressBarInputName->SetPosition(30, 5-9+AdressBarData->GetHeight()/2);
	AdressBarInputName->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Append(AdressBarInputName);

	AdressBarInputImg = new GuiImage(AdressBarData);
	AdressBarInputText = new GuiText(ImagePath.c_str(), 16, (GXColor){0, 0, 0, 255});
	AdressBarInput = new GuiButton(AdressBarInputImg->GetWidth(), AdressBarInputImg->GetHeight());
	AdressBarInput->SetLabel(AdressBarInputText);
	AdressBarInput->SetImage(AdressBarInputImg);
	AdressBarInput->SetSoundOver(btnSoundOver);
	AdressBarInput->SetSoundClick(btnClick);
	AdressBarInput->SetTrigger(trigA);
	AdressBarInput->SetPosition(150, 10);
	AdressBarInputText->SetMaxWidth(AdressBarInputImg->GetWidth()-40, SCROLL_HORIZONTAL);
	AdressBarInput->Clicked.connect(this, &ImageConverterGui::OnButtonClick);
	Append(AdressBarInput);

	/** AdressBarOuput **/
	AdressBarOutputName = new GuiText(tr("Output File"), 18, (GXColor){0, 0, 0, 255});
	AdressBarOutputName->SetPosition(30, 60-9+AdressBarData->GetHeight()/2);
	AdressBarOutputName->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	Append(AdressBarOutputName);

	AdressBarOutputImg = new GuiImage(AdressBarData);
	AdressBarOutputText = new GuiText(OutPath.c_str(), 16, (GXColor){0, 0, 0, 255});
	AdressBarOutput = new GuiButton(AdressBarOutputImg->GetWidth(), AdressBarOutputImg->GetHeight());
	AdressBarOutput->SetLabel(AdressBarOutputText);
	AdressBarOutput->SetImage(AdressBarOutputImg);
	AdressBarOutput->SetSoundOver(btnSoundOver);
	AdressBarOutput->SetSoundClick(btnClick);
	AdressBarOutput->SetTrigger(trigA);
	AdressBarOutput->SetPosition(150, 60);
	AdressBarOutputText->SetMaxWidth(AdressBarOutputImg->GetWidth()-40, SCROLL_HORIZONTAL);
	AdressBarOutput->Clicked.connect(this, &ImageConverterGui::OnButtonClick);
	Append(AdressBarOutput);

	/** Options **/
	Options.ClickedLeft.connect(this, &ImageConverterGui::OnOptionLeftClick);
	Options.ClickedRight.connect(this, &ImageConverterGui::OnOptionRightClick);
	Options.ClickedButton.connect(this, &ImageConverterGui::OnOptionButtonClick);

	int PositionX = 80;
	int PositionY = 115;
	Options.AddOption(tr("Output Type"), PositionX, PositionY);
	Options.AddOption(tr("Image Width"), PositionX+130, PositionY);
	Options.AddOption(tr("Image Heigth"), PositionX+260, PositionY);

	Options.AddOption(tr("JPEG Quality"), PositionX, PositionY+60);
	Options.AddOption(tr("BMP Compression"), PositionX+130, PositionY+60);
	Options.AddOption(tr("GD2 Compression"), PositionX+260, PositionY+60);

	Options.AddOption(tr("Alpha Blending"), PositionX, PositionY+120);
	Options.AddOption(tr("Save Alpha"), PositionX+130, PositionY+120);
	Options.AddOption(tr("Interlace"), PositionX+260, PositionY+120);

	Options.AddOption(tr("Rotate"), PositionX+65, PositionY+180);
	Options.AddOption(tr("Flip Mode"), PositionX+195, PositionY+180);
	SetOptionValues();

	Append(&Options);

	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);
}

void ImageConverterGui::SetOptionValues()
{
	AdressBarInputText->SetText(ImagePath.c_str());
	AdressBarOutputText->SetText(OutPath.c_str());

	int i = 0;

	if(OutputType == IMAGE_PNG)
		Options.SetOptionValue(i++, "PNG");
	else if(OutputType == IMAGE_JPEG)
		Options.SetOptionValue(i++, "JPEG");
	else if(OutputType == IMAGE_GIF)
		Options.SetOptionValue(i++, "GIF");
	else if(OutputType == IMAGE_TIFF)
		Options.SetOptionValue(i++, "TIFF");
	else if(OutputType == IMAGE_BMP)
		Options.SetOptionValue(i++, "BMP");
	else if(OutputType == IMAGE_GD)
		Options.SetOptionValue(i++, "GD");
	else if(OutputType == IMAGE_GD2)
		Options.SetOptionValue(i++, "GD2");


	Options.SetOptionValue(i++, fmt("%i", OutputWidth));
	Options.SetOptionValue(i++, fmt("%i", OutputHeight));
	Options.SetOptionValue(i++, fmt("%i", JPEG_Quality));
	Options.SetOptionValue(i++, fmt("%i", BMP_Compression));

	if(GD2_Compression)
		Options.SetOptionValue(i++, tr("ON"));
	else
		Options.SetOptionValue(i++, tr("OFF"));

	if(AlphaBlending)
		Options.SetOptionValue(i++, tr("ON"));
	else
		Options.SetOptionValue(i++, tr("OFF"));

	if(SaveAlpha)
		Options.SetOptionValue(i++, tr("ON"));
	else
		Options.SetOptionValue(i++, tr("OFF"));

	if(Interlace)
		Options.SetOptionValue(i++, tr("ON"));
	else
		Options.SetOptionValue(i++, tr("OFF"));


	Options.SetOptionValue(i++, fmt("%i", Angle));

	if(FlipMode == NONE)
		Options.SetOptionValue(i++, tr("NONE"));
	else if(FlipMode == FLIP_VERTICAL)
		Options.SetOptionValue(i++, tr("Vertical"));
	else if(FlipMode == FLIP_HORIZONTAL)
		Options.SetOptionValue(i++, tr("Horizontal"));
	else if(FlipMode == FLIP_BOTH)
		Options.SetOptionValue(i++, tr("Both"));
}

void ImageConverterGui::OnButtonClick(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	if(sender == BackBtn)
	{
		SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
		Application::Instance()->PushForDelete(this);
	}
	else if(sender == ResetBtn)
	{
		ResetOptions();
		SetOutPath(ImagePath.c_str());
		SetOptionValues();
	}
	else if(sender == ConvertBtn)
	{
		ConvertBtn->SetClickable(false);
		BackBtn->SetClickable(false);

		ImageConvertTask *task = new ImageConvertTask(this, false);
		task->TaskEnd.connect(this, &ImageConverterGui::OnFinishedConvertTask);
		Taskbar::Instance()->AddTask(task);
		ThreadedTaskHandler::Instance()->AddTask(task);
	}
	else if(sender == AdressBarInput)
	{
		char newpath[200];
		memset(newpath, 0, sizeof(newpath));

		const wchar_t * wText = AdressBarInputText->GetText();
		if(wText)
		{
			wString currentPath(wText);
			snprintf(newpath, sizeof(newpath), "%s", currentPath.toUTF8().c_str());
		}

		int result = OnScreenKeyboard(newpath, 150);
		if(result)
		{
			LoadImage(newpath);
			SetOptionValues();
		}
	}
	else if(sender == AdressBarOutput)
	{
		char newpath[200];
		memset(newpath, 0, sizeof(newpath));

		const wchar_t * wText = AdressBarOutputText->GetText();
		if(wText)
		{
			wString currentPath(wText);
			snprintf(newpath, sizeof(newpath), "%s", currentPath.toUTF8().c_str());
		}

		int result = OnScreenKeyboard(newpath, 150);
		if(result)
		{
			OutPath = newpath;
			SetOptionValues();
		}
	}
}

void ImageConverterGui::OnOptionLeftClick(GuiElement *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	for(int i = 0; i < Options.GetOptionCount(); i++)
	{
		if(sender == Options.GetButtonLeft(i))
		{
			if(i == 0)
			{
				OutputType = LIMIT(OutputType-1, 0, 7);
				SetOutPath(NULL);
			}
			else if(i == 1)
			{
				OutputWidth = LIMIT(OutputWidth-1, 0, 9999);
			}
			else if(i == 2)
			{
				OutputHeight = LIMIT(OutputHeight-1, 0, 9999);
			}
			else if(i == 3)
			{
				JPEG_Quality = LIMIT(JPEG_Quality-1, 0, 100);
			}
			else if(i == 4)
			{
				BMP_Compression = LIMIT(BMP_Compression-1, 0, 9);
			}
			else if(i == 5)
			{
				GD2_Compression = LIMIT(GD2_Compression-1, 0, 1);
			}
			else if(i == 6)
			{
				AlphaBlending = LIMIT(AlphaBlending-1, 0, 1);
			}
			else if(i == 7)
			{
				SaveAlpha = LIMIT(SaveAlpha-1, 0, 1);
			}
			else if(i == 8)
			{
				Interlace = LIMIT(Interlace-1, 0, 1);
			}
			else if(i == 9)
			{
				Angle = LIMIT(Angle-1, 0, 360);
			}
			else if(i == 10)
			{
				FlipMode = LIMIT(FlipMode-1, NONE, FLIP_BOTH);
			}
		}
	}

	SetOptionValues();
}

void ImageConverterGui::OnOptionRightClick(GuiElement *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	for(int i = 0; i < Options.GetOptionCount(); i++)
	{
		if(sender == Options.GetButtonRight(i))
		{
			if(i == 0)
			{
				OutputType = LIMIT(OutputType+1, 0, 6);
				SetOutPath(NULL);
			}
			else if(i == 1)
			{
				OutputWidth = LIMIT(OutputWidth+1, 0, 9999);
			}
			else if(i == 2)
			{
				OutputHeight = LIMIT(OutputHeight+1, 0, 9999);
			}
			else if(i == 3)
			{
				JPEG_Quality = LIMIT(JPEG_Quality+1, 0, 100);
			}
			else if(i == 4)
			{
				BMP_Compression = LIMIT(BMP_Compression+1, 0, 9);
			}
			else if(i == 5)
			{
				GD2_Compression = LIMIT(GD2_Compression+1, 0, 1);
			}
			else if(i == 6)
			{
				AlphaBlending = LIMIT(AlphaBlending+1, 0, 1);
			}
			else if(i == 7)
			{
				SaveAlpha = LIMIT(SaveAlpha+1, 0, 1);
			}
			else if(i == 8)
			{
				Interlace = LIMIT(Interlace+1, 0, 1);
			}
			else if(i == 9)
			{
				Angle = LIMIT(Angle+1, 0, 360);
			}
			else if(i == 10)
			{
				FlipMode = LIMIT(FlipMode+1, NONE, FLIP_BOTH);
			}
		}
	}

	SetOptionValues();
}

void ImageConverterGui::OnOptionButtonClick(GuiElement *sender, int pointer, const POINT &p)
{
	for(int i = 0; i < Options.GetOptionCount(); i++)
	{
		if(sender == Options.GetButton(i))
		{
			if((i >= 1 && i <= 4) || i == 9)
			{
				char newpath[200];
				memset(newpath, 0, sizeof(newpath));

				switch(i)
				{
					case 1: {
						snprintf(newpath, sizeof(newpath), "%i", OutputWidth);
						int result = OnScreenKeyboard(newpath, 150);
						if(result)
						{
							OutputWidth = LIMIT(atoi(newpath), 0, 9999);
							SetOptionValues();
						}
						break;
					}
					case 2: {
						snprintf(newpath, sizeof(newpath), "%i", OutputHeight);
						int result = OnScreenKeyboard(newpath, 150);
						if(result)
						{
							OutputHeight = LIMIT(atoi(newpath), 0, 9999);
							SetOptionValues();
						}
						break;
					}
					case 3: {
						snprintf(newpath, sizeof(newpath), "%i", JPEG_Quality);
						int result = OnScreenKeyboard(newpath, 150);
						if(result)
						{
							JPEG_Quality = LIMIT(atoi(newpath), 0, 100);
							SetOptionValues();
						}
						break;
					}
					case 4: {
						snprintf(newpath, sizeof(newpath), "%i", BMP_Compression);
						int result = OnScreenKeyboard(newpath, 150);
						if(result)
						{
							BMP_Compression = LIMIT(atoi(newpath), 0, 9);
							SetOptionValues();
						}
						break;
					}
					case 9: {
						snprintf(newpath, sizeof(newpath), "%i", Angle);
						int result = OnScreenKeyboard(newpath, 150);
						if(result)
						{
							Angle = LIMIT(atoi(newpath), 0, 360);
							SetOptionValues();
						}
						break;
					}
					default:
						break;
				}
			}
			else
			{
				OnOptionRightClick(sender, pointer, p);
			}
		}
	}
}
