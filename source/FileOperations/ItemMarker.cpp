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
 * ItemMarker Class
 *
 * for WiiXplorer 2010
 ***************************************************************************/
#include <malloc.h>
#include "ItemMarker.h"

void ItemMarker::AddItem(const ItemStruct * file)
{
	if(!file || !file->itempath)
		return;

	else if(strlen(file->itempath) == 0)
		return;

	else if(FindItem(file) >= 0)
		return;

	char * filename = strrchr(file->itempath, '/');

	if(filename)
	{
		filename++;
		if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
			return;
	}

	ItemStruct * newItem = new ItemStruct;
	newItem->itempath = (char *) malloc(strlen(file->itempath)+1);
	newItem->itemsize = file->itemsize;
	newItem->isdir = file->isdir;
	newItem->itemindex = file->itemindex;

	//! Skip double '/'
	int j, n = 0;
	char * srcpath = file->itempath;
	char * dstpath = newItem->itempath;

	for(j = 0; srcpath[j] != 0; ++j)
	{
		if(srcpath[j] == '/' && srcpath[j+1] == '/')
			continue;

		dstpath[n] = srcpath[j];
		++n;
	}
	dstpath[n] = 0;

	Items.push_back(newItem);
}

ItemStruct * ItemMarker::GetItem(int ind) const
{
	if(ind < 0 || ind >= (int) Items.size())
		return NULL;

	return Items.at(ind);
}

const char * ItemMarker::GetItemName(int ind) const
{
	if(ind < 0 || ind >= (int) Items.size())
		return NULL;

	const char * itempath = Items.at(ind)->itempath;

	if(itempath[strlen(itempath)-1] == '/' && strlen(itempath) > 2)
	{
		const char * ptr = &itempath[strlen(itempath)-2];
		while(ptr > itempath && *ptr != '/')
			--ptr;

		if(ptr == itempath)
			return NULL;

		return ptr+1;
	}

	const char * filename = strrchr(itempath, '/');

	if(!filename)
		return NULL;

	return filename+1;
}

const char * ItemMarker::GetItemPath(int ind) const
{
	if(ind < 0 || ind >= (int) Items.size())
		return NULL;

	return (const char *) Items.at(ind)->itempath;
}

u64 ItemMarker::GetItemSize(int ind)
{
	if(ind < 0 || ind >= (int) Items.size())
		return 0;

	return Items.at(ind)->itemsize;
}

bool ItemMarker::IsItemDir(int ind)
{
	if(ind < 0 || ind >= (int) Items.size())
		return false;

	return Items.at(ind)->isdir;
}

int ItemMarker::GetItemIndex(int ind)
{
	if(ind < 0 || ind >= (int) Items.size())
		return -1;

	return Items.at(ind)->itemindex;
}

int ItemMarker::FindItem(const ItemStruct * Item)
{
	if(!Item)
		return -1;

	for(u32 i = 0; i < Items.size(); i++)
	{
		if(strcasecmp(Item->itempath, Items.at(i)->itempath) == 0)
		{
			return i;
		}
	}

	return -1;
}

void ItemMarker::RemoveItem(const ItemStruct * Item)
{
	if(!Item)
		return;

	int num = FindItem(Item);

	if(num < 0)
		return;

	free(Items.at(num)->itempath);
	delete Items.at(num);

	Items.erase(Items.begin()+num);
}

void ItemMarker::Reset()
{
	for(u32 i = 0; i < Items.size(); i++)
	{
		free(Items.at(i)->itempath);
		delete Items.at(i);
	}

	Items.clear();
}

const ItemMarker & ItemMarker::operator=(const ItemMarker &marker)
{
	this->Reset();

	for(int i = 0; i < marker.GetItemcount(); i++)
		this->AddItem(marker.GetItem(i));

	return *this;
}
