/****************************************************************************
 * Copyright (C) 2009-2012 Dimok
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
#ifndef PATH_BROWSER_H_
#define PATH_BROWSER_H_

#include "GUI/gui.h"
#include "Menus/Explorer.h"

class PathBrowser : public GuiFrame, public sigslot::has_slots<>
{
public:
	PathBrowser(const std::string &in_outPath);
	virtual ~PathBrowser();

	sigslot::signal3<PathBrowser*, bool, const std::string &> ButtonClick;
protected:
	void OnButtonClick(GuiButton *, int, const POINT &);

	GuiSound * btnSoundClick;
	GuiSound * btnSoundOver;

	GuiImageData * btnOutline;
	GuiImageData * btnOutlineOver;

	GuiText *doneBtnTxt;
	GuiImage *doneBtnImg;
	GuiButton *doneBtn;

	GuiText *cancelBtnTxt;
	GuiImage *cancelBtnImg;
	GuiButton *cancelBtn;

	Explorer * explorer;

	GuiTrigger trigA;
	GuiTrigger trigB;
};

#endif
