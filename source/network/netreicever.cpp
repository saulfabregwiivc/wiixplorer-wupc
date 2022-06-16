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
#include <stdio.h>
#include <string.h>
#include <ogcsys.h>
#include <zlib.h>
#include <fcntl.h>
#include <ogc/machine/processor.h>
#include "ArchiveOperations/Archive.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/ProgressWindow.h"

#include "http.h"
#include "networkops.h"
#include "FileOperations/fileops.h"
#include "FileOperations/DirList.h"
#include "netreceiver.h"

#define PORT	4299

NetReceiver::NetReceiver()
{
	connection = -1;
	socket = -1;
	filebuffer = NULL;
	filesize = 0;
	uncfilesize = 0;
	memset(FileName, 0, sizeof(FileName));
}

NetReceiver::~NetReceiver()
{
	FreeData();
	CloseConnection();
}

void NetReceiver::CloseConnection()
{
	if(connection >= 0)
		net_close(connection);
	if(socket >= 0)
		net_close(socket);
	connection = -1;
	socket = -1;
}

const u8 * NetReceiver::GetData()
{
	return filebuffer;
}

u32 NetReceiver::GetFilesize()
{
	if(uncfilesize > 0)
		return uncfilesize;

	return filesize;
}

void NetReceiver::FreeData()
{
	if(filebuffer)
		free(filebuffer);
	filebuffer = NULL;
	filesize = 0;
	uncfilesize = 0;
	memset(FileName, 0, sizeof(FileName));
}

bool NetReceiver::CheckIncomming()
{
	struct sockaddr_in sin;
	struct sockaddr_in client_address;
	socklen_t addrlen = sizeof(client_address);

	//Open socket
	socket = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	if (socket == INVALID_SOCKET)
	{
		return false;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	int flags = net_fcntl(socket, F_GETFL, 0);
	flags = net_fcntl(socket, F_SETFL, flags | 4);

	if (net_bind(socket, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
		net_close(socket);
		return false;
	}

	if (net_listen(socket, 10) < 0) {
		net_close(socket);
		return false;
	}

	Timer CTimer;

	do
	{
		connection = net_accept(socket, (struct sockaddr*)&client_address, &addrlen);
		if(connection >= 0)
			break;
	}
	while(CTimer.elapsed() < 1.0f);

	sprintf(incommingIP, "%s", inet_ntoa(client_address.sin_addr));

	if (connection < 0)
	{
		net_close(socket);
		return false;
	}
	else
	{
		unsigned char haxx[9];
		//skip haxx
		net_read(connection, &haxx, 8);
		if (memcmp(haxx, "HAXX", 4) != 0) // unsupported protocol
		{
			net_close(connection);
			net_close(socket);
			return false;
		}
		wiiloadVersion[0] = haxx[4];
		wiiloadVersion[1] = haxx[5];

		net_read(connection, &filesize, 4);

		if (haxx[4] > 0 || haxx[5] > 4)
			net_read(connection, &uncfilesize, 4); // Compressed protocol, read another 4 bytes
	}

	return true;
}

const u8 * NetReceiver::ReceiveData()
{
	if(connection < 0)
		return NULL;

	if(filebuffer)
		free(filebuffer);
	filebuffer = NULL;

	filebuffer = (u8 *) malloc(filesize);
	if(!filebuffer)
	{
		ShowError(tr("Not enough memory."));
		return NULL;
	}

	StartProgress(tr("Receiving file..."));

	u32 done = 0;
	u32 blocksize = 5*1024;
	char tmptxt[200];
	snprintf(tmptxt, sizeof(tmptxt), "Incomming from: %s", incommingIP);

	int retries = 5;

	do
	{
		if(ProgressWindow::Instance()->IsCanceled())
		{
			FreeData();
			StopProgress();
			ShowError(tr("Transfer cancelled."));
			return NULL;
		}

		if (blocksize > filesize - done)
			blocksize = filesize - done;

		ShowProgress(done, filesize, tmptxt);

		int result = network_read(connection, filebuffer+done, blocksize);
		if(result < 0)
		{
			--retries;
			if(retries == 0)
			{
				FreeData();
				StopProgress();
				ShowError(tr("Transfer failed."));
				return NULL;
			}
		}
		if(!result)
		{
			--retries;
			if(!retries)
				break;
		}

		done += result;

	} while(done < filesize);

	// finish up the progress
	FinishProgress(filesize);

	StopProgress();

	if(done != filesize)
	{
		FreeData();
		ShowError(tr("Filesize doesn't match."));
	}

	char temp[50];
	network_read(connection, (u8 *) temp, 49);
	temp[49] = 0;

	snprintf(FileName, sizeof(FileName), "%s", temp);

	if(UncompressData() == NULL)
	{
		FreeData();
		ShowError(tr("Could not decompress the file."));
	}

	return filebuffer;
}

const u8 * NetReceiver::UncompressData()
{
	if(!filebuffer)
		return NULL;

	//Zip File
	if (filebuffer[0] == 'P' && filebuffer[1] == 'K' && filebuffer[2] == 0x03 && filebuffer[3] == 0x04)
	{
		char temppath[200];
		char tempfilepath[200];
		snprintf(temppath, sizeof(temppath), "%s/WiiXplorerTmp/", Settings.BootDevice);
		snprintf(tempfilepath, sizeof(tempfilepath), "%s/WiiXplorerTmp/tmp.zip", Settings.BootDevice);

		if(!CreateSubfolder(temppath))
		{
			FreeData();
			return NULL;
		}

		FILE * file = fopen(tempfilepath, "wb");
		if(!file)
		{
			FreeData();
			RemoveDirectory(temppath);
			return NULL;
		}

		fwrite(filebuffer, 1, filesize, file);
		fclose(file);

		FreeData();

		ArchiveHandle * Zip = new ArchiveHandle(tempfilepath);
		if(!Zip->ExtractAll(temppath))
		{
			delete Zip;
			RemoveDirectory(temppath);
			return NULL;
		}

		delete Zip;

		DirList Dir(temppath, ".dol,.elf");
		if(Dir.GetFilecount() <= 0)
		{
			RemoveDirectory(temppath);
			ShowError(tr("No homebrew in the zip."));
			return NULL;
		}

		char newfilepath[300];
		snprintf(newfilepath, sizeof(newfilepath), "%s", Dir.GetFilepath(0));
		snprintf(FileName, sizeof(FileName), "%s", Dir.GetFilename(0));

		u8 * buffer = NULL;
		u32 newfilesize = 0;

		if(LoadFileToMem(newfilepath, &buffer, &newfilesize) < 0)
		{
			RemoveDirectory(temppath);
			return NULL;
		}

		RemoveDirectory(temppath);
		filesize = newfilesize;
		filebuffer = buffer;
	}

	//WiiLoad zlib compression
	else if((wiiloadVersion[0] > 0 || wiiloadVersion[1] > 4) && uncfilesize != 0)
	{
		u8 * unc = (u8 *) malloc(uncfilesize);
		if(!unc)
		{
			FreeData();
			return NULL;
		}

		uLongf f = uncfilesize;
		if(uncompress(unc, &f, filebuffer, filesize) != Z_OK)
		{
			free(unc);
			FreeData();
			return NULL;
		}

		free(filebuffer);

		filebuffer = unc;
		filesize = f;
	}

	return filebuffer;
}
