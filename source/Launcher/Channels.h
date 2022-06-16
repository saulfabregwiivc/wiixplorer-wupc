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
 * Channel Launcher Class
 *
 * for WiiXplorer 2010
 ***************************************************************************/

#ifndef _CHANNELS_H_
#define _CHANNELS_H_

#include <vector>

#define IMET_MAX_NAME_LEN 0x2a

typedef struct
{
	u64  title;
	char name[IMET_MAX_NAME_LEN+1];
} Channel;

class Channels
{
	public:
		static Channels *Instance();
		static void DestroyInstance();

		void Launch(int index);
		int Count() { return channels.size(); };
		char *GetName(int index) { if(index >= 0 && index < (int) channels.size()) return channels.at(index).name; return NULL; };
		void Sort();

	private:
		Channels();
		~Channels();
		static bool FileSortCallback(const Channel & f1, const Channel & f2);

		static Channels *instance;

		std::vector<Channel> channels;

		u64* GetChannelList(u32* count);
		bool GetAppNameFromTmd(u64 title, char* app);
		bool GetChannelNameFromApp(u64 title, wchar_t* name, int language);
		void Search();
};

#endif
