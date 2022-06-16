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
#include <unistd.h>
#include <gctypes.h>
#include <network.h>
#include "FTPServer.h"
#include "MountVirtualDevices.h"
#include "Tools/gxprintf.h"
#include "ftpii/ftp.h"
#include "ftpii/net.h"
#include "ftpii/virtualpath.h"


FTPServer * FTPServer::instance = NULL;

FTPServer::FTPServer()
	: CThread(30, 32768)
{
	server = -1;
	ftp_running = false;
	ExitRequested = false;
	startThread();
}

FTPServer::~FTPServer()
{
	ShutdownFTP();
	ExitRequested = true;
}

void FTPServer::StartupFTP()
{
	MountVirtualDevices();

	net_close(server);
	server = create_server(Settings.FTPServer.Port);
	if (server < 0)
		return;

	if(strcmp(Settings.FTPServer.Password, "") != 0)
	{
		set_ftp_password(Settings.FTPServer.Password);
	}

	gxprintf(tr("FTP Started.\n"));
	gxprintf("%s %u...\n", tr("Listening on TCP port"), Settings.FTPServer.Port);

	ftp_running = true;
	resumeThread();
}

void FTPServer::ShutdownFTP()
{
	ftp_running = false;
	resumeThread();

	while(!isThreadRunning())
		usleep(10000);

	cleanup_ftp();
	net_close(server);
	UnmountVirtualPaths();

	gxprintf("\x1b[2;0H");
	gxprintf(tr("Server was shutdown...\n"));
	gxprintf("%s %d.\n", tr("Press Startup FTP to start the server on port"), Settings.FTPServer.Port);
}

void FTPServer::executeThread()
{
	bool network_down = false;

	while (!ExitRequested)
	{
		if(!ftp_running)
		{
			suspendThread();
		}
		else
		{
			network_down = process_ftp_events(server);
			if(network_down)
				StartupFTP();
		}

		usleep(1000);
	}
}
