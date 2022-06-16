/****************************************************************************
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
 * WiiXplorer 2010
 ***************************************************************************/
#ifndef _TASK_HPP_
#define _TASK_HPP_

#include "GUI/gui_button.h"

class Task : public GuiButton, public sigslot::has_slots<>
{
	public:
		Task(const std::string &title);
		virtual ~Task();
		void SetTitle(const std::string &title) { Title->SetText(title.c_str()); }
		std::string getTitle() const { return Title->toUTF8(); }
		void SetIcon(GuiImageData * img);
		void SetParameter(int p) { Parameter = p; }
		int getType() const { return TaskType; }
		sigslot::signal2<Task *, int> ShowNormal;
		sigslot::signal1<Task *> TaskBegin;
		sigslot::signal1<Task *> TaskEnd;
		sigslot::signal1<Task *> TaskDestroyed;
		enum
		{
			DEFAULT,
			PROCESS
		};
	protected:
		int TaskType;
	private:
		void OnButtonClick(GuiButton * sender, int channel, const POINT &point);

		GuiText * Title;
		GuiImage * ButtonImg;
		GuiImage * ButtonIcon;
		SimpleGuiTrigger * trigA;
		int Parameter;
};

#endif
