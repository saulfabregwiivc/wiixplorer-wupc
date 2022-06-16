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
#include "Language/gettext.h"
#include "PartitionHandle.h"
#include "PartitionFormatter.hpp"
#include "Tools/tools.h"
#include "stdafx.h"

typedef struct tagFAT_BOOTSECTOR32
{
	// Common fields.
	u8 sJmpBoot[3];
	u8 sOEMName[8];
	u16 wBytsPerSec;
	u8 bSecPerClus;
	u16 wRsvdSecCnt;
	u8 bNumFATs;
	u16 wRootEntCnt;
	u16 wTotSec16;
	u8 bMedia;
	u16 wFATSz16;
	u16 wSecPerTrk;
	u16 wNumHeads;
	u32 dHiddSec;
	u32 dTotSec32;
	// Fat 32/16 only
	u32 dFATSz32;
	u16 wExtFlags;
	u16 wFSVer;
	u32 dRootClus;
	u16 wFSInfo;
	u16 wBkBootSec;
	u8 Reserved[12];
	u8 bDrvNum;
	u8 Reserved1;
	u8 bBootSig; // == 0x29 if next three fields are ok
	u32 dBS_VolID;
	u8 sVolLab[11];
	u8 sBS_FilSysType[8];

} __attribute__((__packed__)) FAT_BOOTSECTOR32;

typedef struct {
	u32 dLeadSig;
	u8 sReserved1[480];
	u32 dStrucSig;
	u32 dFree_Count;
	u32 dNxt_Free;
	u8 sReserved2[12];
	u32 dTrailSig;
} __attribute__((__packed__)) FAT_FSINFO;


static inline u8 get_sectors_per_cluster (u64 DiskSizeBytes)
{
	u8 ret = 0x01; // 1 sector per cluster
	u32 DiskSizeMB = DiskSizeBytes/(1024*1024);

	// 512 MB to 8,191 MB 4 KB
	if (DiskSizeMB > 512)
		ret = 0x8;

	// 8,192 MB to 16,383 MB 8 KB
	if (DiskSizeMB > 8192)
		ret = 0x10;

	// 16,384 MB to 32,767 MB 16 KB
	if (DiskSizeMB > 16384)
		ret = 0x20; // ret = 0x20;

	// Larger than 32,768 MB 32 KB
	if (DiskSizeMB > 32768)
		ret = 0x40;  // ret = 0x40;

	return ret;
}

static inline u32 MakeVolumeID()
{
	time_t rawtime = time(0);
	struct tm * timeinfo = localtime(&rawtime);

	u16 hi = le16(timeinfo->tm_mday + (timeinfo->tm_mon << 8) + (timeinfo->tm_sec << 8));
	u16 lo = le16((timeinfo->tm_hour << 8) + timeinfo->tm_min + timeinfo->tm_year + 1900);

	return (lo + (hi << 16));
}

int PartitionFormatter::FormatToFAT32(const DISC_INTERFACE *interface, sec_t lba, sec_t sec_count)
{
	// Sanity check
	if (!interface)
		return -1;

	// Start the device and check that it is inserted
	if (!interface->startup())
		return -1;

	if (!interface->isInserted())
		return -1;

	if(sec_count < 0xFFFF)
	{
		ShowError(tr("Partition is too small."));
		return -1;
	}

	int BytesPerSect = PartitionHandle::CheckSectorSize(interface);
	u16 ReservedSectCount = 32;
	u8 NumFATs = 2;

	u8 * buffer = (u8 *) memalign(32, ALIGN32(BytesPerSect*18));
	if(buffer == NULL)
	{
		ShowError(tr("Not enough memory."));
		return -1;
	}

	memset(buffer, 0, BytesPerSect*18);

	FAT_BOOTSECTOR32 * FAT32BootSect = (FAT_BOOTSECTOR32 *) (buffer+16*BytesPerSect);
	FAT_FSINFO * FAT32FsInfo = (FAT_FSINFO*) (buffer+17*BytesPerSect);

	// fill out the boot sector and fs info
	FAT32BootSect->sJmpBoot[0] = 0xEB;
	FAT32BootSect->sJmpBoot[1] = 0x5A;
	FAT32BootSect->sJmpBoot[2] = 0x90;
	memcpy(FAT32BootSect->sOEMName, "MSWIN4.1", 8);

	FAT32BootSect->wBytsPerSec = le16(BytesPerSect);

	u8 SectorsPerCluster = get_sectors_per_cluster((u64) sec_count * (u64) BytesPerSect);

	FAT32BootSect->bSecPerClus = SectorsPerCluster;
	FAT32BootSect->wRsvdSecCnt = le16(ReservedSectCount);
	FAT32BootSect->bNumFATs = NumFATs;
	FAT32BootSect->wRootEntCnt = 0;
	FAT32BootSect->wTotSec16 = 0;
	FAT32BootSect->bMedia = 0xF8;
	FAT32BootSect->wFATSz16 = 0;
	FAT32BootSect->wSecPerTrk = le16(63); //SectorsPerTrack;
	FAT32BootSect->wNumHeads = le16(255); //TracksPerCylinder;
	FAT32BootSect->dHiddSec = le32(lba); //HiddenSectors;
	FAT32BootSect->dTotSec32 = le32(sec_count);

	// This is based on
	// http://hjem.get2net.dk/rune_moeller_barnkob/filesystems/fat.html
	u32 FatSize = (4*(sec_count-ReservedSectCount)/((SectorsPerCluster*BytesPerSect)+(4*NumFATs)))+1;

	FAT32BootSect->dFATSz32 = le32(FatSize);
	FAT32BootSect->wExtFlags = 0;
	FAT32BootSect->wFSVer = 0;
	FAT32BootSect->dRootClus = le32(2);
	FAT32BootSect->wFSInfo = le16(1);
	FAT32BootSect->wBkBootSec = le16(6); //BackupBootSect
	FAT32BootSect->bDrvNum = 0x80;
	FAT32BootSect->Reserved1 = 0;
	FAT32BootSect->bBootSig = 0x29;

	FAT32BootSect->dBS_VolID = MakeVolumeID();
	memcpy(FAT32BootSect->sVolLab, "NO NAME	", 11);
	memcpy(FAT32BootSect->sBS_FilSysType, "FAT32   ", 8);
	((u8 *)FAT32BootSect)[510] = 0x55; //Boot Record Signature
	((u8 *)FAT32BootSect)[511] = 0xAA; //Boot Record Signature

	// FSInfo sect signatures
	FAT32FsInfo->dLeadSig = le32(0x41615252);
	FAT32FsInfo->dStrucSig = le32(0x61417272);
	FAT32FsInfo->dTrailSig = le32(0xaa550000);
	((u8 *)FAT32FsInfo)[510] = 0x55; //Boot Record Signature
	((u8 *)FAT32FsInfo)[511] = 0xAA; //Boot Record Signature

	// First FAT Sector
	u32 FirstSectOfFat[3];
	FirstSectOfFat[0] = le32(0x0ffffff8);  // Reserved cluster 1 media id in low byte
	FirstSectOfFat[1] = le32(0x0fffffff);  // Reserved cluster 2 EOC
	FirstSectOfFat[2] = le32(0x0fffffff);  // end of cluster chain for root dir

	u32 UserAreaSize = sec_count - ReservedSectCount - (NumFATs*FatSize);
	u32 ClusterCount = UserAreaSize/SectorsPerCluster;

	if (ClusterCount > 0x0FFFFFFF)
	{
		free(buffer);
		ShowError(tr("This drive has more than 2^28 clusters. Partition might be too small."));
		return -1;
	}

	if (ClusterCount < 65536)
	{
		free(buffer);
		ShowError(tr("FAT32 must have at least 65536 clusters"));
		return -1;
	}

	u32 FatNeeded = (ClusterCount * 4 + (BytesPerSect-1))/BytesPerSect;
	if (FatNeeded > FatSize)
	{
		free(buffer);
		ShowError(tr("This drive is too big"));
		return -1;
	}

	// fix up the FSInfo sector
	FAT32FsInfo->dFree_Count = le32((UserAreaSize/SectorsPerCluster)-1);
	FAT32FsInfo->dNxt_Free = le32(3); // clusters 0-1 resered, we used cluster 2 for the root dir

	/** Now all is done and we start writting **/

	// First zero out ReservedSect + FatSize * NumFats + SectorsPerCluster
	u32 SystemAreaSize = (ReservedSectCount+(NumFATs*FatSize) + SectorsPerCluster);
	u32 done = 0;
	// Read the first sector on the device
	while(SystemAreaSize > 0)
	{
		int write = SystemAreaSize < 16 ? SystemAreaSize : 16;

		if(!interface->writeSectors(lba+done, write, buffer))
		{
			free(buffer);
			ShowError(tr("Cannot write to the drive."));
			return -1;
		}
		SystemAreaSize -= write;
		done += write;
	}

	for (int i = 0; i < 2; i++)
	{
		sec_t SectorStart = (i == 0) ? lba : lba+6; //BackupBootSect

		if (!interface->writeSectors(SectorStart, 1, FAT32BootSect))
		{
			free(buffer);
			ShowError(tr("Cannot write to the drive."));
			return -1;
		}
		if (!interface->writeSectors(SectorStart+1, 1, FAT32FsInfo))
		{
			free(buffer);
			ShowError(tr("Cannot write to the drive."));
			return -1;
		}
	}

	memcpy(buffer, FirstSectOfFat, sizeof(FirstSectOfFat));

	// Write the first fat sector in the right places
	for (int i = 0; i < NumFATs; i++)
	{
		sec_t SectorStart = lba + ReservedSectCount + (i * FatSize);

		if (!interface->writeSectors(SectorStart, 1, buffer))
		{
			free(buffer);
			ShowError(tr("Cannot write to the drive."));
			return -1;
		}
	}

	free(buffer);

	return 1;
}

int PartitionFormatter::WriteMBR_FAT32(const DISC_INTERFACE *interface, sec_t part_lba)
{
	if(!interface)
		return -1;

	//Let's write the new MBR
	MASTER_BOOT_RECORD *mbr = (MASTER_BOOT_RECORD *) malloc(MAX_SECTOR_SIZE);
	if(!mbr)
		return -1;

	int i;

	if (!interface->readSectors(0, 1, mbr))
	{
		free(mbr);
		ShowError(tr("Cannot read from the drive."));
		return -1;
	}

	for(i = 0; i < 4; i++)
		if(le32(mbr->partitions[i].lba_start) == part_lba)
			break;

	if(i == 3)
	{
		free(mbr);
		ShowError(tr("Can't find the right partition."));
		return -1;
	}

	mbr->partitions[i].chs_start[0] = mbr->partitions[i].chs_end[0] = 0xFE;
	mbr->partitions[i].chs_start[1] = mbr->partitions[i].chs_end[1] = 0xFF;
	mbr->partitions[i].chs_start[2] = mbr->partitions[i].chs_end[2] = 0xFF;
	mbr->partitions[i].type = 0x0c;

	if (!interface->writeSectors(0, 1, mbr))
	{
		free(mbr);
		ShowError(tr("Failed to write MBR."));
		return -1;
	}
	free(mbr);

	return 1;
}

int PartitionFormatter::WriteEBR_FAT32(const DISC_INTERFACE *interface, sec_t erb_lba, sec_t part_lba UNUSED)
{
	if(!interface)
		return -1;

	//Let's write the new MBR
	EXTENDED_BOOT_RECORD *ebr = (EXTENDED_BOOT_RECORD *) malloc(MAX_SECTOR_SIZE);
	if(!ebr)
		return -1;

	if (!interface->readSectors(erb_lba, 1, ebr))
	{
		free(ebr);
		ShowError(tr("Cannot read from the drive."));
		return -1;
	}

	ebr->partition.chs_start[0] = ebr->partition.chs_end[0] = 0xFE;
	ebr->partition.chs_start[1] = ebr->partition.chs_end[1] = 0xFF;
	ebr->partition.chs_start[2] = ebr->partition.chs_end[2] = 0xFF;
	ebr->partition.type = 0x0c;

	if (!interface->writeSectors(erb_lba, 1, ebr))
	{
		free(ebr);
		ShowError(tr("Failed to write EBR."));
		return -1;
	}

	free(ebr);

	return 1;
}

int PartitionFormatter::SetActive(const DISC_INTERFACE *interface, int partition_number)
{
	if(!interface)
		return -1;

	//Let's write the new MBR
	MASTER_BOOT_RECORD *mbr = (MASTER_BOOT_RECORD *) malloc(MAX_SECTOR_SIZE);
	if(!mbr)
		return -1;

	int i;

	if (!interface->readSectors(0, 1, mbr))
	{
		free(mbr);
		ShowError(tr("Cannot read from the drive."));
		return -1;
	}

	for(i = 0; i < 4; i++)
	{
		if(mbr->partitions[i].status == PARTITION_BOOTABLE && i != partition_number)
			mbr->partitions[i].status = PARTITION_NONBOOTABLE;

		if(i == partition_number)
			mbr->partitions[i].status = PARTITION_BOOTABLE;
	}

	if (!interface->writeSectors(0, 1, mbr))
	{
		free(mbr);
		ShowError(tr("Failed to write MBR."));
		return -1;
	}
	free(mbr);

	return 1;
}
