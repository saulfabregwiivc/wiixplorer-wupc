/****************************************************************************
 * Copyright (C) 2009-2011 Dimok
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
 #ifndef __PROPERTIES_H_
 #define __PROPERTIES_H_

#include <gctypes.h>
#include <unistd.h>

#include "GUI/gui.h"
#include "Memory/Resources.h"
#include "FileOperations/ItemMarker.h"

class Properties : public GuiFrame, public sigslot::has_slots<>
{
	public:
		Properties(ItemMarker * IMarker);
		virtual ~Properties();
		void Draw();
	private:
		void UpdateSizeValue();
		void InternalFolderSizeGain();
		static void * FolderSizeThread(void *arg);
		void OnButtonClick(GuiButton *sender, int pointer, const POINT &p);

		ItemMarker Marker;
		bool folder;
		bool sizegaindone;
		bool bClosing;
		u64 TotalSize;
		u32 FileCount;
		u64 OldSize;
		u64 devicefree;
		u64 devicesize;
		lwp_t foldersizethread;

		GuiImage * dialogBoxImg;
		GuiImage * TitleImg;
		GuiImage * arrowUpImg;
		GuiImage * arrowUpImgOver;

		GuiImageData * dialogBox;
		GuiImageData * titleData;
		GuiImageData * arrowUp;
		GuiImageData * arrowUpOver;

		GuiSound * btnClick;

		GuiText * TitleTxt;
		GuiText * filepathTxt;
		GuiText * filepathvalTxt;
		GuiText * filecountTxt;
		GuiText * filecountTxtVal;
		GuiText * filesizeTxt;
		GuiText * filesizeTxtVal;
		GuiText * filetypeTxt;
		GuiText * filetypeTxtVal;
		GuiText * devicefreeTxt;
		GuiText * devicefreeTxtVal;
		GuiText * devicetotalTxt;
		GuiText * devicetotalTxtVal;
		GuiText * last_accessTxt;
		GuiText * last_accessTxtVal;
		GuiText * last_modifTxt;
		GuiText * last_modifTxtVal;
		GuiText * last_changeTxt;
		GuiText * last_changeTxtVal;

		GuiButton * CloseBtn;

		SimpleGuiTrigger * trigA;
		GuiTrigger * trigB;
};

#endif
