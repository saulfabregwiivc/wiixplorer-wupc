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
#include <algorithm>
#include <ogc/wiilaunch.h>
#include "Controls/Taskbar.h"
#include "DeviceControls/DeviceHandler.hpp"
#include "network/DownloadTask.h"
#include "FileOperations/fileops.h"
#include "Language/gettext.h"
#include "Prompts/PromptWindows.h"
#include "Tools/tools.h"
#include "sys.h"
#include "OperaBooter.hpp"

#define OPERA_USA   0x0001000148414445LL
#define OPERA_JAP   0x000100014841444ALL
#define OPERA_PAL   0x0001000148414450LL

OperaBooter::OperaBooter(const char * xml)
{
	xmlfile = NULL;
	xmlbuffer = NULL;
	OperaID = 0;
	u32 size = 0;

	if(!xml)
		return;

	FilePath = xml;
	LoadFileToMem(xml, &xmlbuffer, &size);

	if(!xmlbuffer)
	{
		xmlfile = mxmlNewXML("1.0");
		mxmlSetWrapMargin(0);
	}
	else
	{
		xmlfile = mxmlLoadString(NULL, (const char *) xmlbuffer, MXML_OPAQUE_CALLBACK);
	}

	if(!xmlfile)
		return;

	ParseXML();

	if(FindTitle(OPERA_USA))
	{
		OperaID = OPERA_USA;
	}
	else if(FindTitle(OPERA_PAL))
	{
		OperaID = OPERA_PAL;
	}
	else if(FindTitle(OPERA_JAP))
	{
		OperaID = OPERA_JAP;
	}
}

OperaBooter::~OperaBooter()
{
	if(xmlfile)
		mxmlDelete(xmlfile);
	if(xmlbuffer)
		free(xmlbuffer);

	ClearList();
}

bool OperaBooter::AddLink()
{
	int choice = WindowPrompt(tr("Do you want to add an URL?"), 0, tr("Yes"), tr("Cancel"));
	if(!choice)
		return true;

	choice = WindowPrompt(tr("Please enter first the URL-Name."), tr("This name will be shown in the Start Menu"), tr("OK"));
	if(!choice)
		return true;

	char name[150];
	char addr[300];

	choice = OnScreenKeyboard(name, sizeof(name));
	if(!choice)
		return true;

	choice = WindowPrompt(tr("Please enter now the URL."), tr("This link will be used in Opera or as a Download Link."), tr("OK"));
	if(!choice)
		return true;

	choice = OnScreenKeyboard(addr, sizeof(addr));
	if(!choice)
		return true;

	return AddLink(name, addr);
}

bool OperaBooter::AddLink(const char * name, const char * addr)
{
	if(!xmlfile)
		return false;

	mxml_node_t * url = mxmlNewElement(xmlfile, "url");
	mxmlElementSetAttr(url, "name", name);
	mxmlElementSetAttr(url, "addr", addr);

	int position = LinkList.size();

	LinkList.resize(position+1);

	LinkList[position].name = strdup(name);
	LinkList[position].addr = strdup(addr);

	if(LinkList.size() > 1)
		Sort();

	return SaveXML();
}

void OperaBooter::RemoveLink(int pos)
{
	if(pos < 0 || pos >= (int) LinkList.size())
		return;

	if(LinkList[pos].name)
		free(LinkList[pos].name);
	if(LinkList[pos].addr)
		free(LinkList[pos].addr);

	LinkList.erase(LinkList.begin()+pos);

	if(LinkList.size() > 1)
		Sort();

	SaveXML();
}

bool OperaBooter::ParseXML()
{
	if(!xmlfile)
		return false;

	ClearList();

	mxml_node_t * node = mxmlFindElement(xmlfile, xmlfile, "url", NULL, NULL, MXML_DESCEND_FIRST);
	if (node == NULL)
		return false;

	u32 position = 0;

	while(node != NULL)
	{
		const char * name = mxmlElementGetAttr(node, "name");
		const char * addr = mxmlElementGetAttr(node, "addr");

		if(name && addr)
		{
			LinkList.resize(position+1);

			LinkList[position].name = strdup(name);
			LinkList[position].addr = strdup(addr);
			++position;
		}

		node = mxmlFindElement(node, xmlfile, "url", NULL, NULL, MXML_NO_DESCEND);
	}

	if(LinkList.size() > 1)
		Sort();

	return true;
}

void OperaBooter::ClearList()
{
	for(u32 i = 0; i < LinkList.size(); i++)
	{
		if(LinkList[i].name)
			free(LinkList[i].name);
		if(LinkList[i].addr)
			free(LinkList[i].addr);
	}
	LinkList.clear();
	std::vector<Link>().swap(LinkList);
}

bool OperaBooter::SaveXML()
{
	mxml_node_t * newfile = mxmlNewXML("1.0");
	if(!newfile)
		return false;

	mxmlSetWrapMargin(0);

	FILE *f = fopen(FilePath.c_str(), "wb");
	if(!f)
	{
		mxmlDelete(newfile);
		ShowError("%s %s", tr("Cannot write to this path:"), FilePath.c_str());
		return false;
	}

	for(u32 i = 0; i < LinkList.size(); i++)
	{
		mxml_node_t * url = mxmlNewElement(newfile, "url");
		mxmlElementSetAttr(url, "name", LinkList[i].name);
		mxmlElementSetAttr(url, "addr", LinkList[i].addr);
	}

	mxmlSaveFile(newfile, f, NULL);
	fclose(f);

	mxmlDelete(xmlfile);
	xmlfile = newfile;

	return true;
}

const char * OperaBooter::GetName(int pos)
{
	if(pos < 0 || pos >= (int) LinkList.size())
		return NULL;

	return LinkList[pos].name;
}

const char * OperaBooter::GetLink(int pos)
{
	if(pos < 0 || pos >= (int) LinkList.size())
		return NULL;

	return LinkList[pos].addr;
}

bool OperaBooter::Launch(int pos)
{
	if(pos < 0 || pos >= (int) LinkList.size())
		return false;

	if(OperaID == 0)
	{
		ShowError(tr("Opera Channel not found on the system."));
		return false;
	}

	ExitApp();

	int ret = WII_LaunchTitleWithArgs(OperaID, 0, LinkList[pos].addr, NULL);
	if(ret < 0)
	{
		ShowError(tr("Failed to load the channel."));
		return false;
	}

	return true;
}

bool OperaBooter::DownloadFile(int pos)
{
	if(pos < 0 || pos >= (int) LinkList.size())
		return false;

	if(!NetworkInitPrompt())
		return false;

	char filepath[1024];
	snprintf(filepath, sizeof(filepath), Settings.LastUsedPath.c_str());

	int choice = WindowPrompt(tr("Please enter a path."), tr("The file will be downloaded to this path."), tr("OK"));
	if(!choice)
		return true;

	choice = OnScreenKeyboard(filepath, sizeof(filepath));
	if(!choice)
		return true;

	DownloadTask *task = new DownloadTask(tr("Downloading file..."), LinkList[pos].addr, filepath);
	task->SetAutoDelete(true);
	Taskbar::Instance()->AddTask(task);
	ThreadedTaskHandler::Instance()->AddTask(task);

	return 0;
}

void OperaBooter::Sort()
{
	std::sort(LinkList.begin(), LinkList.end(), SortCallback);
}

bool OperaBooter::SortCallback(const Link & f1, const Link & f2)
{
	if(stricmp(f1.name, f2.name) > 0)
		return false;
	else
		return true;
}
