/****************************************************************************
 * Copyright (C) 2012 Dimok
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
#include "ThrobberWindow.h"
#include "Memory/Resources.h"

ThrobberWindow::ThrobberWindow(const char *title, const char *msg, const char *btnText)
	: PromptWindow(title, msg, btnText)
	, bAutoRotate(true)
{
	throbberImgData = Resources::GetImageData("throbber.png");
	throbberImg = new GuiImage(throbberImgData);
	throbberImg->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	throbberImg->SetPosition(0, 35);
	Append(throbberImg);
}

ThrobberWindow::~ThrobberWindow()
{
	Remove(throbberImg);

	Resources::Remove(throbberImgData);
	delete throbberImg;
}

void ThrobberWindow::Draw()
{
	if(bAutoRotate)
		AdvanceRotation();

	PromptWindow::Draw();
}
