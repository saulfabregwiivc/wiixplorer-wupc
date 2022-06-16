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
#include "ImageConvertTask.h"
#include "Controls/Application.h"
#include "Prompts/ThrobberWindow.h"

ImageConvertTask::ImageConvertTask(ImageConverter *converter, bool silent)
	: Task(tr("Converting image"))
	, m_converter(converter)
	, m_silent(silent)
{
}

void ImageConvertTask::Execute(void)
{
	TaskBegin(this);

	if(!m_converter)
	{
		TaskEnd(this);
		return;
	}

	ThrobberWindow * Prompt = NULL;

	if(!m_silent)
	{
		Prompt = new ThrobberWindow(tr("Converting image..."), tr("Please wait..."));
		Prompt->DimBackground(true);
		Application::Instance()->Append(Prompt);
		Application::Instance()->SetUpdateOnly(Prompt);
	}

	bool result = m_converter->Convert();
	if(result)
		ThrowMsg(tr("Image successfully converted."), 0);
	else
		ThrowMsg(tr("Could not convert image."), 0);

	if(!m_silent)
	{
		Prompt->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
		Application::Instance()->PushForDelete(Prompt);
	}

	TaskEnd(this);
}
