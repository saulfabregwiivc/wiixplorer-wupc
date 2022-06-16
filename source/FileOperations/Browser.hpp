/****************************************************************************
 * Copyright (C) 2009
 * by Dimok
 *
 * Original Filebrowser by Tantric for libwiigui
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
 * Browser Class
 *
 * Virtual class to inherit the browsers off
 * introducing the minimum of functions need to browse
 *
 * for WiiXplorer 2009
 ***************************************************************************/
#ifndef _BROWSER_H_
#define _BROWSER_H_

#include "GUI/gui_trigger.h"
#include "ItemMarker.h"

class Browser
{
	public:
		Browser() { DelayCounter = 0; };
		virtual ~Browser() { };
		virtual int GetEntrieCount() { return 0; };
		virtual int GetPageIndex() { return 0; };
		virtual int GetSelIndex() { return 0; };
		virtual void SetPageIndex(int ind UNUSED) { };
		virtual void SetSelectedIndex(int ind UNUSED) { };
		virtual bool IsDir(int ind UNUSED) { return true; };
		virtual bool IsCurrentDir() { return IsDir(GetSelIndex()); };
		virtual const char * GetItemName(int ind UNUSED) { return NULL; };
		virtual int BrowsePath(const char * path UNUSED) { return 0; };
		virtual int ChangeDirectory() { return 0; };
		virtual const char * GetCurrentPath() { return NULL; };
		virtual const char * GetCurrentSelectedFilepath() { return NULL; };
		virtual u64 GetFilesize(int ind UNUSED) { return 0; };
		virtual void Refresh() { };
		virtual bool listChanged() { return false; };
		virtual int BackInDirectory() { return -1; };
		virtual ItemStruct * GetItemStruct(int pos UNUSED) { return NULL; };
		virtual ItemStruct * GetCurrentItemStruct() { return GetItemStruct(GetSelIndex()); };
		virtual void SetFilter(u8 filtermode UNUSED) {};
		virtual int ExecuteFile(const char *filepath UNUSED) { return -1; };
		virtual void Lock(void) {}
		virtual void Unlock(void) {}
		void MarkAllItems();
		void MarkCurrentItem();
		void UnMarkCurrentItem();
		void ResetMarker() { IMarker.Reset(); };
		void UpdateMarker(GuiTrigger * t);
		ItemMarker * GetItemMarker() { return (ItemMarker *) &IMarker; };
	protected:
		ItemMarker IMarker;
		int DelayCounter;
};

#endif
