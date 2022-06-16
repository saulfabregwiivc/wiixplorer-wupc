/***************************************************************************
 * Copyright (C) 2009
 * by Dimok & r-win
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
 * Resources.cpp
 *
 * for WiiXplorer 2009
 ***************************************************************************/

#include "Resources.h"
#include "filelist.h"

Resources * Resources::instance = NULL;

Resources * Resources::Instance()
{
	if (instance == NULL)
	{
		instance = new Resources();
	}
	return instance;
}

void Resources::DestroyInstance()
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
}

Resources::~Resources()
{
	// Delete all images...
	std::map<const u8 *, GuiImageData *>::iterator imgitr;
	for (imgitr = images.begin(); imgitr != images.end(); imgitr++)
	{
		delete imgitr->second;
	}
	images.clear();

	// Delete all sounds...
	std::map<const u8 *, GuiSound *>::iterator snditr;
	for (snditr = sounds.begin(); snditr != sounds.end(); snditr++)
	{
		delete snditr->second;
	}
	sounds.clear();

	imageCount.clear();
	soundCount.clear();
}

const u8 *Resources::GetFile(const char *filename)
{
	if(!filename)
		return NULL;

	for(int i = 0; RecourceList[i].filename != NULL; ++i)
	{
		if(strcasecmp(filename, RecourceList[i].filename) == 0)
		{
			return (RecourceList[i].CustomFile ? RecourceList[i].CustomFile : RecourceList[i].DefaultFile);
		}
	}

	return NULL;
}

u32 Resources::GetFileSize(const char *filename)
{
	if(!filename)
		return 0;

	for(int i = 0; RecourceList[i].filename != NULL; ++i)
	{
		if(strcasecmp(filename, RecourceList[i].filename) == 0)
		{
			return (RecourceList[i].CustomFile ? RecourceList[i].CustomFileSize : RecourceList[i].DefaultFileSize);
		}
	}

	return 0;
}

GuiImageData *Resources::GetImageData(const char *filename)
{
	if(!filename)
		return NULL;

	for(int i = 0; RecourceList[i].filename != NULL; ++i)
	{
		if(strcasecmp(filename, RecourceList[i].filename) == 0)
		{
			const u8 *img = (RecourceList[i].CustomFile ? RecourceList[i].CustomFile : RecourceList[i].DefaultFile);
			u32 imgSize = (RecourceList[i].CustomFile ? RecourceList[i].CustomFileSize : RecourceList[i].DefaultFileSize);
			return Resources::Instance()->InternalGetImageData(img, imgSize);
		}
	}
	return NULL;
}

GuiSound *Resources::GetSound(const char *filename)
{
	if(!filename)
		return NULL;

	for(int i = 0; RecourceList[i].filename != NULL; ++i)
	{
		if(strcasecmp(filename, RecourceList[i].filename) == 0)
		{
			const u8 *snd = (RecourceList[i].CustomFile ? RecourceList[i].CustomFile : RecourceList[i].DefaultFile);
			u32 sndSize = (RecourceList[i].CustomFile ? RecourceList[i].CustomFileSize : RecourceList[i].DefaultFileSize);
			return Resources::Instance()->InternalGetSound(snd, sndSize);
		}
	}
	return NULL;
}

void Resources::Remove(GuiImageData *img)
{
	Resources::Instance()->InternalRemoveImageData(img);
}

void Resources::Remove(GuiSound *snd)
{
	Resources::Instance()->InternalRemoveSound(snd);
}

GuiImageData *Resources::InternalGetImageData(const u8 *img, u32 imgSize)
{
	std::map<const u8 *, GuiImageData *>::iterator itr = images.find(img);
	if (itr == images.end())
	{
		// Not found, create a new one
		GuiImageData *d = new GuiImageData(img, imgSize);
		images[img] = d;
		imageCount[d] = 1;
		return d;

	}
	imageCount[itr->second]++;
	return itr->second;
}

GuiSound *Resources::InternalGetSound(const u8 *snd, s32 sndSize)
{
	std::map<const u8 *, GuiSound *>::iterator itr = sounds.find(snd);
	if (itr == sounds.end())
	{
		// Not found, create a new one
		// Only unallocated sounds go here
		GuiSound *d = new GuiSound(snd, sndSize, false);
		sounds[snd] = d;
		soundCount[d] = 1;
		return d;

	}
	soundCount[itr->second]++;
	return itr->second;
}

void Resources::InternalRemoveImageData(GuiImageData *img)
{
	std::map<GuiImageData *, int>::iterator itr = imageCount.find(img);
	if (itr == imageCount.end())
		return;

	itr->second--;

	if (itr->second == 0) // Remove the resource
	{
		imageCount.erase(itr);

		std::map<const u8 *, GuiImageData *>::iterator iitr;
		for (iitr = images.begin(); iitr != images.end(); iitr++)
		{
			if (iitr->second == img)
			{
				delete iitr->second;
				images.erase(iitr);
				break;
			}
		}
	}
}

void Resources::InternalRemoveSound(GuiSound *snd)
{
	std::map<GuiSound *, int>::iterator itr = soundCount.find(snd);
	if (itr == soundCount.end())
		return;

	itr->second--;

	if (itr->second == 0) // Remove the resource
	{
		soundCount.erase(itr);

		std::map<const u8 *, GuiSound *>::iterator iitr;
		for (iitr = sounds.begin(); iitr != sounds.end(); iitr++)
		{
			if (iitr->second == snd)
			{
				delete iitr->second;
				sounds.erase(iitr);
				break;
			}
		}
	}
}
