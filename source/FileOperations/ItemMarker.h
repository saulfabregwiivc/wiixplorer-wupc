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
 * ItemMarker.h
 *
 * for WiiXplorer 2010
 ***************************************************************************/
#ifndef _ITEMMARKER_H_
#define _ITEMMARKER_H_

#include <vector>
#include <unistd.h>
#include <gccore.h>
#include <string.h>
#include <sys/dir.h>

typedef struct _ItemStruct
{
	char *  itempath;
	u64	 itemsize;
	bool	isdir;
	int	 itemindex;
} ItemStruct;

class ItemMarker
{
	public:
		ItemMarker() { };
		virtual ~ItemMarker() { Reset(); };
		void AddItem(const ItemStruct * Item);
		int FindItem(const ItemStruct * Item);
		void RemoveItem(const ItemStruct * Item);
		ItemStruct * GetItem(int ind) const;
		const char * GetItemName(int ind) const;
		const char * GetItemPath(int ind) const;
		u64 GetItemSize(int ind);
		bool IsItemDir(int ind);
		int GetItemIndex(int ind);
		int GetItemcount() const { return Items.size(); };
		void Reset();
		const ItemMarker & operator=(const ItemMarker &marker);
	protected:
		std::vector<ItemStruct *> Items;
};

#endif
