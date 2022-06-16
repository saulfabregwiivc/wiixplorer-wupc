#ifndef _IOSLOADER_H_
#define _IOSLOADER_H_

#include <gccore.h>
#include <ogc/machine/processor.h>

#define CheckAHBPROT()	(read32(0x0D800064) == 0xFFFFFFFF)

typedef struct _iosinfo_t
{
	u32 magicword;			  //0x1ee7c105
	u32 magicversion;		   // 1
	u32 version;				// Example: 5
	u32 baseios;				// Example: 56
	char name[0x10];			// Example: d2x
	char versionstring[0x10];   // Example: beta2
} __attribute__((packed)) iosinfo_t;

class IosLoader
{
	public:
		static s32 ReloadIos(s32 ios) { return IOS_ReloadIOS(ios); }
		static s32 ReloadAppIos(s32 ios);
		static s32 ReloadIosKeepingRights(s32 ios);
		static bool IsHermesIOS(s32 ios = IOS_GetVersion());
		static bool IsWaninkokoIOS(s32 ios = IOS_GetVersion());
		static bool IsD2X(s32 ios = IOS_GetVersion());
		static iosinfo_t *GetIOSInfo(s32 ios);
	private:
		static int LoadFileFromNand(const char *filepath, u8 **outbuffer, u32 *outfilesize);
};

#endif
