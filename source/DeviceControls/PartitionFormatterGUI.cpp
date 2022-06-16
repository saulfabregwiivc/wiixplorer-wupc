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
#include "Prompts/ProgressWindow.h"
#include "Prompts/ThrobberWindow.h"
#include "Prompts/PromptWindows.h"
#include "TextOperations/wstring.hpp"
#include "Controls/ThreadedTaskHandler.hpp"
#include "PartitionFormatterGUI.hpp"
#include "PartitionFormatter.hpp"
#include "sys.h"

class FormatTask : public ThreadedTask
{
public:
	FormatTask(PartitionHandle *dev, int dev_in, int partition)
		: Device(dev), dev(dev_in), part(partition)
	{}
	virtual ~FormatTask() {}
	virtual void Execute(void);
	sigslot::signal1<int> FormatFinish;
private:
	PartitionHandle * Device;
	int dev;
	int part;
};

PartitionFormatterGui::PartitionFormatterGui()
	: GuiFrame(0,0)
{
	CurPart = 0;
	CurDevice = SD;
	currentState = -1;
	Device = NULL;

	btnClick = Resources::GetSound("button_click.wav");
	btnSoundOver = Resources::GetSound("button_over.wav");

	bgWindow = Resources::GetImageData("bg_properties.png");
	bgWindowImg = new GuiImage(bgWindow);
	width = bgWindow->GetWidth();
	height = bgWindow->GetHeight();
	Append(bgWindowImg);

	btnOutline = Resources::GetImageData("small_button.png");
	CloseImgData = Resources::GetImageData("close.png");
	CloseImgOverData = Resources::GetImageData("close_over.png");

	trigA = new SimpleGuiTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigB = new GuiTrigger();
	trigB->SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);

	TitleTxt = new GuiText(tr("Partition Formatter"), 22, (GXColor){0, 0, 0, 255});
	TitleTxt->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	TitleTxt->SetPosition(0, 25);
	Append(TitleTxt);

	/** Options **/
	DeviceOption.ClickedLeft.connect(this, &PartitionFormatterGui::OnOptionLeftClick);
	DeviceOption.ClickedRight.connect(this, &PartitionFormatterGui::OnOptionRightClick);
	DeviceOption.AddOption(tr("Device"), 150, 70);
	DeviceOption.AddOption(tr("Partition Nr."), 280, 70);
	DeviceOption.SetScale(1.25);
	Append(&DeviceOption);

	int PositionY = 150;
	int PositionX = 50;
	int PositionXVal = 260;

	MountNameTxt = new GuiText(tr("Mounted as:"), 20, (GXColor){0, 0, 0, 255});
	MountNameTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	MountNameTxt->SetPosition(PositionX, PositionY);
	Append(MountNameTxt);

	MountNameValTxt = new GuiText((char *) NULL, 20, (GXColor){0, 0, 0, 255});
	MountNameValTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	MountNameValTxt->SetPosition(PositionXVal, PositionY);
	Append(MountNameValTxt);

	PositionY += 30;

	PartitionTxt = new GuiText(tr("Partition Type:"), 20, (GXColor){0, 0, 0, 255});
	PartitionTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	PartitionTxt->SetPosition(PositionX, PositionY);
	Append(PartitionTxt);

	PartitionValTxt = new GuiText((char *) NULL, 20, (GXColor){0, 0, 0, 255});
	PartitionValTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	PartitionValTxt->SetPosition(PositionXVal, PositionY);
	Append(PartitionValTxt);

	PositionY += 30;

	PartActiveTxt = new GuiText(tr("Active (Bootable):"), 20, (GXColor){0, 0, 0, 255});
	PartActiveTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	PartActiveTxt->SetPosition(PositionX, PositionY);
	Append(PartActiveTxt);

	PartActiveValTxt = new GuiText((char *) NULL, 20, (GXColor){0, 0, 0, 255});
	PartActiveValTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	PartActiveValTxt->SetPosition(PositionXVal, PositionY);
	Append(PartActiveValTxt);

	PositionY += 30;

	PartTypeTxt = new GuiText(tr("File System:"), 20, (GXColor){0, 0, 0, 255});
	PartTypeTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	PartTypeTxt->SetPosition(PositionX, PositionY);
	Append(PartTypeTxt);

	PartTypeValTxt = new GuiText((char *) NULL, 20, (GXColor){0, 0, 0, 255});
	PartTypeValTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	PartTypeValTxt->SetPosition(PositionXVal, PositionY);
	Append(PartTypeValTxt);

	PositionY += 30;

	PartSizeTxt = new GuiText(tr("Partition Size:"), 20, (GXColor){0, 0, 0, 255});
	PartSizeTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	PartSizeTxt->SetPosition(PositionX, PositionY);
	Append(PartSizeTxt);

	PartSizeValTxt = new GuiText((char *) NULL, 20, (GXColor){0, 0, 0, 255});
	PartSizeValTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	PartSizeValTxt->SetPosition(PositionXVal, PositionY);
	Append(PartSizeValTxt);

	ActiveBtnTxt = new GuiText(tr("Set Active"), 16, (GXColor){0, 0, 0, 255});
	ActiveBtnImg = new GuiImage(btnOutline);
	ActiveBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	ActiveBtn->SetLabel(ActiveBtnTxt);
	ActiveBtn->SetImage(ActiveBtnImg);
	ActiveBtn->SetSoundOver(btnSoundOver);
	ActiveBtn->SetSoundClick(btnClick);
	ActiveBtn->SetTrigger(trigA);
	ActiveBtn->SetPosition(80, 320);
	ActiveBtn->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	ActiveBtn->SetEffectGrow();
	ActiveBtn->Clicked.connect(this, &PartitionFormatterGui::OnActiveButtonClick);
	Append(ActiveBtn);

	FormatBtnTxt = new GuiText(tr("Format"), 16, (GXColor){0, 0, 0, 255});
	FormatBtnImg = new GuiImage(btnOutline);
	FormatBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	FormatBtn->SetLabel(FormatBtnTxt);
	FormatBtn->SetImage(FormatBtnImg);
	FormatBtn->SetSoundOver(btnSoundOver);
	FormatBtn->SetSoundClick(btnClick);
	FormatBtn->SetTrigger(trigA);
	FormatBtn->SetPosition(-80, 320);
	FormatBtn->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	FormatBtn->SetEffectGrow();
	FormatBtn->Clicked.connect(this, &PartitionFormatterGui::OnFormatButtonClick);
	Append(FormatBtn);

	BackBtnImg = new GuiImage(CloseImgData);
	BackBtnImgOver = new GuiImage(CloseImgOverData);
	BackBtn = new GuiButton(btnOutline->GetWidth(), btnOutline->GetHeight());
	BackBtn->SetImage(BackBtnImg);
	BackBtn->SetImageOver(BackBtnImgOver);
	BackBtn->SetSoundOver(btnSoundOver);
	BackBtn->SetSoundClick(btnClick);
	BackBtn->SetTrigger(trigA);
	BackBtn->SetTrigger(trigB);
	BackBtn->SetPosition(370, 20);
	BackBtn->SetEffectGrow();
	BackBtn->Clicked.connect(this, &PartitionFormatterGui::OnBackButtonClick);
	Append(BackBtn);

	SetDevice();
	ListPartitions();

	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);
}

PartitionFormatterGui::~PartitionFormatterGui()
{
	RemoveAll();

	delete ActiveBtn;
	delete BackBtn;
	delete FormatBtn;

	delete TitleTxt;
	delete ActiveBtnTxt;
	delete FormatBtnTxt;
	delete MountNameTxt;
	delete PartitionTxt;
	delete PartActiveTxt;
	delete PartTypeTxt;
	delete PartSizeTxt;
	delete MountNameValTxt;
	delete PartitionValTxt;
	delete PartActiveValTxt;
	delete PartTypeValTxt;
	delete PartSizeValTxt;

	delete bgWindowImg;
	delete ActiveBtnImg;
	delete FormatBtnImg;
	delete BackBtnImg;
	delete BackBtnImgOver;

	Resources::Remove(btnClick);
	Resources::Remove(btnSoundOver);

	Resources::Remove(bgWindow);
	Resources::Remove(btnOutline);
	Resources::Remove(CloseImgData);
	Resources::Remove(CloseImgOverData);

	delete trigA;
	delete trigB;
}

void PartitionFormatterGui::ListPartitions()
{
	if(!Device)
		return;

	if(CurPart < 0 || CurPart >= Device->GetPartitionCount())
		return;

	char MountName[10];
	if(Device->MountName(CurPart))
		sprintf(MountName, "%s:/", Device->MountName(CurPart));

	MountNameValTxt->SetText(Device->MountName(CurPart) ? MountName : tr("Not mounted"));
	PartitionValTxt->SetText(Device->GetEBRSector(CurPart) > 0 ? tr("Logical") : Device->GetPartitionType(CurPart) == PARTITION_TYPE_GPT ? tr("GUID") : tr("Primary"));
	PartActiveValTxt->SetText(Device->IsActive(CurPart) ? tr("Yes"): tr("No"));
	PartTypeValTxt->SetText(fmt("%s (0x%02X)", Device->GetFSName(CurPart), Device->GetPartitionType(CurPart)));
	PartSizeValTxt->SetTextf("%0.2fGB", Device->GetSize(CurPart)/GBSIZE);
}

void PartitionFormatterGui::SetDevice()
{
	if(CurDevice == SD)
	{
		DeviceOption.SetOptionValue(0, tr("SD-Card"));
		Device = DeviceHandler::Instance()->GetSDHandle();
	}
	else if(CurDevice == GCSDA)
	{
		DeviceOption.SetOptionValue(0, tr("GC-SD A"));
		Device = DeviceHandler::Instance()->GetGCAHandle();
	}
	else if(CurDevice == GCSDB)
	{
		DeviceOption.SetOptionValue(0, tr("GC-SD B"));
		Device = DeviceHandler::Instance()->GetGCBHandle();
	}
	else if(CurDevice == USB1)
	{
		DeviceOption.SetOptionValue(0, tr("USB Device Port 0"));
		Device = DeviceHandler::Instance()->GetUSB0Handle();
	}
	else if(CurDevice == USB2)
	{
		DeviceOption.SetOptionValue(0, tr("USB Device Port 1"));
		Device = DeviceHandler::Instance()->GetUSB1Handle();
	}

	DeviceOption.SetOptionValue(1, fmt("%i", CurPart+1));
}

void PartitionFormatterGui::OnActiveButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	if(!Device)
		return;

	if(CurPart < 0 || CurPart >= Device->GetPartitionCount())
		return;

	if(Device->GetEBRSector(CurPart) > 0)
	{
		ShowError(tr("You can't set a logical partition as active."));
		return;
	}
	else if(Device->GetPartitionType(CurPart) == 0x05 || Device->GetPartitionType(CurPart) == 0x0F ||
			Device->GetPartitionType(CurPart) == 0x00)
	{
		ShowError(tr("You can't set this partition as active."));
		return;
	}

	int ret = WindowPrompt(tr("Set this Partition as Active?"), fmt("%s %i: %s (%0.2fGB)", tr("Partition"), CurPart, Device->GetFSName(CurPart), Device->GetSize(CurPart)/GBSIZE), tr("Yes"), tr("Cancel"));
	if(ret == 0)
		return;

	PartitionFormatter::SetActive(Device->GetDiscInterface(), CurPart);
	Device->GetPartitionRecord(CurPart)->Bootable = true;

	SetDevice();
	ListPartitions();
}

void PartitionFormatterGui::OnBackButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	//! set slide out effect
	SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	Application::Instance()->PushForDelete(this);
}

void PartitionFormatterGui::OnOptionLeftClick(GuiElement *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	if(sender == DeviceOption.GetButtonLeft(0))
	{
		if(CurDevice == SD)
		{
			if(DeviceHandler::Instance()->GCB_Inserted())
				CurDevice = GCSDB;
			else if(DeviceHandler::Instance()->GCA_Inserted())
				CurDevice = GCSDA;
			else if(DeviceHandler::Instance()->USB1_Inserted())
				CurDevice = USB2;
			else if(DeviceHandler::Instance()->USB0_Inserted())
				CurDevice = USB1;
		}
		else if(CurDevice == GCSDB)
		{
			if(DeviceHandler::Instance()->GCA_Inserted())
				CurDevice = GCSDA;
			else if(DeviceHandler::Instance()->USB1_Inserted())
				CurDevice = USB2;
			else if(DeviceHandler::Instance()->USB0_Inserted())
				CurDevice = USB1;
			else if(DeviceHandler::Instance()->SD_Inserted())
				CurDevice = SD;
		}
		else if(CurDevice == GCSDA)
		{
			if(DeviceHandler::Instance()->USB0_Inserted())
				CurDevice = USB1;
			else if(DeviceHandler::Instance()->USB1_Inserted())
				CurDevice = USB2;
			else if(DeviceHandler::Instance()->SD_Inserted())
				CurDevice = SD;
			else if(DeviceHandler::Instance()->GCB_Inserted())
				CurDevice = GCSDB;
		}
		else if(CurDevice == USB2)
		{
			if(DeviceHandler::Instance()->USB0_Inserted())
				CurDevice = USB1;
			else if(DeviceHandler::Instance()->SD_Inserted())
				CurDevice = SD;
			else if(DeviceHandler::Instance()->GCB_Inserted())
				CurDevice = GCSDB;
			else if(DeviceHandler::Instance()->GCA_Inserted())
				CurDevice = GCSDA;
		}
		else if(CurDevice == USB1)
		{
			if(DeviceHandler::Instance()->SD_Inserted())
				CurDevice = SD;
			else if(DeviceHandler::Instance()->GCB_Inserted())
				CurDevice = GCSDB;
			else if(DeviceHandler::Instance()->GCA_Inserted())
				CurDevice = GCSDA;
			else if(DeviceHandler::Instance()->USB1_Inserted())
				CurDevice = USB2;
		}
	}
	else if(sender == DeviceOption.GetButtonLeft(1))
	{
		if(CurPart > 0)
			--CurPart;
	}

	SetDevice();
	ListPartitions();
}

void PartitionFormatterGui::OnOptionRightClick(GuiElement *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	if(sender == DeviceOption.GetButtonRight(0))
	{
		if(CurDevice == SD)
		{
			if(DeviceHandler::Instance()->USB0_Inserted())
				CurDevice = USB1;
			else if(DeviceHandler::Instance()->USB1_Inserted())
				CurDevice = USB2;
			else if(DeviceHandler::Instance()->GCA_Inserted())
				CurDevice = GCSDA;
			else if(DeviceHandler::Instance()->GCB_Inserted())
				CurDevice = GCSDB;
		}
		else if(CurDevice == USB1)
		{
			if(DeviceHandler::Instance()->USB1_Inserted())
				CurDevice = USB2;
			else if(DeviceHandler::Instance()->GCA_Inserted())
				CurDevice = GCSDA;
			else if(DeviceHandler::Instance()->GCB_Inserted())
				CurDevice = GCSDB;
			else if(DeviceHandler::Instance()->SD_Inserted())
				CurDevice = SD;
		}
		else if(CurDevice == USB2)
		{
			if(DeviceHandler::Instance()->GCA_Inserted())
				CurDevice = GCSDA;
			else if(DeviceHandler::Instance()->GCB_Inserted())
				CurDevice = GCSDB;
			else if(DeviceHandler::Instance()->SD_Inserted())
				CurDevice = SD;
			else if(DeviceHandler::Instance()->USB0_Inserted())
				CurDevice = USB1;
		}
		else if(CurDevice == GCSDA)
		{
			if(DeviceHandler::Instance()->GCB_Inserted())
				CurDevice = GCSDB;
			else if(DeviceHandler::Instance()->SD_Inserted())
				CurDevice = SD;
			else if(DeviceHandler::Instance()->USB0_Inserted())
				CurDevice = USB1;
			else if(DeviceHandler::Instance()->USB1_Inserted())
				CurDevice = USB2;
		}
		else if(CurDevice == GCSDB)
		{
			if(DeviceHandler::Instance()->SD_Inserted())
				CurDevice = SD;
			else if(DeviceHandler::Instance()->USB0_Inserted())
				CurDevice = USB1;
			else if(DeviceHandler::Instance()->USB1_Inserted())
				CurDevice = USB2;
			else if(DeviceHandler::Instance()->GCA_Inserted())
				CurDevice = GCSDA;
		}
	}
	else if(sender == DeviceOption.GetButtonRight(1))
	{
		if(Device && CurPart+1 < Device->GetPartitionCount())
			++CurPart;
	}

	SetDevice();
	ListPartitions();
}

void PartitionFormatterGui::OnFormatButtonClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	if(!Device)
		return;

	if(CurPart < 0 || CurPart >= Device->GetPartitionCount())
		return;

	if(Device->GetPartitionType(CurPart) == PARTITION_TYPE_GPT)
	{
		ShowError(tr("You have a GUID Partition Table (GPT). Formatting partitions in a GPT is not supported yet."));
		return;
	}

	int ret = WindowPrompt(tr("Format Partition?"), fmt("%s %i: %s (%0.2fGB)", tr("Partition"), CurPart, Device->GetFSName(CurPart), Device->GetSize(CurPart)/GBSIZE), tr("Yes"), tr("Cancel"));
	if(ret == 0)
		return;

	ret = WindowPrompt(tr("WARNING"), tr("This will delete all the data on this partition. Do you want to continue?"), tr("Yes"), tr("Cancel"));
	if(ret == 0)
		return;

	formatResult = 0xDEADBEAF;

	FormatTask task(Device, CurDevice, CurPart);
	task.FormatFinish.connect(this, &PartitionFormatterGui::OnFormatFinish);

	ThreadedTaskHandler::Instance()->AddTask(&task);

	while(formatResult == (int) 0xDEADBEAF)
		Application::Instance()->updateEvents();

	if(formatResult >= 0)
		WindowPrompt(tr("Successfully formated the partition."), 0, tr("OK"));
	else
		WindowPrompt(tr("Failed formatting the partition."), 0, tr("OK"));

	SetDevice();
	ListPartitions();
}

void PartitionFormatterGui::OnFormatFinish(int result)
{
	formatResult = result;
}

void FormatTask::Execute(void)
{
	ThrobberWindow *window = new ThrobberWindow(tr("Formatting partition!"), tr("Please wait..."));
	window->DimBackground(true);
	Application::Instance()->SetUpdateOnly(window);
	Application::Instance()->Append(window);

	const DISC_INTERFACE *interface = Device->GetDiscInterface();
	int ebrLBA = Device->GetEBRSector(part);
	u32 lbaStart = Device->GetLBAStart(part);
	u32 secCount = Device->GetSecCount(part);

	DeviceHandler::Instance()->UnMount(dev);

	int ret = PartitionFormatter::FormatToFAT32(interface, lbaStart, secCount);
	if(ret >= 0)
	{
		if(ebrLBA > 0)
			ret = PartitionFormatter::WriteEBR_FAT32(interface, ebrLBA, lbaStart);
		else
			ret = PartitionFormatter::WriteMBR_FAT32(interface, lbaStart);
	}

	DeviceHandler::Instance()->Mount(dev);

	window->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
	Application::Instance()->PushForDelete(window);

	FormatFinish(ret);
}
