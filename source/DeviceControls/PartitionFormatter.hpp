 /****************************************************************************
 * Copyright (C) 2010
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
 * for WiiXplorer 2010
 ***************************************************************************/
#ifndef PARTITION_FORMATTER_HPP_
#define PARTITION_FORMATTER_HPP_

#include <gccore.h>

class PartitionFormatter
{
	public:
		static int FormatToFAT32(const DISC_INTERFACE *interface, sec_t lba, sec_t sec_count);
		static int WriteMBR_FAT32(const DISC_INTERFACE *interface, sec_t part_lba);
		static int WriteEBR_FAT32(const DISC_INTERFACE *interface, sec_t erb_lba, sec_t part_lba);
		static int SetActive(const DISC_INTERFACE *interface, int partition_number);
};


#endif
