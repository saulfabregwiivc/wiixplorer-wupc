 /***************************************************************************
 * Copyright (C) 2009
 * by Dimok
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
 * URL List Class
 *
 * Listing all SubURLs of a given URL
 * for Wii-Xplorer 2009
 ***************************************************************************/
#ifndef ___URLLIST_H_
#define ___URLLIST_H_

#include "network/networkops.h"
#include "network/http.h"

typedef struct
{
	char	*URL;
	bool	fullurl;
} Link_Info;

class URL_List
{
	public:
		//!Constructor
		//!\param url from where to get the list of sublinks
		URL_List(const char *url);
		//!Destructor
		~URL_List();
		//! Get an URL from the List
		//!\param list index
		char * GetURL(int index);
		//! Check if URL is a full standalone URL or just a file/SubURL
		bool IsFullURL(int ind);
		//! Get the number of links counted
		int GetURLCount();
		//! Sort list
		void SortList();
	protected:
		int urlcount;
		Link_Info *Links;
};

#endif
