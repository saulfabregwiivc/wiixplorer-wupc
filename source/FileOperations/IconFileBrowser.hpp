/****************************************************************************
 * Copyright (C) 2010-2011 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef __ICONBROWSER_HPP_
#define __ICONBROWSER_HPP_

#include "Controls/Scrollbar.hpp"
#include "GuiFileBrowser.hpp"
#include "GUI/gui_tooltip.h"

using namespace std;

class IconFileBrowser : public GuiFileBrowser, public sigslot::has_slots<>
{
	public:
		IconFileBrowser(Browser * filebrowser, int w, int h);
		virtual ~IconFileBrowser();
		void ResetState();
		void SetSelected(int i);
		void Refresh() { if(browser) browser->Refresh(); OnListChange(0, 0); };
		void Draw();
		void Update(GuiTrigger * t);
	protected:
		void OnListChange(int selItem, int selIndex);
		void OnButtonClicked(GuiButton *sender, int pointer, const POINT &p);
		GuiImage * GetIconFromExt(const char * fileext, bool dir);
		void AddButton();
		void SetButton(int i, const char * name, bool dir, bool enable, int x, int y);
		void RemoveButton(int i);

		u16 RowSize;
		u16 PageSize;
		int selectedItem;

		vector<GuiText *> ButtonText;
		vector<GuiImage *> FileSelectionImg;
		vector<GuiImage *> ButtonImg;
		vector<GuiButton *> Buttons;
		vector<GuiTooltip *> Tooltip;

		Scrollbar * scrollbar;

		GuiImageData * bgFileSelection;
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
