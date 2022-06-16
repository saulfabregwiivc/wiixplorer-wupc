/***************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * Original ListBrowser by Tantric (C) 2009
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
#ifndef __LISTBROWSER_HPP_
#define __LISTBROWSER_HPP_

#include <vector>
#include "GuiFileBrowser.hpp"
#include "Controls/Scrollbar.hpp"

//!Display a list of files
class ListFileBrowser : public GuiFileBrowser, public sigslot::has_slots<>
{
	public:
		ListFileBrowser(Browser * filebrowser, int w, int h);
		virtual ~ListFileBrowser();
		void ResetState();
		void SetSelected(int i);
		void Refresh() { if(browser) browser->Refresh(); OnListChange(0, 0); };
		void Draw();
		void Update(GuiTrigger * t);
	protected:
		void OnListChange(int selItem, int selIndex);
		void AddButton();
		void SetButton(int i, const char * name, u64 filesize, bool dir, bool enable);
		void RemoveButton(int i);
		GuiImage * GetIconFromExt(const char * fileext, bool dir);
		void OnClicked(GuiButton *sender, int pointer, const POINT &p);
		int selectedItem;
		int numEntries;

		std::vector<GuiText *> fileBtnText;
		std::vector<GuiText *> fileBtnTextOver;
		std::vector<GuiText *> fileSizeText;
		std::vector<GuiImage *> fileSelectionImg;
		std::vector<GuiImage *> fileBtnIcon;
		std::vector<GuiButton *> fileBtn;

		Scrollbar * scrollbar;

		GuiImageData * bgFileSelectionEntry;
		GuiImageData * fileArchives;
		GuiImageData * fileDefault;
		GuiImageData * fileFolder;
		GuiImageData * fileGFX;
		GuiImageData * filePLS;
		GuiImageData * fileSFX;
		GuiImageData * fileTXT;
		GuiImageData * fileXML;
		GuiImageData * fileVID;
		GuiImageData * filePDF;

		GuiSound * btnSoundOver;
		GuiSound * btnSoundClick;
		GuiTrigger * trigA;
};

#endif
