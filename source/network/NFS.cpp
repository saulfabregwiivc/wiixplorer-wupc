 /***************************************************************************
 * Copyright (C) 2010
 * by r-win 
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
#include <stdio.h>
#include <string.h>
#include <ogcsys.h>
#include <unistd.h>
#include <nfs.h>
#include "Tools/StringTools.h"
#include "network/networkops.h"
#include "Settings.h"

static bool NFS_Mounted[MAXNFSUSERS];
static bool firstNFSRun = true;

/****************************************************************************
 * Mount one NFS Mountpoint 
 ****************************************************************************/
bool ConnectNFS(int client)
{
	if(client < 0)
		return false;

	if(firstNFSRun)
	{
		if(!IsNetworkInit())
			Initialize_Network();

		for(int i = 0; i < MAXNFSUSERS; i++)
			NFS_Mounted[i] = false;

		firstNFSRun = false;
	}

	if(NFS_Mounted[client])
		return true;

	bool result = false;

	char mountname[10];

	sprintf(mountname, "nfs%i", client+1);

	if(strcmp(Settings.NFSUser[client].Host, "") != 0)
	{
		if(nfsMount(mountname, Settings.NFSUser[client].Host, Settings.NFSUser[client].Mountpoint))
		{
			result = true;
			NFS_Mounted[client] = true;
		}
	}

	return result;
}

/****************************************************************************
 * Mount NFS Mountpoints 
 ****************************************************************************/
bool ConnectNFS()
{
	bool result = false;

	for(int i = 0; i < MAXNFSUSERS; i++)
	{
		if(ConnectNFS(i))
			result = true;
	}

	return result;
}

/****************************************************************************
 * IsNFS_Mounted
 ***************************************************************************/
bool IsNFS_Mounted(int nfs)
{
	if(nfs < 0 || nfs >= MAXSMBUSERS)
		return false;

	return NFS_Mounted[nfs];
}

/****************************************************************************
 * Close NFS Mountpoint 
 ****************************************************************************/
void CloseNFS(int connection)
{
	if(connection < 0 || connection >= MAXNFSUSERS)
		return;

	char mountname[10];
	sprintf(mountname, "nfs%i", connection+1);

	if(IsNFS_Mounted(connection))
		nfsUnmount(mountname);

	NFS_Mounted[connection] = false;
}

void CloseNFS()
{
	for(int i = 0; i < MAXNFSUSERS; i++)
	{
		CloseNFS(i);
	}
}

/****************************************************************************
 * Reconnect NFS Connection
 ****************************************************************************/
bool NFS_Reconnect()
{
	CloseNFS();
	usleep(200000);
	return ConnectNFS();
}

