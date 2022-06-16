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

#include <mxml.h>
#include <algorithm>

#include "Applications.h"
#include "FileOperations/DirList.h"
#include "BootHomebrew/BootHomebrewTask.h"
#include "Controls/Clipboard.h"
#include "Controls/Taskbar.h"

Applications::Applications(const char * path)
{
	if(!path)
		return;

	Search(path);

	if(applications.size() > 1)
		Sort();
}

void Applications::Launch(int index)
{
	if(index < 0 || index >= (int) applications.size())
		return;

	ClearArguments();
	AddBootArgument(applications.at(index).path);

	BootHomebrewTask *task = new BootHomebrewTask(applications.at(index).path);
	task->SetAutoRunOnLoadFinish(true);
}

mxml_error_cb_t xmlerror(const char* error UNUSED)
{
	return NULL;
}

bool Applications::GetNameFromXML(const char *xml, char *name)
{
	mxml_node_t *tree = NULL;
	mxml_node_t *data = NULL;

	bool ret = false;

	FILE *fp = fopen(xml, "rb");
	if (fp)
	{
		mxmlSetErrorCallback((mxml_error_cb_t)xmlerror);

		tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);

		fclose(fp);
		if (tree)
		{
			data = mxmlFindElement(tree, tree, "name", NULL, NULL, MXML_DESCEND);
			if (data)
			{
				char *ptr = name;

				while (data->child != NULL && ptr < name+255)
				{
					if (data->child->value.text.whitespace)
						*ptr++ = ' ';

					strcpy(ptr, data->child->value.text.string);
					ptr += strlen(data->child->value.text.string);
					data->child = data->child->next;
				}
				*ptr = 0;

				mxmlDelete(data);
				ret = true;
			}

			mxmlDelete(tree);
		}
	}

	return ret;
}

void Applications::Reload()
{
	applications.clear();
	Search(LastPath.c_str());
}

void Applications::Search(const char * path)
{
	if (!path || strcmp(path, "") == 0)
		return;

	LastPath = path;
	std::string hbpath;

	DirList dir(path);

	int entries = dir.GetFilecount();

	if (entries > 0)
	{
		for (int j = 0; j < entries; j++)
		{
			if (!dir.IsDir(j))
				continue;

			hbpath.assign(path);
			if(hbpath[hbpath.size()-1] != '/')
				hbpath.append("/");
			hbpath.append(dir.GetFilename(j));

			DirList binary(hbpath.c_str(), ".dol,.elf");
			if (binary.GetFilecount() > 0)
			{
				App app;
				snprintf(app.path, sizeof(app.path), "%s", binary.GetFilepath(0));

				DirList meta(hbpath.c_str(), ".xml");
				if (meta.GetFileIndex("meta.xml") >= 0)
				{
					if (!GetNameFromXML(fmt("%s/meta.xml", hbpath.c_str()), app.name))
					{
						snprintf(app.name, sizeof(app.name), dir.GetFilename(j));
					}
				}
				else
				{
					snprintf(app.name, sizeof(app.name), dir.GetFilename(j));
				}

				applications.push_back(app);
			}
		}
	}

	std::vector<App>(applications).swap(applications);
}

void Applications::Sort()
{
	std::sort(applications.begin(), applications.end(), SortCallback);
}

bool Applications::SortCallback(const App & f1, const App & f2)
{
	if(strcasecmp(f1.name, f2.name) > 0)
		return false;
	else
		return true;
}
