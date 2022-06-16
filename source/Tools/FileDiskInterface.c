/****************************************************************************
 * Copyright (C) 2013 Dimok
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
#include <gccore.h>
#include <stdio.h>
#include <string.h>

#define DEVICE_TYPE_FILEDISK (('F'<<24)|('D'<<16)|('I'<<8)|'F')
#define SECTOR_SIZE		512
#define SECTOR_COUNT	(DEVICE_SIZE >> 9)

// debug inteface with a virtual partition in a file
static const char *cpFileDiskPath = "nand:/virtual_partition";
static FILE *pFile = NULL;
static u32 DEVICE_SIZE = 0;

void filestorage_SetPath(const char *cpPath)
{
	if(cpPath)
		cpFileDiskPath = cpPath;
}

static bool __filestorage_Startup(void)
{
	if(pFile)
		return true;

	pFile = fopen(cpFileDiskPath, "rb+");
	if(pFile)
	{
		fseek(pFile, 0, SEEK_END);
		DEVICE_SIZE = ftell(pFile);
	}

	return (pFile != NULL);
}

static bool __filestorage_Shutdown(void)
{
	if (pFile != NULL)
	{
		fclose(pFile);
		pFile = NULL;
	}
	return true;
}

static bool __filestorage_ReadSectors(u32 sector, u32 numSectors, void *buffer)
{
	/* Device not opened */
	if ((pFile == NULL) || ((sector + numSectors) > SECTOR_COUNT))
		return false;

	u32 toRead = numSectors * SECTOR_SIZE;

	fseek(pFile, sector * SECTOR_SIZE, SEEK_SET);

	u32 readSize = fread(buffer, 1, toRead, pFile);

	return (toRead == readSize);
}

static bool __filestorage_WriteSectors(u32 sector, u32 numSectors, const void *buffer)
{
	/* Device not opened */
	if ((pFile == NULL) || ((sector + numSectors) > SECTOR_COUNT))
		return false;

	u32 toWrite = numSectors * SECTOR_SIZE;

	fseek(pFile, sector * SECTOR_SIZE, SEEK_SET);

	u32 writeSize = fwrite(buffer, 1, toWrite, pFile);

	return (writeSize == toWrite);
}

static bool __filestorage_IsInserted(void)
{
	return pFile != NULL;
}

static bool __filestorage_ClearStatus(void)
{
	return true;
}


const DISC_INTERFACE __io_filestorage = {
	DEVICE_TYPE_FILEDISK, FEATURE_MEDIUM_CANREAD | FEATURE_MEDIUM_CANWRITE | FEATURE_WII_USB,
	(FN_MEDIUM_STARTUP) &__filestorage_Startup,
	(FN_MEDIUM_ISINSERTED) &__filestorage_IsInserted,
	(FN_MEDIUM_READSECTORS) &__filestorage_ReadSectors,
	(FN_MEDIUM_WRITESECTORS) &__filestorage_WriteSectors,
	(FN_MEDIUM_CLEARSTATUS) &__filestorage_ClearStatus,
	(FN_MEDIUM_SHUTDOWN) &__filestorage_Shutdown
};
