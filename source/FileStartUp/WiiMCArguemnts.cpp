/****************************************************************************
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "DeviceControls/DeviceHandler.hpp"
#include "BootHomebrew/BootHomebrew.h"
#include "Settings.h"

void CreateWiiMCArguments(const char * src)
{
	char Text[512];
	AddBootArgument(Settings.WiiMCPath);

	if(strncasecmp(src, "smb", 3) == 0)
	{
		int client = atoi(src+3);
		sprintf(Text, "smb:%s:%s:%s:%s",
			Settings.SMBUser[client-1].User,
			Settings.SMBUser[client-1].Password,
			Settings.SMBUser[client-1].Host,
			Settings.SMBUser[client-1].SMBName);
		AddBootArgument(Text);
	}
	else if(strncasecmp(src, "sd", 2) == 0)
	{
		PartitionHandle * sd = (PartitionHandle *) DeviceHandler::Instance()->GetSDHandle();
		sprintf(Text, "sd:fat:%i", sd->GetLBAStart(0));
		AddBootArgument(Text);
	}
	else if(strncasecmp(src, "dvd", 3) == 0)
	{
		AddBootArgument("dvd");
	}
	else if(strncasecmp(src, "usb", 3) == 0)
	{
		int device = DeviceHandler::PathToDriveType(src) - USB1;
		PartitionHandle * usb = (PartitionHandle *) DeviceHandler::Instance()->GetUSB0Handle();
	
		if(usb && usb->GetFSName(device) && strncasecmp(usb->GetFSName(device), "NTF", 3) == 0)
		{
			sprintf(Text, "usb:ntfs:%i", usb->GetLBAStart(device));
			AddBootArgument(Text);
		}
		else if(usb)
		{
			sprintf(Text, "usb:fat:%i", usb->GetLBAStart(device));
			AddBootArgument(Text);
		}
	}

	const char * filepath = strchr(src, '/');
	if(filepath)
		AddBootArgument(filepath);

	AddBootArgument("WiiXplorer");
}
