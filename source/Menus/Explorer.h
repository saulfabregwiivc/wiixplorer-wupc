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
 #ifndef __EXPLORER_H_
 #define __EXPLORER_H_

#include "FileOperations/filebrowser.h"
#include "FileOperations/GuiFileBrowser.hpp"
#include "FileOperations/Browser.hpp"
#include "Prompts/DeviceMenu.h"
#include "Prompts/PopUpMenu.h"
#include "Controls/Task.hpp"

class Explorer : public GuiFrame, public sigslot::has_slots<>
{
	public:
		Explorer(GuiFrame *parent = 0, const char * path = 0);
		virtual ~Explorer();
		int LoadPath(const char * path);
		void SetFilter(u8 filtermode) { fileBrowser->SetFilter(filtermode); }
		const char * GetCurrectPath() { return fileBrowser->GetCurrentPath(); }
		void show();
		void hide();
	protected:
		void Init();
		void OnBrowserChanges(int index);
		void OnDeviceSelect(DeviceMenu *deviceMenu, int device);
		void OnRightClick(PopUpMenu *menu, int item);
		void SetDeviceImage();
		void OnCreditsButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnDeviceButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnContextButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void BackInDirectory(GuiButton *sender, int pointer, const POINT &p);
		void OnFinishedTask(Task *t);
		void OnEffectFinish(GuiElement *e);
		//! TODO: Move these
		void ProcessChoice(int choice);
		void ProcessArcChoice(int choice, const char * destCandidat);

		FileBrowser *fileBrowser;
		Browser * curBrowser;
		GuiFileBrowser * guiBrowser;
		int guiBrowserType;
		int explorerTasks;

		GuiImage * BackgroundImg;
		GuiImage * creditsImg;
		GuiImage * RefreshImg;
		GuiImage * deviceImg;
		GuiImage * AdressbarImg;

		GuiImageData * creditsImgData;
		GuiImageData * Refresh;
		GuiImageData * Background;
		GuiImageData * Address;
		GuiImageData * sdstorage;
		GuiImageData * usbstorage;
		GuiImageData * usbstorage_blue;
		GuiImageData * networkstorage;
		GuiImageData * ftpstorage;
		GuiImageData * nand_ImgData;
		GuiImageData * dvd_ImgData;

		GuiSound * btnSoundClick;
		GuiSound * btnSoundOver;

		GuiText * AdressText;

		GuiButton * CreditsBtn;
		GuiButton * RefreshBtn;
		GuiButton * deviceSwitchBtn;
		GuiButton * Adressbar;
		GuiButton * clickmenuBtn;
		GuiButton * BackInDirBtn;

		SimpleGuiTrigger * trigA;
		GuiTrigger * trigPlus;
		GuiTrigger * trigBackInDir;
};

#endif
