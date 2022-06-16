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
 * netreceiver.cpp
 *
 * for WiiXplorer 2010
 ***************************************************************************/
 #include <gctypes.h>

#include "BootHomebrew/BootHomebrew.h"
#include "Language/gettext.h"
#include "Prompts/PromptWindows.h"
#include "Controls/Clipboard.h"
#include "Controls/Taskbar.h"
#include "networkops.h"
#include "netreceiver.h"

void IncommingConnection(NetReceiver & Receiver)
{
	char text[200];
	snprintf(text, sizeof(text),  tr("Do you want to load file from: %s ?"), Receiver.GetIncommingIP());
	int choice = WindowPrompt(tr("Incomming connection."), text, tr("Yes"), tr("No"));
	if(choice)
	{
		const u8 * buffer = Receiver.ReceiveData();
		if(buffer)
		{
			choice = WindowPrompt(tr("Do you want to boot file now?"), Receiver.GetFilename(), tr("Yes"), tr("No"));
			if(choice)
			{
				CopyHomebrewMemory((u8*) buffer, 0, Receiver.GetFilesize());

				ItemStruct * Item = new ItemStruct;
				memset(&Item, 0, sizeof(ItemStruct));

				Item->itempath = strdup("WiiLoad");
				Item->itemsize = Receiver.GetFilesize();
				Item->isdir = false;

				Clipboard::Instance()->AddItem(Item);

				if(Item->itempath)
					free(Item->itempath);
				delete Item;

				BootHomebrew();
			}
		}
	}
	Receiver.FreeData();
	Receiver.CloseConnection();
}
