/***************************************************************************
 * Copyright (C) 2010
 * by Dimok, dude
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
 * Application Launcher Class
 *
 * for WiiXplorer 2010
 ***************************************************************************/

#ifndef _APPLICATIONS_H_
#define _APPLICATIONS_H_

#include <string>
#include <vector>

class Applications
{
	public:
		Applications(const char * path);
		void Search(const char * path);
		void Launch(int index);
		int Count() { return applications.size(); }
		const char * GetName(int ind) { if(ind < 0 || ind >= (int) applications.size()) return NULL; return applications.at(ind).name; }
		void Reload();
		void Sort();
	private:
		typedef struct
		{
			char path[255];
			char name[255];
		} App;
		
		static bool SortCallback(const App & f1, const App & f2);

		std::vector<App> applications;
		std::string LastPath;

		bool GetNameFromXML(const char *xml, char *name);
};

#endif
