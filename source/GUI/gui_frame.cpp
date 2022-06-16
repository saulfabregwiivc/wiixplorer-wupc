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
#include "gui_frame.h"
#include "Controls/Application.h"
#include "VideoOperations/video.h"

GuiFrame::GuiFrame(GuiFrame *p)
{
	parent = p;
	width = 0;
	height = 0;
	dim = false;
	if(parent) parent->Append(this);
}

GuiFrame::GuiFrame(int w, int h, GuiFrame *p)
{
	parent = p;
	width = w;
	height = h;
	dim = false;
	if(parent)
		parent->Append(this);
}

GuiFrame::~GuiFrame()
{
	Closing(this);

	if(parent)
		parent->Remove(this);
}

void GuiFrame::Append(GuiElement* e)
{
	if (e == NULL)
		return;

	Remove(e);
	_elements.push_back(e);
	e->SetParent(this);
}

void GuiFrame::Insert(GuiElement* e, u32 index)
{
	if (e == NULL || (index >= _elements.size()))
		return;

	Remove(e);
	_elements.insert(_elements.begin()+index, e);
	e->SetParent(this);
}

void GuiFrame::Remove(GuiElement* e)
{
	if (e == NULL)
		return;

	for (u32 i = 0; i < _elements.size(); ++i)
	{
		if(e == _elements[i])
		{
			_elements.erase(_elements.begin()+i);
			break;
		}
	}
}

void GuiFrame::RemoveAll()
{
	_elements.clear();
}

void GuiFrame::Close()
{
	Application::Instance()->PushForDelete(this);
}

void GuiFrame::DimBackground(bool d)
{
	dim = d;
}

GuiElement* GuiFrame::GetGuiElementAt(u32 index) const
{
	if (index >= _elements.size())
		return NULL;
	return _elements[index];
}

u32 GuiFrame::GetSize()
{
	return _elements.size();
}

void GuiFrame::ResetState()
{
	if(state != STATE_DISABLED)
		state = STATE_DEFAULT;

	for (u32 i = 0; i < _elements.size(); ++i)
	{
		_elements[i]->ResetState();
	}
}

void GuiFrame::SetState(int s)
{
	state = s;

	for (u32 i = 0; i < _elements.size(); ++i)
	{
		_elements[i]->SetState(s);
	}
}

void GuiFrame::SetVisible(bool v)
{
	visible = v;

	for (u32 i = 0; i < _elements.size(); ++i)
	{
		_elements[i]->SetVisible(v);
	}
}

void GuiFrame::SetBounds(iRect w)
{
	GuiElement::SetBounds(w);

	for (u32 i = 0; i < _elements.size(); ++i)
	{
		_elements[i]->SetBounds(w);
	}
}

int GuiFrame::GetSelected()
{
	// find selected element
	int found = -1;
	for (u32 i = 0; i < _elements.size(); ++i)
	{
		if(_elements[i]->GetState() == STATE_SELECTED)
		{
			found = i;
			break;
		}
	}
	return found;
}

void GuiFrame::Draw()
{
	if(!this->IsVisible() && parentElement)
		return;

	if(parentElement && dim == true)
	{
		GXColor dimColor = (GXColor){0, 0, 0, 0x70};
		Menu_DrawRectangle(0, 0, GetZPosition(), screenwidth,screenheight, &dimColor, false, true);
	}

	//! render appended items next frame but allow stop of render if size is reached
	u32 size = _elements.size();

	for (u32 i = 0; i < size && i < _elements.size(); ++i)
	{
		_elements[i]->Draw();
	}

	this->UpdateEffects();
}

void GuiFrame::Update(GuiTrigger * t)
{
	if(state == STATE_DISABLED && parentElement)
		return;

	//! update appended items next frame
	u32 size = _elements.size();

	for (u32 i = 0; i < size && i < _elements.size(); ++i)
	{
		_elements[i]->Update(t);
	}
}
