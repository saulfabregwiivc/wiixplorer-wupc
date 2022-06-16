/***************************************************************************
 * Copyright (C) 2010
 * by Dimok, dude
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
 * Channel Launcher Class
 *
 * for WiiXplorer 2010
 ***************************************************************************/

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogc/conf.h>
#include <ogc/isfs.h>
#include <ogc/wiilaunch.h>
#include <algorithm>

#include "certs.h"
#include "Channels.h"
#include "FileOperations/MD5.h"
#include "TextOperations/wstring.hpp"
#include "Tools/tools.h"
#include "sys.h"

#define IMET_OFFSET			0x40
#define IMET_SIGNATURE		0x494d4554
#define DOWNLOADED_CHANNELS	0x00010001
#define SYSTEM_CHANNELS		0x00010002
#define RF_NEWS_CHANNEL		0x48414741
#define RF_FORECAST_CHANNEL	0x48414641

typedef struct
{
	u8  zeroes1[0x40];
	u32 sig;	// "IMET"
	u32 unk1;
	u32 unk2;
	u32 filesizes[3];
	u32 unk3;
	u16 name_japanese[IMET_MAX_NAME_LEN];
	u16 name_english[IMET_MAX_NAME_LEN];
	u16 name_german[IMET_MAX_NAME_LEN];
	u16 name_french[IMET_MAX_NAME_LEN];
	u16 name_spanish[IMET_MAX_NAME_LEN];
	u16 name_italian[IMET_MAX_NAME_LEN];
	u16 name_dutch[IMET_MAX_NAME_LEN];
	u16 name_simp_chinese[IMET_MAX_NAME_LEN];
	u16 name_trad_chinese[IMET_MAX_NAME_LEN];
	u16 name_korean[IMET_MAX_NAME_LEN];
	u8  zeroes2[0x24c];
	u8  md5[0x10];
} IMET;

Channels *Channels::instance = NULL;

Channels::Channels()
{
	Search();

	if(channels.size() > 1)
		Sort();
}

Channels::~Channels()
{
}

Channels * Channels::Instance()
{
	if (instance == NULL)
	{
		instance = new Channels();
	}
	return instance;
}

void Channels::DestroyInstance()
{
	if (instance)
	{
		delete instance;
	}
	instance = NULL;
}

u64* Channels::GetChannelList(u32* count)
{
	u32 countall;
	u32 ret = ES_GetNumTitles(&countall);

	if (ret || !countall)
		return NULL;

	u64* titles = (u64*)memalign(32, ALIGN32(countall * sizeof(u64)));
	if (!titles)
		return NULL;

	u64* channels = (u64*)malloc(countall * sizeof(u64));
	if (!channels)
	{
		free(titles);
		return NULL;
	}

	ret = ES_GetTitles(titles, countall);

	*count = 0;
	for (u32 i = 0; i < countall; i++)
	{
		u32 type = titles[i] >> 32;

		if (type == DOWNLOADED_CHANNELS || type == SYSTEM_CHANNELS)
		{
			if ((titles[i] & 0xFFFFFFFF) == RF_NEWS_CHANNEL ||	// skip region free news and forecast channel
				(titles[i] & 0xFFFFFFFF) == RF_FORECAST_CHANNEL)
				continue;

			channels[(*count)++] = titles[i];
		}
	}

	free(titles);

	return (u64*)realloc(channels, *count * sizeof(u64));
}

bool Channels::GetAppNameFromTmd(u64 title, char* app)
{
	char tmd[ISFS_MAXPATH];
	static fstats stats ATTRIBUTE_ALIGN(32);

	u32 high = (u32)(title >> 32);
	u32 low  = (u32)(title & 0xFFFFFFFF);

	bool ret = false;

	sprintf(tmd, "/title/%08x/%08x/content/title.tmd", high, low);

	s32 fd = ISFS_Open(tmd, ISFS_OPEN_READ);
	if (fd >= 0)
	{
		if (ISFS_GetFileStats(fd, &stats) >= 0)
		{
			u32* data = NULL;

			if (stats.file_length > 0)
				data = (u32*)memalign(32, ALIGN32(stats.file_length));

			if (data)
			{
				if (ISFS_Read(fd, (char*)data, stats.file_length) > 0x208)
				{
					u16 i;
					struct _tmd * tmd_file = (struct _tmd *) SIGNATURE_PAYLOAD(data);
					for(i = 0; i < tmd_file->num_contents; ++i)
						if(tmd_file->contents[i].index == 0)
							break;

					sprintf(app, "/title/%08x/%08x/content/%08x.app", high, low, tmd_file->contents[i].cid);
					ret = true;
				}
				free(data);
			}
		}
		ISFS_Close(fd);
	}

	return ret;
}

bool Channels::GetChannelNameFromApp(u64 title, wchar_t* name, int language)
{
	char app[ISFS_MAXPATH];
	static IMET imet ATTRIBUTE_ALIGN(32);
	static fstats stats ATTRIBUTE_ALIGN(32);

	bool ret = false;

	if (!GetAppNameFromTmd(title, app))
		return false;

	if (language > CONF_LANG_KOREAN)
		language = CONF_LANG_ENGLISH;

	s32 fd = ISFS_Open(app, ISFS_OPEN_READ);

	if (fd >= 0)
	{
		if (ISFS_GetFileStats(fd, &stats) < 0)
		{
			ISFS_Close(fd);
			return false;
		}

		if (stats.file_length < sizeof(IMET)+IMET_OFFSET)
		{
			ISFS_Close(fd);
			return false;
		}

		if (ISFS_Seek(fd, IMET_OFFSET, SEEK_SET) != IMET_OFFSET)
		{
			ISFS_Close(fd);
			return false;
		}

		if (ISFS_Read(fd, (void*)(&imet), sizeof(IMET)) == sizeof(IMET))
		{
			if (imet.sig == IMET_SIGNATURE)
			{
				unsigned char md5[16];
				unsigned char imetmd5[16];

				memcpy(imetmd5, imet.md5, 16);
				memset(imet.md5, 0, 16);

				MD5(md5, (unsigned char*)(&imet), sizeof(IMET));
				if (memcmp(imetmd5, md5, 16) == 0)
				{
 					//now we can be pretty sure that we have a valid imet :)
					if (imet.name_japanese[language*IMET_MAX_NAME_LEN] == 0)
					{
						// channel name is not available in system language
						if (imet.name_english[0] != 0)
						{
							language = CONF_LANG_ENGLISH;
						}
						else
						{
							// channel name is also not available on english, get ascii name
							language = -1;
							for (int i = 0; i < 4; i++)
							{
								name[i] = ((title&0xFFFFFFFF) >> (24-i*8)) & 0xFF;
							}
							name[4] = 0;
						}
					}

					if (language >= 0)
					{
						// retrieve channel name in system language or on english
						for (int i = 0; i < IMET_MAX_NAME_LEN; i++)
						{
							name[i] = imet.name_japanese[i+(language*IMET_MAX_NAME_LEN)];
						}
					}
					ret = true;
				}
			}
		}
		ISFS_Close(fd);
	}

	return ret;
}

void Channels::Launch(int index)
{
	if((u32)index >= channels.size())
		return;

	//! save the title id of the channel we are about to launch
	u64 title = channels[index].title;

	//! clean up application
	ExitApp();

	//! Launch title
	WII_Initialize();
	WII_LaunchTitle(title);
}

void Channels::Search()
{
	u32 count;
	u64* list = GetChannelList(&count);

	if (!list)
		return;

	wchar_t name[IMET_MAX_NAME_LEN];

	ES_Identify((u32*)Certificates, sizeof(Certificates), (u32*)Tmd, sizeof(Tmd), (u32*)Ticket, sizeof(Ticket), 0);

	int language = CONF_GetLanguage();

	for (u32 i = 0; i < count; i++)
	{
		if (GetChannelNameFromApp(list[i], name, language))
		{
			Channel channel;
			wString *wsname = new wString(name);

			snprintf(channel.name, sizeof(channel.name), "%s", (wsname->toUTF8()).c_str());
			delete wsname;

			channel.title = list[i];

			channels.push_back(channel);
		}
	}

	free(list);

	std::vector<Channel>(channels).swap(channels);
}

void Channels::Sort()
{
	std::sort(channels.begin(), channels.end(), FileSortCallback);
}

bool Channels::FileSortCallback(const Channel & f1, const Channel & f2)
{
	if(stricmp(f1.name, f2.name) > 0)
		return false;
	else
		return true;
}
