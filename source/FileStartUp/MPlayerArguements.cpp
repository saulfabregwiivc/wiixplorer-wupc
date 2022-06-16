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
#include "Tools/StringTools.h"
#include "Settings.h"

void CreateMPlayerArguments(const char * filepath)
{
	if(!filepath)
		return;

	char dst[1024];

	if(strncasecmp(filepath, "smb", 3) == 0)
	{
		int client = atoi(&filepath[3]);
		if(client <= 0 || client >= MAXSMBUSERS)
			return;

		while(filepath[0] != '/' || filepath[1] == '/')
			filepath++;

		filepath++;

		sprintf(dst, "smb://%s:%s@%s/%s/%s",
				Settings.SMBUser[client-1].User,
				Settings.SMBUser[client-1].Password,
				Settings.SMBUser[client-1].Host,
				Settings.SMBUser[client-1].SMBName,
				filepath);
	}
	else
	{
		int i = 0;
		char device[10];

		while(filepath[i] != ':')
		{
			device[i] = filepath[i];
			device[i+1] = 0;
			i++;
		}

		char * ptr = (char *) &filepath[i];

		while(ptr[0] != '/' || ptr[1] == '/')
			ptr++;

		if(strncmp(DeviceHandler::PathToFSName(filepath), "NTFS", 4) != 0)
		{
			sprintf(dst, "ntfs_usb:%s", ptr);
		}
		else if(strncmp(device, "usb", 3) == 0)
		{
			sprintf(dst, "usb:%s", ptr);
		}
		else
		{
			sprintf(dst, "%s:%s", device, ptr);
		}
	}

	AddBootArgument(Settings.MPlayerPath);
	AddBootArgument(dst);
	AddBootArgument(fmt("−quiet"));
}
