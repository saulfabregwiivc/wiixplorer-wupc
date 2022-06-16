 /****************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * Created based on information from:
 * http://wiibrew.org/wiki//shared2/sys/net/02/config.dat
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
 * netconfig.cpp
 * for WiiXplorer 2010
 ***************************************************************************/
#include <ogcsys.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "netconfig.h"

#define MAXCONNECTIONS  3

struct _proxy
{
	u8 use_proxy;			   // 0x00 -> no proxy;  0x01 -> proxy
	u8 use_proxy_userandpass;	// 0x00 -> don't use username and password;  0x01 -> use username and password
	u8 padding_1[2];			 // 0x00
	u8 proxy_name[255];
	u8 padding_2;			   // 0x00
	u16 proxy_port;			 // 0-34463 range
	u8 proxy_username[32];
	u8 padding_3;			   // 0x00
	u8 proxy_password[32];
} __attribute__((__packed__));

typedef struct _proxy proxy_t;

typedef struct _connection
{
	/*
	 *  Settings common to both wired and wireless connections
	 */
	u8 flags;		   // Defined below.
	u8 padding_1[3];
	u8 ip[4];			// Wii IP Address
	u8 netmask[4];
	u8 gateway[4];
	u8 dns1[4];
	u8 dns2[4];
	u8 padding_2[2];
	u16 mtu;			//valid values are 0 and 576-1500 range
	u8 padding_3[8];	// 0x00 padding?
	proxy_t proxy_settings;
	u8 padding_4;	   //0x00
	proxy_t proxy_settings_copy;	// Seems to be a duplicate of proxy_settings
	u8 padding_5[1297];			 //0x00
	/*
	 *  Wireless specific settings
	 */
	u8 ssid[32];		// Access Point name.
	u8 padding_6;	   // 0x00
	u8 ssid_length;	 // length of ssid[] (AP name) in bytes.
	u8 padding_7[2];	// 0x00
	u8 padding_8;	   // 0x00
	u8 encryption;	  // (Probably) Encryption.  OPN: 0x00, WEP64: 0x01, WEP128: 0x02 WPA-PSK (TKIP): 0x04, WPA2-PSK (AES): 0x05, WPA-PSK (AES): 0x06
	u8 padding_9[2];	// 0x00
	u8 padding_10;	  // 0x00
	u8 key_length;	  // length of key[] (encryption key) in bytes.  0x00 for WEP64 and WEP128.
	u8 unknown;		 // 0x00 or 0x01 toogled with a WPA-PSK (TKIP) and with a WEP entered with hex instead of ascii.
	u8 padding_11;	  // 0x00
	u8 key[64];		 // Encryption key.  For WEP, key is stored 4 times (20 bytes for WEP64 and 52 bytes for WEP128) then padded with 0x00.
	u8 padding_12[236]; // 0x00
} connection_t;

typedef struct _netconfig
{
	u8 header0;	 // 0x00
	u8 header1;	 // 0x00
	u8 header2;	 // 0x00
	u8 header3;	 // 0x00
	u8 header4;	 // 0x01  When there's at least one valid connection to the Internet.
	u8 header5;	 // 0x00
	u8 header6;	 // 0x07
	u8 header7;	 // 0x00
	connection_t connection[3];
} netconfig_t;

static const char net_conf_file[] ATTRIBUTE_ALIGN(32) = "/shared2/sys/net/02/config.dat";
static u8 net_conf_inited = 0;
static u8 net_configfile[0x1B60] ATTRIBUTE_ALIGN(32);


s32 NetConfInit()
{
	if(net_conf_inited)
		return 0;

	s32 file_fd = IOS_Open(net_conf_file, 1);
	if(file_fd < 0)
		return -1;

	memset(net_configfile, 0, 0x1B60);

	s32 ret = IOS_Read(file_fd, net_configfile, 0x1B5C);

	IOS_Close(file_fd);

	if(ret != 0x1B5C)
		return -1;

	net_conf_inited = 1;

	return 0;
}

u8 ConnectionFlags()
{
	if(NetConfInit() < 0)
		return READINGFAILED;

	netconfig_t * configfile = (netconfig_t *) net_configfile;

	u8 i = 0;

	for(i = 0; i < MAXCONNECTIONS; i++)
	{
		if(configfile->connection[i].flags & CONNECTIONSELECTED)
		{
			return configfile->connection[i].flags;
		}
	}

	return READINGFAILED;
}

u8 HasValidConnection()
{
	if(NetConfInit() < 0)
		return READINGFAILED;

	netconfig_t * configfile = (netconfig_t *) net_configfile;

	return configfile->header4;
}

const char * GetWLanKeys()
{
	if(NetConfInit() < 0)
		return NULL;

	netconfig_t * configfile = (netconfig_t *) net_configfile;

	u8 i = 0;

	for(i = 0; i < MAXCONNECTIONS; i++)
	{
		if(configfile->connection[i].flags & CONNECTIONSELECTED)
		{
			return (const char *) configfile->connection[i].key;
		}
	}

	return NULL;
}
