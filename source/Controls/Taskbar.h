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
#ifndef _TASKBAR_H
#define _TASKBAR_H

#include "GUI/gui_frame.h"
#include "GUI/PictureButton.h"
#include "Prompts/PopUpMenu.h"
#include "Controls/Callback.hpp"
#include "Controls/Task.hpp"

//! Explorer class, forward declaration
class Explorer;

class Taskbar : public GuiFrame, public sigslot::has_slots<>
{
	public:
		static Taskbar *Instance() { if(!instance) instance = new Taskbar(); return instance; }
		static void DestroyInstance() { delete instance; instance = NULL; }

		void AddTask(Task * t);
		void RemoveTask(Task * t);

		u32 TaskCount() const { return Tasks.size(); }

		/*
		 * Temporary function till multi window feature
		 */
		Explorer *GetExplorer() { return mainExplorer; }
	protected:
		void Draw();

	private:
		Taskbar();
		virtual ~Taskbar();

		void OnStartButtonClick(GuiButton *sender, int pointer, const POINT &p);
		void OnMusicPlayerClick(GuiButton *sender, int pointer, const POINT &p);
		void OnStartmenuItemClick(PopUpMenu *menu, int item);
		void OnAppsMenuClick(PopUpMenu *menu, int item);
		void OnChannelsMenuClick(PopUpMenu *menu, int item);
		void OnUrlsMenuClick(PopUpMenu *menu, int item);
		void OnMenuClosing(GuiFrame *menu);

		static Taskbar *instance;

		Explorer *mainExplorer;

		GuiImageData *taskbarImgData;
		GuiImageData *HeadPhonesData;
		GuiImageData *WifiData;
		GuiImage *taskbarImg;
		GuiImage *HeadPhonesImg;
		GuiImage *WifiImg;
		GuiText *timeTxt;
		PictureButton *startBtn;
		SimpleGuiTrigger *trigA;
		GuiSound *soundClick;
		GuiSound *soundOver;
		GuiButton *Musicplayer;
		std::vector<Task *> Tasks;
};

#endif // _TASKBAR_H
