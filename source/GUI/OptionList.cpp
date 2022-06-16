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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <gctypes.h>
#include "OptionList.hpp"

OptionList::OptionList()
{
}

OptionList::~OptionList()
{
	ClearList();
}

void OptionList::SetName(int i, const char *format, ...)
{
	if(i < (int) name.size())
	{
		if(name.at(i))
			delete [] name.at(i);

		name.at(i) = NULL;
	}

	char *tmp=0;
	va_list va;
	va_start(va, format);
	if((vasprintf(&tmp, format, va)>=0) && tmp)
	{
		if(i >= (int) name.size())
		{
			Resize(i+1);
		}

		name.at(i) = new char[strlen(tmp)+1];
		strcpy(name.at(i), tmp);

		listChanged = true;
	}
	va_end(va);

	if(tmp)
		free(tmp);
}

void OptionList::SetValue(int i, const char *format, ...)
{
	if(i < (int) value.size())
	{
		if(value.at(i))
			delete [] value.at(i);

		value.at(i) = NULL;
	}

	char *tmp=0;
	va_list va;
	va_start(va, format);
	if((vasprintf(&tmp, format, va)>=0) && tmp)
	{
		if(i >= (int) value.size())
		{
			Resize(i+1);
		}

		value.at(i) = new char[strlen(tmp)+1];
		strcpy(value.at(i), tmp);

		listChanged = true;
	}
	va_end(va);

	if(tmp)
		free(tmp);
}

const char * OptionList::GetName(int i)
{
	if(i < 0 || i >= (int) name.size())
		return NULL;

	return name.at(i);
}

const char * OptionList::GetValue(int i)
{
	if(i < 0 || i >= (int) value.size())
		return NULL;

	return value.at(i);
}

void OptionList::Resize(int size)
{
	while(size < (int) name.size())
	{
		RemoveOption(name.size()-1);
	}

	int oldsize = name.size();

	name.resize(size);
	value.resize(size);

	for(u32 i = oldsize; i < name.size(); i++)
	{
		name.at(i) = NULL;
		value.at(i) = NULL;
	}
}

void OptionList::RemoveOption(int i)
{
	if(i < 0 || i >= (int) name.size())
		return;

	if(name.at(i))
	{
		delete [] name.at(i);
		name.at(i) = NULL;
	}
	if(value.at(i))
	{
		delete [] value.at(i);
		value.at(i) = NULL;
	}

	name.erase(name.begin()+i);
	value.erase(value.begin()+i);
}

void OptionList::ClearList()
{
	for (u32 i = 0; i < name.size(); i++)
	{
		if(name.at(i))
		{
			delete [] name.at(i);
			name.at(i) = NULL;
		}
	}
	for (u32 i = 0; i < value.size(); i++)
	{
		if(value.at(i))
		{
			delete [] value.at(i);
			value.at(i) = NULL;
		}
	}

	name.clear();
	value.clear();
}
