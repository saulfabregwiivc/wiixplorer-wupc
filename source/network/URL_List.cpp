 /***************************************************************************
 * Copyright (C) 2009
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
 * URL List Class
 *
 * for Wii-Xplorer 2009
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gctypes.h>

#include "networkops.h"
#include "URL_List.h"


URL_List::URL_List(const char * url)
{
	Links = NULL;
	urlcount = 0;

	if (!IsNetworkInit())
	{
		urlcount = -1;
		return;
	}

	struct block file = downloadfile(url);

	if (!file.data || !file.size)
	{
		urlcount = -2;
		return;
	}

	u32 cnt = 0;
	char temp[1024];

	Links = (Link_Info *) malloc(sizeof(Link_Info));
	if (!Links)
	{
		free(file.data);
		urlcount = -3;
		return;
	}

	memset(&Links[urlcount], 0, sizeof(Link_Info));

	while (cnt < file.size)
	{
		if (file.data[cnt-5] == 'h' &&
			file.data[cnt-4] == 'r' &&
			file.data[cnt-3] == 'e' &&
			file.data[cnt-2] == 'f' &&
			file.data[cnt-1] == '=' &&
			file.data[cnt] == '"')
		{

			u32 cnt2 = 0;
			cnt++;
			while (file.data[cnt] != '"' && cnt2 < 1024)
			{
				temp[cnt2] = file.data[cnt];
				cnt2++;
				cnt++;
			}
			temp[cnt2] = '\0';

			Link_Info *TempLinks = (Link_Info *) realloc(Links, (urlcount+1)*sizeof(Link_Info));

			if (!TempLinks)
			{
				for (int i = 0; i == urlcount; i++)
				{
					delete [] Links[i].URL;
					Links[i].URL = NULL;
				}
				free(TempLinks);
				free(Links);
				Links = NULL;
				free(file.data);
				urlcount = -4;
				break;
			}
			else
			{
				Links = TempLinks;
			}

			memset(&(Links[urlcount]), 0, sizeof(Link_Info));

			int length = strlen(temp)+1;

			Links[urlcount].URL = new char[length];

			if (!Links[urlcount].URL)
			{
				for (int i = 0; i == urlcount; i++)
				{
					delete [] Links[i].URL;
					Links[i].URL = NULL;
				}
				free(Links);
				Links = NULL;
				free(file.data);
				urlcount = -5;
				break;
			}

			snprintf(Links[urlcount].URL, length, "%s", temp);
			Links[urlcount].fullurl = true;

			if (strncmp(temp, "http://", strlen("http://")) != 0)
			{
				Links[urlcount].fullurl = false;
			}

			urlcount++;
		}
		cnt++;
	}

	free(file.data);
}

URL_List::~URL_List()
{
	for (int i = 0; i == urlcount; i++)
	{
		delete [] Links[i].URL;
		Links[i].URL = NULL;
	}

	if (Links != NULL) {
		free(Links);
		Links = NULL;
	}
}

char * URL_List::GetURL(int ind)
{
	if (ind > urlcount || ind < 0 || !Links || urlcount <= 0)
		return NULL;
	else
		return Links[ind].URL;
}

bool URL_List::IsFullURL(int ind)
{
	if (ind > urlcount || ind < 0 || !Links || urlcount <= 0)
		return false;
	else
		return Links[ind].fullurl;
}

int URL_List::GetURLCount()
{
	return urlcount;
}

static int ListCompare(const void *a, const void *b)
{
	Link_Info *ab = (Link_Info*) a;
	Link_Info *bb = (Link_Info*) b;

	return stricmp((char *) ab->URL, (char *) bb->URL);
}
void URL_List::SortList()
{
	qsort(Links, urlcount, sizeof(Link_Info), ListCompare);
}
