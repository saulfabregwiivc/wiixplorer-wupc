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
#include <gccore.h>
#include <fat.h>
#include <ntfs.h>
#include <ext2.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include "Language/gettext.h"
#include "Tools/tools.h"
#include "PartitionHandle.h"
#include "PartitionFormatter.hpp"

#define PARTITION_TYPE_DOS33_EXTENDED	   0x05 /* DOS 3.3+ extended partition */
#define PARTITION_TYPE_WIN95_EXTENDED	   0x0F /* Windows 95 extended partition */

#define CACHE 8
#define SECTORS 64

static inline const char * PartFromType(int type)
{
	switch (type)
	{
		case 0x00: return "Unused";
		case 0x01: return "FAT12";
		case 0x04: return "FAT16";
		case 0x05: return "Extended";
		case 0x06: return "FAT16";
		case 0x07: return "NTFS";
		case 0x0b: return "FAT32";
		case 0x0c: return "FAT32";
		case 0x0e: return "FAT16";
		case 0x0f: return "Extended";
		case 0x82: return "LxSWP";
		case 0x83: return "LINUX";
		case 0x8e: return "LxLVM";
		case 0xa8: return "OSX";
		case 0xab: return "OSXBT";
		case 0xaf: return "OSXHF";
		case 0xe8: return "LUKS";
		default: return tr("Unknown");
	}
}

PartitionHandle::PartitionHandle(const DISC_INTERFACE *discio)
{
	interface = discio;

	// Sanity check
	if (!interface)
		return;

	// Start the device and check that it is inserted
	if (!interface->startup())
		return;

	if (!interface->isInserted())
		return;

	sectorSize = CheckSectorSize(interface);

	FindPartitions();
}

PartitionHandle::~PartitionHandle()
{
	UnMountAll();

	//shutdown device
	interface->shutdown();
}

bool PartitionHandle::IsMounted(int pos)
{
	if(pos < 0 || pos >= (int) MountNameList.size())
		return false;

	if(MountNameList[pos].size() == 0)
		return false;

	return true;
}

bool PartitionHandle::Mount(int pos, const char * name)
{
	if(!valid(pos))
		return false;

	if(!name)
		return false;

	UnMount(pos);

	if(pos >= (int) MountNameList.size())
		MountNameList.resize(GetPartitionCount());

	MountNameList[pos] = name;

	if(strncmp(GetFSName(pos), "FAT", 3) == 0 || strcmp(GetFSName(pos), "GUID-Entry") == 0)
	{
		if (fatMount(MountNameList[pos].c_str(), interface, GetLBAStart(pos), CACHE, SECTORS))
		{
			if(strcmp(GetFSName(pos), "GUID-Entry") == 0)
				PartitionList[pos].FSName = "FAT";
			return true;
		}
	}

	if(strncmp(GetFSName(pos), "NTFS", 4) == 0 || strcmp(GetFSName(pos), "GUID-Entry") == 0)
	{
		if(ntfsMount(MountNameList[pos].c_str(), interface, GetLBAStart(pos), CACHE, SECTORS, NTFS_SHOW_HIDDEN_FILES | NTFS_RECOVER))
		{
			PartitionList[pos].FSName = "NTFS";
			return true;
		}
	}

	if(strncmp(GetFSName(pos), "LINUX", 5) == 0 || strcmp(GetFSName(pos), "GUID-Entry") == 0)
	{
		if(ext2Mount(MountNameList[pos].c_str(), interface, GetLBAStart(pos), CACHE, SECTORS, EXT2_FLAG_DEFAULT))
		{
			PartitionList[pos].FSName = "LINUX";
			return true;
		}
	}
	MountNameList[pos].clear();

	return false;
}

void PartitionHandle::UnMount(int pos)
{
	if(!interface)
		return;

	if(pos >= (int) MountNameList.size())
		return;

	if(MountNameList[pos].size() == 0)
		return;

	char DeviceName[20];
	snprintf(DeviceName, sizeof(DeviceName), "%s:", MountNameList[pos].c_str());

	//closing all open Files write back the cache
	fatUnmount(DeviceName);
	//closing all open Files write back the cache
	ntfsUnmount(DeviceName, true);
	//closing all open Files write back the cache
	ext2Unmount(DeviceName);
	//Remove name from list
	MountNameList[pos].clear();
}

bool PartitionHandle::IsExisting(u64 lba)
{
	for(u32 i = 0; i < PartitionList.size(); ++i)
	{
		if(PartitionList[i].LBA_Start == lba)
			return true;
	}

	return false;
}

void PartitionHandle::AddPartition(const char * name, u64 lba_start, u64 sec_count, bool bootable, u8 part_type, u8 part_num, u32 EBR_Sector)
{
	if(IsExisting(lba_start))
		return;

	u8 *buffer = new u8[MAX_SECTOR_SIZE];

	if (!interface->readSectors(lba_start, 1, buffer)) {
		delete [] buffer;
		return;
	}

	//! Partition typ can be missleading the correct partition format. Stupid lazy ass Partition Editors.
	if((memcmp(buffer + 0x36, "FAT", 3) == 0 || memcmp(buffer + 0x52, "FAT", 3) == 0) &&
		strncmp(PartFromType(part_type), "FAT", 3) != 0)
	{
		name = "FAT32";
		part_type = 0x0c;
	}
	if (memcmp(buffer + 0x03, "NTFS", 4) == 0)
	{
		name = "NTFS";
		part_type = 0x07;
	}

	int part = PartitionList.size();
	PartitionList.resize(part+1);

	PartitionList[part].FSName = name;
	PartitionList[part].LBA_Start = lba_start;
	PartitionList[part].SecCount = sec_count;
	PartitionList[part].Bootable = bootable;
	PartitionList[part].PartitionType = part_type;
	PartitionList[part].PartitionNum = part_num;
	PartitionList[part].EBR_Sector = EBR_Sector;
	delete [] buffer;
}

int PartitionHandle::FindPartitions()
{
	MASTER_BOOT_RECORD *mbr = (MASTER_BOOT_RECORD *) malloc(MAX_SECTOR_SIZE);
	if(!mbr)
		return -1;

	// Read the first sector on the device
	if (!interface->readSectors(0, 1, mbr)) {
		free(mbr);
		return -1;
	}

	// If this is the devices master boot record
	if (mbr->signature != MBR_SIGNATURE && mbr->signature != MBR_SIGNATURE_MOD ) {
		free(mbr);
		return -1;
	}

	for (int i = 0; i < 4; i++)
	{
		PARTITION_RECORD * partition = (PARTITION_RECORD *) &mbr->partitions[i];

		if(partition->type == PARTITION_TYPE_GPT)
		{
			int ret = CheckGPT(i);
			if(ret == 0)
				break;
		}

		if(partition->type == PARTITION_TYPE_DOS33_EXTENDED || partition->type == PARTITION_TYPE_WIN95_EXTENDED)
		{
			CheckEBR(i, le32(partition->lba_start));
			continue;
		}

		if(le32(partition->block_count) > 0)
		{
			AddPartition(PartFromType(partition->type), le32(partition->lba_start),
						 le32(partition->block_count), (partition->status == PARTITION_BOOTABLE),
						 partition->type, i);
		}
	}

	free(mbr);

	return 0;
}

void PartitionHandle::CheckEBR(int PartNum, sec_t ebr_lba)
{
	EXTENDED_BOOT_RECORD *ebr = (EXTENDED_BOOT_RECORD *) malloc(MAX_SECTOR_SIZE);
	if(!ebr)
		return;

	sec_t next_erb_lba = 0;

	do
	{
		// Read and validate the extended boot record
		if (!interface->readSectors(ebr_lba + next_erb_lba, 1, ebr))
			break;

		if (ebr->signature != EBR_SIGNATURE)
			break;

		PARTITION_RECORD * partition = (PARTITION_RECORD *) &ebr->partition;

		if(le32(partition->block_count) > 0)
		{
			AddPartition(PartFromType(partition->type), ebr_lba + next_erb_lba + le32(partition->lba_start),
						 le32(partition->block_count), (partition->status == PARTITION_BOOTABLE),
						 partition->type, PartNum, ebr_lba + next_erb_lba);
		}
		// Get the start sector of the current partition
		// and the next extended boot record in the chain
		next_erb_lba = le32(ebr->next_ebr.lba_start);
	}
	while(next_erb_lba > 0);

	free(ebr);
}

static const u8 TYPE_UNUSED[16] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
static const u8 TYPE_BIOS[16] = { 0x48,0x61,0x68,0x21,0x49,0x64,0x6F,0x6E,0x74,0x4E,0x65,0x65,0x64,0x45,0x46,0x49 };
//static const u8 TYPE_LINUX_MS_BASIC_DATA[16] = { 0xA2,0xA0,0xD0,0xEB,0xE5,0xB9,0x33,0x44,0x87,0xC0,0x68,0xB6,0xB7,0x26,0x99,0xC7 };

int PartitionHandle::CheckGPT(int PartNum)
{
	GPT_HEADER *gpt_header = (GPT_HEADER *) malloc(MAX_SECTOR_SIZE);
	if(!gpt_header)
		return -1;

	// Read and validate the extended boot record
	if (!interface->readSectors(1, 1, gpt_header)) {
		free(gpt_header);
		return -1;
	}

	if(strncmp(gpt_header->magic, "EFI PART", 8) != 0) {
		free(gpt_header);
		return -1;
	}

	gpt_header->part_table_lba = le64(gpt_header->part_table_lba);
	gpt_header->part_entries = le32(gpt_header->part_entries);
	gpt_header->part_entry_size = le32(gpt_header->part_entry_size);
	gpt_header->part_entry_checksum = le32(gpt_header->part_entry_checksum);

	u8 * sector_buf = (u8 *) malloc(MAX_SECTOR_SIZE);
	if(!sector_buf) {
		free(gpt_header);
		return -1;
	}

	u64 next_lba = gpt_header->part_table_lba;

	for(u32 i = 0; i < gpt_header->part_entries; ++i)
	{
		if (!interface->readSectors(next_lba, 1, sector_buf))
			break;

		for(u32 n = 0; n < sectorSize/gpt_header->part_entry_size; ++n, ++i)
		{
			GUID_PART_ENTRY * part_entry = (GUID_PART_ENTRY *) (sector_buf+gpt_header->part_entry_size*n);

			if(memcmp(part_entry->part_type_guid, TYPE_UNUSED, 16) == 0)
				continue;

			bool bootable = (memcmp(part_entry->part_type_guid, TYPE_BIOS, 16) == 0);

			AddPartition("GUID-Entry", le64(part_entry->part_first_lba), le64(part_entry->part_last_lba),
						 bootable, PARTITION_TYPE_GPT, PartNum);
		}

		next_lba++;
	}

	free(sector_buf);
	free(gpt_header);

	return 0;
}

int PartitionHandle::CheckSectorSize(const DISC_INTERFACE* interface)
{
	int counter1 = 0;
	int counter2 = 0;
	int i;

	u8 *memblock = (u8 *) memalign(32, MAX_SECTOR_SIZE);
	if(!memblock)
		return 512;

	memset(memblock, 0x00, MAX_SECTOR_SIZE);

	if(!interface->readSectors(0, 1, memblock)) {
		free(memblock);
		return 512;
	}

	for(i = 0; i < MAX_SECTOR_SIZE; ++i)
	{
		if(memblock[i] != 0x00)
			counter1++;
	}

	memset(memblock, 0xFF, MAX_SECTOR_SIZE);

	if(!interface->readSectors(0, 1, memblock)) {
		free(memblock);
		return 512;
	}

	for(i = 0; i < MAX_SECTOR_SIZE; ++i)
	{
		if(memblock[i] != 0xFF)
			counter2++;
	}

	free(memblock);

	if(counter1 <= 512 && counter2 <= 512)
		return 512;

	if(counter1 <= 1024 && counter2 <= 1024)
		return 1024;

	if(counter1 <= 2048 && counter2 <= 2048)
		return 2048;

	return 4096;
}
