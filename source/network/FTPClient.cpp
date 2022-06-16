 /***************************************************************************
 * Copyright (C) 2010
 * by dude
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
#include <ogcsys.h>
#include <string.h>
#include "network/networkops.h"
#include "FTPOperations/ftp_devoptab.h"
#include "Settings.h"

static bool ftpReady = false;

bool ConnectFTP(int client)
{
	if(client < 0 || client >= MAXFTPUSERS)
		return false;

	char name[10];
	sprintf(name, "ftp%i", client+1);

	if (strcmp(Settings.FTPUser[client].Host, "") == 0)
		return false;

	char User[50];
	char Password[50];

	if(strcmp(Settings.FTPUser[client].User, "") == 0)
		snprintf(User, sizeof(User), "anonymous");
	else
		snprintf(User, sizeof(User), "%s", Settings.FTPUser[client].User);

	if(strcmp(Settings.FTPUser[client].Password, "") == 0)
		snprintf(Password, sizeof(Password), "anonymous@WiiXplorer.com");
	else
		snprintf(Password, sizeof(Password), "%s", Settings.FTPUser[client].Password);

	bool result = false;

	if (ftpInitDevice(name,
		User,
		Password,
		Settings.FTPUser[client].FTPPath,
		Settings.FTPUser[client].Host,
		Settings.FTPUser[client].Port,
		Settings.FTPUser[client].Passive))
	{
		ftpReady = true;
		result = true;
	}

	return result;
}

/****************************************************************************
 * FTP Stuff
 ***************************************************************************/
bool ConnectFTP()
{
	if(!IsNetworkInit())
		Initialize_Network();

	for (int i = 0; i < MAXFTPUSERS; i++)
		ConnectFTP(i);

	return ftpReady;
}

void CloseFTP()
{
	for (int i = 0; i < MAXFTPUSERS; i++)
	{
		char name[10];
		sprintf(name, "ftp%i", i+1);
		ftpClose(name);
	}
}

void CloseFTP(int client)
{
	if (client < 0 || client >= MAXFTPUSERS || !ftpReady)
		return;

	if(!IsFTPConnected(client))
		return;

	char name[10];
	sprintf(name, "ftp%i", client+1);
	ftpClose(name);
}

bool IsFTPConnected(int ftp)
{
	if (ftp < 0 || ftp >= MAXFTPUSERS || !ftpReady)
		return false;

	char name[10];
	sprintf(name, "ftp%i", ftp+1);

	return CheckFTPConnection(name);
}
