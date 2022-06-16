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
#ifndef OPTIONLIST_HPP_
#define OPTIONLIST_HPP_

#include <vector>

class OptionList
{
	public:
		OptionList();
		virtual ~OptionList();
		void SetName(int i, const char *format, ...) __attribute__((format (printf, 3, 4)));
		void SetValue(int i, const char *format, ...) __attribute__((format (printf, 3, 4)));
		const char * GetName(int i);
		const char * GetValue(int i);
		void Resize(int size);
		int GetLength()	{ return name.size(); }
		bool IsChanged() { bool ret = listChanged; listChanged = false; return ret;}
		void RemoveOption(int i);
		void ClearList();
	private:
		std::vector<char *> name;
		std::vector<char *> value;
		bool listChanged;
};

#endif

