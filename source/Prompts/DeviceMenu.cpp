/***************************************************************************
 * Copyright (C) 2009-2011
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
 ***************************************************************************/
#include "DeviceMenu.h"
#include "Controls/Application.h"
#include "DeviceControls/DeviceHandler.hpp"
#include "Memory/Resources.h"
#include "network/networkops.h"
#include "Settings.h"
#include "FileOperations/filebrowser.h"
#include "Prompts/ProgressWindow.h"

DeviceMenu::DeviceMenu(int x, int y)
	:	GuiFrame(0, 0)
{
	int PositionY = 19;
	int FontSize = 17;
	deviceCount = 0;
	choice = -1;

	//! Menu imagedata
	device_choose_right_Data = Resources::GetImageData("device_choose_right.png");
	device_choose_left_Data = Resources::GetImageData("device_choose_left.png");
	device_choose_center_Data = Resources::GetImageData("device_choose_center.png");
	menu_select = Resources::GetImageData("deviceselection.png");

	//! Device imagedata
	sd_ImgData = Resources::GetImageData("sdstorage.png");
	usb_ImgData = Resources::GetImageData("usbstorage.png");
	usb_blue_ImgData = Resources::GetImageData("usbstorage_blue.png");
	smb_ImgData = Resources::GetImageData("networkstorage.png");
	ftp_ImgData = Resources::GetImageData("ftpstorage.png");
	nfs_ImgData = Resources::GetImageData("networkstorage.png");
	nand_ImgData = Resources::GetImageData("nandstorage.png");
	dvd_ImgData = Resources::GetImageData("dvdstorage.png");

	//! Menu images
	centerImg = new GuiImage(device_choose_center_Data);
	leftImg = new GuiImage(device_choose_left_Data);
	rightImg = new GuiImage(device_choose_right_Data);

	trigA = new SimpleGuiTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigB = new GuiTrigger();
	trigB->SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);

	btnClick = Resources::GetSound("button_click.wav");

	for(int i = 0; i < MAXDEVICES; i++)
	{
		deviceBtn[i] = NULL;
		deviceImgs[i] = NULL;
		deviceImgOver[i] = NULL;
		deviceText[i] = NULL;
		deviceSelection[i] = -1;
	}

	int PositionX = leftImg->GetWidth();

	if(DeviceHandler::Instance()->IsInserted(SD))
	{
		deviceText[deviceCount] = new GuiText(DeviceName[SD], FontSize, (GXColor){0, 0, 0, 255});
		deviceText[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
		deviceText[deviceCount]->SetPosition(0, 2);
		deviceImgs[deviceCount] = new GuiImage(sd_ImgData);
		deviceImgs[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
		deviceImgOver[deviceCount] = new GuiImage(menu_select);
		deviceImgOver[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
		deviceBtn[deviceCount] = new GuiButton(deviceImgs[deviceCount]->GetWidth(), deviceImgs[deviceCount]->GetHeight()+FontSize);
		deviceBtn[deviceCount]->SetLabel(deviceText[deviceCount]);
		deviceBtn[deviceCount]->SetSoundClick(btnClick);
		deviceBtn[deviceCount]->SetIcon(deviceImgs[deviceCount]);
		deviceBtn[deviceCount]->SetImageOver(deviceImgOver[deviceCount]);
		deviceBtn[deviceCount]->SetTrigger(trigA);
		deviceBtn[deviceCount]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
		deviceBtn[deviceCount]->SetPosition(PositionX, PositionY);
		deviceBtn[deviceCount]->Clicked.connect(this, &DeviceMenu::OnButtonClick);
		PositionX += deviceImgs[deviceCount]->GetWidth()+10;

		deviceSelection[deviceCount] = SD;

		deviceCount++;
	}

	if(DeviceHandler::Instance()->IsInserted(GCSDA))
	{
		deviceText[deviceCount] = new GuiText(DeviceName[GCSDA], FontSize, (GXColor){0, 0, 0, 255});
		deviceText[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
		deviceText[deviceCount]->SetPosition(0, 2);
		deviceImgs[deviceCount] = new GuiImage(sd_ImgData);
		deviceImgs[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
		deviceImgOver[deviceCount] = new GuiImage(menu_select);
		deviceImgOver[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
		deviceBtn[deviceCount] = new GuiButton(deviceImgs[deviceCount]->GetWidth(), deviceImgs[deviceCount]->GetHeight()+FontSize);
		deviceBtn[deviceCount]->SetLabel(deviceText[deviceCount]);
		deviceBtn[deviceCount]->SetSoundClick(btnClick);
		deviceBtn[deviceCount]->SetIcon(deviceImgs[deviceCount]);
		deviceBtn[deviceCount]->SetImageOver(deviceImgOver[deviceCount]);
		deviceBtn[deviceCount]->SetTrigger(trigA);
		deviceBtn[deviceCount]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
		deviceBtn[deviceCount]->SetPosition(PositionX, PositionY);
		deviceBtn[deviceCount]->Clicked.connect(this, &DeviceMenu::OnButtonClick);
		PositionX += deviceImgs[deviceCount]->GetWidth()+10;

		deviceSelection[deviceCount] = GCSDA;

		deviceCount++;
	}

	if(DeviceHandler::Instance()->IsInserted(GCSDB))
	{
		deviceText[deviceCount] = new GuiText(DeviceName[GCSDB], FontSize, (GXColor){0, 0, 0, 255});
		deviceText[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
		deviceText[deviceCount]->SetPosition(0, 2);
		deviceImgs[deviceCount] = new GuiImage(sd_ImgData);
		deviceImgs[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
		deviceImgOver[deviceCount] = new GuiImage(menu_select);
		deviceImgOver[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
		deviceBtn[deviceCount] = new GuiButton(deviceImgs[deviceCount]->GetWidth(), deviceImgs[deviceCount]->GetHeight()+FontSize);
		deviceBtn[deviceCount]->SetLabel(deviceText[deviceCount]);
		deviceBtn[deviceCount]->SetSoundClick(btnClick);
		deviceBtn[deviceCount]->SetIcon(deviceImgs[deviceCount]);
		deviceBtn[deviceCount]->SetImageOver(deviceImgOver[deviceCount]);
		deviceBtn[deviceCount]->SetTrigger(trigA);
		deviceBtn[deviceCount]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
		deviceBtn[deviceCount]->SetPosition(PositionX, PositionY);
		deviceBtn[deviceCount]->Clicked.connect(this, &DeviceMenu::OnButtonClick);
		PositionX += deviceImgs[deviceCount]->GetWidth()+10;

		deviceSelection[deviceCount] = GCSDB;

		deviceCount++;
	}

	for(int j = USB1; j <= USB8; j++)
	{
		if(DeviceHandler::Instance()->IsInserted(j))
		{
			const char * FSName = DeviceHandler::GetFSName(j);

			deviceText[deviceCount] = new GuiText(DeviceName[j], FontSize, (GXColor){0, 0, 0, 255});
			deviceText[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
			deviceText[deviceCount]->SetPosition(0, 2);
			if(FSName && strncmp(FSName, "NTF", 3) != 0)
				deviceImgs[deviceCount] = new GuiImage(usb_ImgData);
			else
				deviceImgs[deviceCount] = new GuiImage(usb_blue_ImgData);
			deviceImgs[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
			deviceImgOver[deviceCount] = new GuiImage(menu_select);
			deviceImgOver[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
			deviceBtn[deviceCount] = new GuiButton(deviceImgs[deviceCount]->GetWidth(), deviceImgs[deviceCount]->GetHeight()+FontSize);
			deviceBtn[deviceCount]->SetLabel(deviceText[deviceCount]);
			deviceBtn[deviceCount]->SetSoundClick(btnClick);
			deviceBtn[deviceCount]->SetIcon(deviceImgs[deviceCount]);
			deviceBtn[deviceCount]->SetImageOver(deviceImgOver[deviceCount]);
			deviceBtn[deviceCount]->SetTrigger(trigA);
			deviceBtn[deviceCount]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
			deviceBtn[deviceCount]->SetPosition(PositionX, PositionY);
			deviceBtn[deviceCount]->Clicked.connect(this, &DeviceMenu::OnButtonClick);
			PositionX += deviceImgs[deviceCount]->GetWidth()+10;

			deviceSelection[deviceCount] = j;

			deviceCount++;
		}
	}

	if(DeviceHandler::Instance()->IsInserted(NAND))
	{
		deviceText[deviceCount] = new GuiText(DeviceName[NAND], FontSize, (GXColor){0, 0, 0, 255});
		deviceText[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
		deviceText[deviceCount]->SetPosition(0, 2);
		deviceImgs[deviceCount] = new GuiImage(nand_ImgData);
		deviceImgs[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
		deviceImgOver[deviceCount] = new GuiImage(menu_select);
		deviceImgOver[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
		deviceBtn[deviceCount] = new GuiButton(deviceImgs[deviceCount]->GetWidth(), deviceImgs[deviceCount]->GetHeight()+FontSize);
		deviceBtn[deviceCount]->SetLabel(deviceText[deviceCount]);
		deviceBtn[deviceCount]->SetSoundClick(btnClick);
		deviceBtn[deviceCount]->SetIcon(deviceImgs[deviceCount]);
		deviceBtn[deviceCount]->SetImageOver(deviceImgOver[deviceCount]);
		deviceBtn[deviceCount]->SetTrigger(trigA);
		deviceBtn[deviceCount]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
		deviceBtn[deviceCount]->SetPosition(PositionX, PositionY);
		deviceBtn[deviceCount]->Clicked.connect(this, &DeviceMenu::OnButtonClick);
		PositionX += deviceImgs[deviceCount]->GetWidth()+10;

		deviceSelection[deviceCount] = NAND;

		deviceCount++;
	}

	if(DeviceHandler::Instance()->IsInserted(DVD))
	{
		deviceText[deviceCount] = new GuiText(DeviceName[DVD], FontSize, (GXColor){0, 0, 0, 255});
		deviceText[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
		deviceText[deviceCount]->SetPosition(0, 2);
		deviceImgs[deviceCount] = new GuiImage(dvd_ImgData);
		deviceImgs[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
		deviceImgOver[deviceCount] = new GuiImage(menu_select);
		deviceImgOver[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
		deviceBtn[deviceCount] = new GuiButton(deviceImgs[deviceCount]->GetWidth(), deviceImgs[deviceCount]->GetHeight()+FontSize);
		deviceBtn[deviceCount]->SetLabel(deviceText[deviceCount]);
		deviceBtn[deviceCount]->SetSoundClick(btnClick);
		deviceBtn[deviceCount]->SetIcon(deviceImgs[deviceCount]);
		deviceBtn[deviceCount]->SetImageOver(deviceImgOver[deviceCount]);
		deviceBtn[deviceCount]->SetTrigger(trigA);
		deviceBtn[deviceCount]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
		deviceBtn[deviceCount]->SetPosition(PositionX, PositionY);
		deviceBtn[deviceCount]->Clicked.connect(this, &DeviceMenu::OnButtonClick);
		PositionX += deviceImgs[deviceCount]->GetWidth()+10;

		deviceSelection[deviceCount] = DVD;

		deviceCount++;
	}

	for(int i = SMB1; i <= SMB10; i++)
	{
		if(DeviceHandler::Instance()->IsInserted(i))
		{
			deviceText[deviceCount] = new GuiText(DeviceName[i], FontSize, (GXColor){0, 0, 0, 255});
			deviceText[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
			deviceText[deviceCount]->SetPosition(0, 2);
			deviceImgs[deviceCount] = new GuiImage(smb_ImgData);
			deviceImgs[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
			deviceImgOver[deviceCount] = new GuiImage(menu_select);
			deviceImgOver[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
			deviceBtn[deviceCount] = new GuiButton(deviceImgs[deviceCount]->GetWidth(), deviceImgs[deviceCount]->GetHeight()+FontSize);
			deviceBtn[deviceCount]->SetLabel(deviceText[deviceCount]);
			deviceBtn[deviceCount]->SetSoundClick(btnClick);
			deviceBtn[deviceCount]->SetIcon(deviceImgs[deviceCount]);
			deviceBtn[deviceCount]->SetImageOver(deviceImgOver[deviceCount]);
			deviceBtn[deviceCount]->SetTrigger(trigA);
			deviceBtn[deviceCount]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
			deviceBtn[deviceCount]->SetPosition(PositionX, PositionY);
			deviceBtn[deviceCount]->Clicked.connect(this, &DeviceMenu::OnButtonClick);
			PositionX += deviceImgs[deviceCount]->GetWidth()+10;

			deviceSelection[deviceCount] = i;

			deviceCount++;
		}
	}

	for(int i = FTP1; i <= FTP10; i++)
	{
		if(DeviceHandler::Instance()->IsInserted(i))
		{
			deviceText[deviceCount] = new GuiText(DeviceName[i], FontSize, (GXColor){0, 0, 0, 255});
			deviceText[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
			deviceText[deviceCount]->SetPosition(0, 2);
			deviceImgs[deviceCount] = new GuiImage(ftp_ImgData);
			deviceImgs[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
			deviceImgOver[deviceCount] = new GuiImage(menu_select);
			deviceImgOver[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
			deviceBtn[deviceCount] = new GuiButton(deviceImgs[deviceCount]->GetWidth(), deviceImgs[deviceCount]->GetHeight()+FontSize);
			deviceBtn[deviceCount]->SetLabel(deviceText[deviceCount]);
			deviceBtn[deviceCount]->SetSoundClick(btnClick);
			deviceBtn[deviceCount]->SetIcon(deviceImgs[deviceCount]);
			deviceBtn[deviceCount]->SetImageOver(deviceImgOver[deviceCount]);
			deviceBtn[deviceCount]->SetTrigger(trigA);
			deviceBtn[deviceCount]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
			deviceBtn[deviceCount]->SetPosition(PositionX, PositionY);
			deviceBtn[deviceCount]->Clicked.connect(this, &DeviceMenu::OnButtonClick);
			PositionX += deviceImgs[deviceCount]->GetWidth()+10;

			deviceSelection[deviceCount] = i;

			deviceCount++;
		}
	}

	for(int i = NFS1; i <= NFS10; i++)
	{
		if(DeviceHandler::Instance()->IsInserted(i))
		{
			deviceText[deviceCount] = new GuiText(DeviceName[i], FontSize, (GXColor){0, 0, 0, 255});
			deviceText[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
			deviceText[deviceCount]->SetPosition(0, 2);
			deviceImgs[deviceCount] = new GuiImage(nfs_ImgData);
			deviceImgs[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
			deviceImgOver[deviceCount] = new GuiImage(menu_select);
			deviceImgOver[deviceCount]->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
			deviceBtn[deviceCount] = new GuiButton(deviceImgs[deviceCount]->GetWidth(), deviceImgs[deviceCount]->GetHeight()+FontSize);
			deviceBtn[deviceCount]->SetLabel(deviceText[deviceCount]);
			deviceBtn[deviceCount]->SetSoundClick(btnClick);
			deviceBtn[deviceCount]->SetIcon(deviceImgs[deviceCount]);
			deviceBtn[deviceCount]->SetImageOver(deviceImgOver[deviceCount]);
			deviceBtn[deviceCount]->SetTrigger(trigA);
			deviceBtn[deviceCount]->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
			deviceBtn[deviceCount]->SetPosition(PositionX, PositionY);
			deviceBtn[deviceCount]->Clicked.connect(this, &DeviceMenu::OnButtonClick);
			PositionX += deviceImgs[deviceCount]->GetWidth()+10;

			deviceSelection[deviceCount] = i;

			deviceCount++;
		}
	}

	//! Set image position and tile
	tile = (PositionX-leftImg->GetWidth())/centerImg->GetWidth();

	leftImg->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	leftImg->SetPosition(0, 0);

	centerImg->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	centerImg->SetPosition(leftImg->GetLeft()+leftImg->GetWidth(), 0);
	centerImg->SetTileHorizontal(tile);

	rightImg->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	rightImg->SetPosition(leftImg->GetWidth() + tile * centerImg->GetWidth(), 0);

	width = leftImg->GetWidth() + tile * centerImg->GetWidth() + rightImg->GetWidth();
	height = leftImg->GetHeight();

	BackBtn = new GuiButton(screenwidth, screenheight);
	BackBtn->SetTrigger(trigB);
	BackBtn->Clicked.connect(this, &DeviceMenu::OnButtonClick);

	NoBtn = new GuiButton(screenwidth, screenheight);
	NoBtn->SetTrigger(trigA);
	NoBtn->Clicked.connect(this, &DeviceMenu::OnButtonClick);

	Append(NoBtn);
	Append(BackBtn);
	Append(leftImg);
	Append(centerImg);
	Append(rightImg);
	for(int i = 0; i < deviceCount; i++)
		Append(deviceBtn[i]);

	SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	SetPosition(x, y);
	SetEffect(EFFECT_FADE, 25);

	NoBtn->SetPosition(-GetLeft(), -GetTop());
}

DeviceMenu::~DeviceMenu()
{
	RemoveAll();

	Resources::Remove(device_choose_right_Data);
	Resources::Remove(device_choose_left_Data);
	Resources::Remove(device_choose_center_Data);
	Resources::Remove(sd_ImgData);
	Resources::Remove(usb_ImgData);
	Resources::Remove(usb_blue_ImgData);
	Resources::Remove(smb_ImgData);
	Resources::Remove(ftp_ImgData);
	Resources::Remove(dvd_ImgData);
	Resources::Remove(menu_select);

	Resources::Remove(btnClick);

	for(int i = 0; i < MAXDEVICES; i++)
	{
		if(deviceBtn[i]) {
			delete deviceBtn[i];
			deviceBtn[i] = NULL;
		}
		if(deviceImgs[i]) {
			delete deviceImgs[i];
			deviceImgs[i] = NULL;
		}
		if(deviceImgOver[i]) {
			delete deviceImgOver[i];
			deviceImgOver[i] = NULL;
		}
		if(deviceText[i]) {
			delete deviceText[i];
			deviceText[i] = NULL;
		}
	}

	delete centerImg;
	delete leftImg;
	delete rightImg;

	delete NoBtn;
	delete BackBtn;

	delete trigA;
	delete trigB;
}

void DeviceMenu::OnButtonClick(GuiButton *sender, int pointer UNUSED, const POINT &p)
{
	if(sender == BackBtn)
	{
		DeviceSelected(this, -2);
	}
	else if(sender == NoBtn)
	{
		if(!this->IsInside(p.x, p.y))
			DeviceSelected(this, -2);
		return;
	}

	for(int i = 0; i < deviceCount; i++)
	{
		if(sender == deviceBtn[i])
		{
			DeviceSelected(this, deviceSelection[i]);
			return;
		}
	}
}
