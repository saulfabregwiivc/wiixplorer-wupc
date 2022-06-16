/****************************************************************************
 * Copyright (C) 2011 Dimok
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
#ifndef _CLIPBOARD_H_
#define _CLIPBOARD_H_

#include "FileOperations/ItemMarker.h"

enum
{
	OP_NONE = 0,
	OP_COPY,
	OP_MOVE,
	OP_DELETE,
	OP_ADDTOZIP,
};

class Clipboard : public ItemMarker
{
	public:
		static Clipboard * Instance() { if(!instance) instance = new Clipboard(); return instance; }
		static void DestroyInstance() { delete instance; instance = NULL; }
		int Operation;
	private:
		Clipboard() : Operation(OP_NONE) {}

		static Clipboard *instance;
};

#endif
