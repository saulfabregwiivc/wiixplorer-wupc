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
 * for WiiXplorer 2010
 ***************************************************************************/
#ifndef PLAYLIST_HPP_
#define PLAYLIST_HPP_

#include <vector>
#include <string>
#include "GUI/gui_frame.h"
#include "Controls/Scrollbar.hpp"

class PlayList : public GuiFrame, public sigslot::has_slots<>
{
	public:
		PlayList();
		virtual ~PlayList();
		const char * at(int pos);
		const char * operator[](int pos) { return at(pos); }
		int size() { return FileList.size(); }
		bool ParsePath(const char * filepath);
		void AddEntrie(const std::string &filepath);
		void RemoveEntrie(int pos);
		void ClearList();
		void Show();
		void Hide();
		bool Save();
		bool LoadList();
		int FindFile(const char * filepath);
		int GetSelectedItem() { return listOffset+selectedItem; }
		bool IsMinimized() { return Minimized; }
		void Draw();
		void Update(GuiTrigger * t);
		sigslot::signal2<PlayList *, int> ItemClicked;
	protected:
		void SwitchMinimized();
		void OnListChange(int selItem, int selIndex);
		void OnListStateChange(GuiElement *sender, int s, int c);
		void OnButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnButtonListClick(GuiButton *sender, int pointer, const POINT &p);

		int listOffset;
		int selectedItem;
		bool Minimized;
		bool listChanged;

		std::vector<std::string> FileList;

		//!Gui stuff

		std::vector<GuiButton *> ListBtn;
		std::vector<GuiText *> ListBtnTxt;
		std::vector<GuiImage *> ListBtnImgOver;

		GuiButton * PlayListBtn;
		GuiImageData * menu_selectionData;
		GuiImageData * playlistImgData;
		GuiImage * playlistImg;

		GuiSound * btnSoundClick;
		GuiSound * btnSoundOver;

		Scrollbar * scrollbar;

		GuiTrigger * trigA;
};

#endif
