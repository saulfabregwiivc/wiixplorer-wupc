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
#ifndef _NETRECEIVER_H_
#define _NETRECEIVER_H_

class NetReceiver
{
	public:
		NetReceiver();
		~NetReceiver();
		bool CheckIncomming();
		void CloseConnection();
		const u8 * ReceiveData();
		const u8 * GetData();
		u32 GetFilesize();
		const char * GetFilename() { return (const char *) FileName; };
		const char * GetIncommingIP() { return (const char *) incommingIP; };
		void FreeData();
	protected:
		const u8 * UncompressData();

		int connection;
		int socket;
		u8 * filebuffer;
		u32 filesize;
		u32 uncfilesize;
		char incommingIP[20];
		char FileName[50];
		char wiiloadVersion[2];
};


void IncommingConnection(NetReceiver & Receiver);

#endif
