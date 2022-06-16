/****************************************************************************
 * Copyright (C) 2011
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
 ***************************************************************************/
#include <malloc.h>
#include <unistd.h>
#include <string.h>
#include <ogc/mutex.h>
#include <ogc/system.h>
#include <ogc/machine/processor.h>
#include <sdcard/wiisd_io.h>
#include <sdcard/gcsd.h>
#include "network/SMB.h"
#include "network/FTPClient.h"
#include "network/NFS.h"
#include "mload/usb2storage.h"
#include "System/IosLoader.h"
#include "System/isfs.h"
#include "DeviceHandler.hpp"

#include "Settings.h"
#include "DiskOperations/fst.h"
#include "DiskOperations/gcfst.h"
#include "DiskOperations/iso.h"
#include "DiskOperations/di2.h"

#define USB_REG_BASE				0x0D040000
#define USB_REG_OP_BASE				(USB_REG_BASE + (read32(USB_REG_BASE) & 0xff))
#define USB_REG_PORTSC_OFF(port)	(0x44 + 4 * port)
#define USB_PORT_CONNECTED(port)	(read32(USB_REG_OP_BASE + USB_REG_PORTSC_OFF(port)) & 0x0F)

DeviceHandler * DeviceHandler::instance = NULL;

DeviceHandler::DeviceHandler()
	: sd(0), gca(0), gcb(0),
	  usb0(0), usb1(0)
{
}

DeviceHandler::~DeviceHandler()
{
	UnMountAll();
}

bool DeviceHandler::MountAll()
{
	bool result = false;

	for(u32 i = SD; i <= DVD; i++)
	{
		if(Mount(i))
			result = true;
	}

	return result;
}

void DeviceHandler::UnMountAll()
{
	for(u32 i = SD; i <= DVD; i++)
		UnMount(i);

	if(sd)
		delete sd;
	if(gca)
		delete gca;
	if(gcb)
		delete gca;
	if(usb0)
		delete usb0;
	if(usb1)
		delete usb1;

	sd = NULL;
	gca = NULL;
	gcb = NULL;
	usb0 = NULL;
	usb1 = NULL;
	USBStorage2_Deinit();
}

bool DeviceHandler::Mount(int dev)
{
	if(dev == SD)
		return MountSD();

	else if(dev == GCSDA)
		return MountGCA();

	else if(dev == GCSDB)
		return MountGCB();

	else if(dev >= USB1 && dev <= USB8)
		return MountUSB(dev-USB1);

	else if(dev >= SMB1 && dev <= SMB10)
		return ConnectSMBShare(dev-SMB1);

	else if(dev >= FTP1 && dev <= FTP10)
		return ConnectFTP(dev-FTP1);

	else if(dev >= NFS1 && dev <= NFS10)
		return ConnectNFS(dev-NFS1);

	else if(dev == NAND)
		return MountNAND();

	else if(dev == DVD)
		return MountDVD();

	return false;
}

bool DeviceHandler::IsInserted(int dev)
{
	if(dev == SD)
		return SD_Inserted() && sd->IsMounted(0);

	else if(dev == GCSDA)
		return GCA_Inserted() && gca->IsMounted(0);

	else if(dev == GCSDB)
		return GCB_Inserted() && gcb->IsMounted(0);

	else if(dev >= USB1 && dev <= USB8)
		return GetUSBFromDev(dev) && GetUSBFromDev(dev)->IsMounted(PartToPortPart(dev-USB1));

	else if(dev >= SMB1 && dev <= SMB10)
		return IsSMB_Mounted(dev-SMB1);

	else if(dev >= FTP1 && dev <= FTP10)
		return IsFTPConnected(dev-FTP1); //later

	else if(dev >= NFS1 && dev <= NFS10)
		return IsNFS_Mounted(dev-NFS1);

	else if(dev == NAND)
		return ISFS_IsMounted();

	else if(dev == DVD)
		return DVD_Inserted();

	return false;
}

void DeviceHandler::UnMount(int dev)
{
	if(dev == SD)
		UnMountSD();

	else if(dev == GCSDA)
		UnMountGCA();

	else if(dev == GCSDB)
		UnMountGCB();

	else if(dev >= USB1 && dev <= USB8)
		UnMountUSB(dev-USB1);

	else if(dev >= SMB1 && dev <= SMB10)
		CloseSMBShare(dev-SMB1);

	else if(dev >= FTP1 && dev <= FTP10)
		CloseFTP(dev-FTP1);

	else if(dev >= NFS1 && dev <= NFS10)
		CloseNFS(dev-FTP1);

	else if(dev == NAND)
		UnMountNAND();

	else if(dev == DVD)
		UnMountDVD();
}

bool DeviceHandler::MountSD()
{
	if(!sd)
		sd = new PartitionHandle(&__io_wiisd);

	if(sd->GetPartitionCount() < 1)
	{
		delete sd;
		sd = NULL;
		return false;
	}

	//! Mount only one SD Partition
	return sd->Mount(0, DeviceName[SD]);
}

bool DeviceHandler::MountGCA()
{
	if(!gca)
		gca = new PartitionHandle(&__io_gcsda);

	if(gca->GetPartitionCount() < 1)
	{
		delete gca;
		gca = NULL;
		return false;
	}

	//! Mount only one Partition
	return gca->Mount(0, DeviceName[GCSDA]);
}

bool DeviceHandler::MountGCB()
{
	if(!gcb)
		gcb = new PartitionHandle(&__io_gcsdb);

	if(gcb->GetPartitionCount() < 1)
	{
		delete gcb;
		gcb = NULL;
		return false;
	}

	//! Mount only one Partition
	return gcb->Mount(0, DeviceName[GCSDB]);
}

const DISC_INTERFACE *DeviceHandler::GetUSB0Interface(void)
{
	return (IOS_GetVersion() > 200) ? &__io_usbstorage2_port0 : &__io_usbstorage;
}

const DISC_INTERFACE *DeviceHandler::GetUSB1Interface(void)
{
	if(IOS_GetVersion() < 200)
		return NULL;

	return &__io_usbstorage2_port1;
}

bool DeviceHandler::USBSpinUp(int iTimeout)
{
	bool started0 = false;
	bool started1 = false;
	time_t tStart = time(0);

	//! if we have AHBPROT check if some device is connected to the ports before trying to mount them
	//! otherwise just try and wait till it times out
	if(CheckAHBPROT() && !USB_PORT_CONNECTED(0) && ((Settings.USBPort == 0) || !USB_PORT_CONNECTED(1)))
		return false;

	const DISC_INTERFACE * handle0 = DeviceHandler::GetUSB0Interface();
	const DISC_INTERFACE * handle1 = NULL;

	if(Settings.USBPort == 1 && USB_PORT_CONNECTED(1))
		handle1 = DeviceHandler::GetUSB1Interface();

	do
	{
		if(handle0)
			started0 = (handle0->startup() && handle0->isInserted());

		if(handle1)
			started1 = (handle1->startup() && handle1->isInserted());

		if(   (!handle0 || started0)
		   && (!handle1 || started1)) {
			break;
		}

		usleep(50000);
	}
	while(time(0) - tStart < iTimeout);

	return (started0 || started1);
}

bool DeviceHandler::MountUSB(int pos)
{
	if(!usb0)
		usb0 = new PartitionHandle(GetUSB0Interface());
	if(!usb1 && Settings.USBPort == 1 && IOS_GetVersion() > 200)
		usb1 = new PartitionHandle(GetUSB1Interface());

	int partCount = 0;
	if(usb0)
		partCount += usb0->GetPartitionCount();
	if(usb1)
		partCount += usb1->GetPartitionCount();

	if(pos >= partCount)
		return false;

	return GetUSBFromDev(USB1+pos)->Mount(PartToPortPart(pos), DeviceName[USB1+pos]);
}

bool DeviceHandler::MountAllUSB()
{
	if(!USBSpinUp(10))
		return false;

	if(!usb0)
		usb0 = new PartitionHandle(GetUSB0Interface());
	if(!usb1 && Settings.USBPort == 1 && IOS_GetVersion() > 200)
		usb1 = new PartitionHandle(GetUSB1Interface());

	bool result = false;
	int partCount = 0;
	if(usb0)
		partCount += usb0->GetPartitionCount();
	if(usb1)
		partCount += usb1->GetPartitionCount();

	for(int i = 0; i < partCount; i++)
	{
		if(MountUSB(i))
			result = true;
	}

	return result;
}

void DeviceHandler::UnMountUSB(int pos)
{
	if(GetUSBFromDev(USB1+pos))
		GetUSBFromDev(USB1+pos)->UnMount(PartToPortPart(pos));
}

void DeviceHandler::UnMountAllUSB()
{
	int partCount = 0;
	if(usb0)
		partCount += usb0->GetPartitionCount();
	if(usb1)
		partCount += usb1->GetPartitionCount();

	for(int i = 0; i < partCount; i++)
		UnMountUSB(i);

	delete usb0;
	delete usb1;
	usb0 = NULL;
	usb1 = NULL;
	USBStorage2_Deinit();
}

bool DeviceHandler::MountNAND()
{
	if(!Settings.MountISFS)
		return false;

	return ISFS_Mount(!Settings.ISFSWriteAccess);
}

void DeviceHandler::UnMountNAND()
{
	ISFS_Unmount();
}

bool DeviceHandler::MountDVDFS()
{
	bool devicemounted = ISO9660_Mount();

	if(!devicemounted)
		devicemounted = FST_Mount();
	if(!devicemounted)
		devicemounted = GCFST_Mount();

	return devicemounted;
}

bool DeviceHandler::MountDVD()
{
	if(!DVD_Inserted())
		return false;

	char read_buffer[2048];
	if(DI2_ReadDVD(read_buffer, 1, 0) == 0)
		return true;

	UnMountDVD();
	DI2_Mount();

	time_t timer1, timer2;
	timer1 = time(0);

	while(DI2_GetStatus() & DVD_INIT)
	{
		timer2 = time(0);
		if(timer2-timer1 > 15)
			return false;

		usleep(5000);
	}

	return MountDVDFS();
}

bool DeviceHandler::DVD_Inserted()
{
	uint32_t cover = 0;
	DI2_GetCoverRegister(&cover);

	if(cover & DVD_COVER_DISC_INSERTED)
		return true;

	return false;
}

void DeviceHandler::UnMountDVD()
{
	FST_Unmount();
	GCFST_Unmount();
	ISO9660_Unmount();
}

int DeviceHandler::PathToDriveType(const char * path)
{
	if(!path)
		return -1;

	for(int i = SD; i <= DVD; i++)
	{
		if(strncmp(path, DeviceName[i], strlen(DeviceName[i])) == 0)
			return i;
	}

	return -1;
}

const char * DeviceHandler::GetFSName(int dev)
{
	if(dev == SD && DeviceHandler::instance->sd)
	{
		return DeviceHandler::instance->sd->GetFSName(0);
	}
	else if(dev == GCSDA && DeviceHandler::instance->gca)
	{
		return DeviceHandler::instance->gca->GetFSName(0);
	}
	else if(dev == GCSDB && DeviceHandler::instance->gcb)
	{
		return DeviceHandler::instance->gcb->GetFSName(0);
	}
	else if(dev >= USB1 && dev <= USB8 && instance->GetUSBFromDev(dev))
	{
		return instance->GetUSBFromDev(dev)->GetFSName(instance->PartToPortPart(dev-USB1));
	}

	return NULL;
}

PartitionHandle * DeviceHandler::GetUSBFromDev(int dev)
{
	int usbPart = dev-USB1;

	if(!usb0 || usbPart >= usb0->GetPartitionCount())
		return usb1;
	else
		return usb0;
}

 int DeviceHandler::PartToPortPart(int part)
 {
	if(!usb0 || part >= usb0->GetPartitionCount())
		return part-usb0->GetPartitionCount();
	else
		return part;
 }

 int DeviceHandler::PartToPort(int part)
 {
	if(!usb0 || part >= usb0->GetPartitionCount())
		return 1;
	else
		return 0;
 }
