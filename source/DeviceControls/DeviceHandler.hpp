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
#ifndef DEVICE_HANDLER_HPP_
#define DEVICE_HANDLER_HPP_

#include "PartitionHandle.h"

enum
{
	SD = 0,
	GCSDA,
	GCSDB,
	USB1,
	USB2,
	USB3,
	USB4,
	USB5,
	USB6,
	USB7,
	USB8,
	SMB1,
	SMB2,
	SMB3,
	SMB4,
	SMB5,
	SMB6,
	SMB7,
	SMB8,
	SMB9,
	SMB10,
	FTP1,
	FTP2,
	FTP3,
	FTP4,
	FTP5,
	FTP6,
	FTP7,
	FTP8,
	FTP9,
	FTP10,
	NAND,
	DVD,
	NFS1,
	NFS2,
	NFS3,
	NFS4,
	NFS5,
	NFS6,
	NFS7,
	NFS8,
	NFS9,
	NFS10,
	MAXDEVICES
};

const char DeviceName[MAXDEVICES][6] =
{
	"sd",
	"gca",
	"gcb",
	"usb1",
	"usb2",
	"usb3",
	"usb4",
	"usb5",
	"usb6",
	"usb7",
	"usb8",
	"smb1",
	"smb2",
	"smb3",
	"smb4",
	"smb5",
	"smb6",
	"smb7",
	"smb8",
	"smb9",
	"smb10",
	"ftp1",
	"ftp2",
	"ftp3",
	"ftp4",
	"ftp5",
	"ftp6",
	"ftp7",
	"ftp8",
	"ftp9",
	"ftp10",
	"nand",
	"dvd",
	"nfs1",
	"nfs2",
	"nfs3",
	"nfs4",
	"nfs5",
	"nfs6",
	"nfs7",
	"nfs8",
	"nfs9",
	"nfs10",
};

class DeviceHandler
{
	public:
		static DeviceHandler * Instance() { if(!instance) instance = new DeviceHandler; return instance; }
		static void DestroyInstance() { delete instance; instance = NULL; }

		bool MountAll();
		void UnMountAll();
		bool Mount(int dev);
		bool IsInserted(int dev);
		void UnMount(int dev);

		//! Individual Mounts/UnMounts...
		bool MountSD();
		bool MountGCA();
		bool MountGCB();
		bool MountAllUSB();
		bool MountUSB(int part);
		bool MountNAND();
		bool MountDVD();
		bool MountDVDFS();
		bool SD_Inserted() { if(sd) return sd->IsInserted(); return false; };
		bool GCA_Inserted() { if(gca) return gca->IsInserted(); return false; };
		bool GCB_Inserted() { if(gcb) return gcb->IsInserted(); return false; };
		bool USB0_Inserted() { if(usb0) return usb0->IsInserted(); return false; };
		bool USB1_Inserted() { if(usb1) return usb1->IsInserted(); return false; };
		bool DVD_Inserted();
		void UnMountSD() { if(sd) delete sd; sd = NULL; };
		void UnMountGCA() { if(gca) delete gca; gca = NULL; };
		void UnMountGCB() { if(gcb) delete gcb; gcb = NULL; };
		void UnMountUSB(int pos);
		void UnMountAllUSB();
		void UnMountNAND();
		void UnMountDVD();
		PartitionHandle * GetSDHandle() { return sd; };
		PartitionHandle * GetGCAHandle() { return gca; };
		PartitionHandle * GetGCBHandle() { return gcb; };
		PartitionHandle * GetUSB0Handle() { return usb0; };
		PartitionHandle * GetUSB1Handle() { return usb1; };
		PartitionHandle * GetUSBFromDev(int dev);
		static const DISC_INTERFACE *GetUSB0Interface(void);
		static const DISC_INTERFACE *GetUSB1Interface(void);

		static bool USBSpinUp(int iTimeout);

		int PartToPortPart(int part);
		int PartToPort(int part);
		static int PathToDriveType(const char * path);
		static const char * GetFSName(int dev);
		static const char * PathToFSName(const char * path) { return GetFSName(PathToDriveType(path)); };
	private:
		DeviceHandler();
		~DeviceHandler();

		static DeviceHandler *instance;

		PartitionHandle * sd;
		PartitionHandle * gca;
		PartitionHandle * gcb;
		PartitionHandle * usb0;
		PartitionHandle * usb1;
};

#endif
