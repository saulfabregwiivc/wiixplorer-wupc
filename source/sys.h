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
#ifndef _SYS_H_
#define _SYS_H_

#ifdef __cplusplus
extern "C"
{
#endif

s32 MagicPatches(s32 enable);
void __exception_setreload(int t);
u32 __di_check_ahbprot(void);

void wiilight(int enable);

bool RebootApp();
void ExitApp();

void Sys_Init(void);
void Sys_Reboot(void);
void Sys_Shutdown(void);
void Sys_ShutdownToIdle(void);
void Sys_ShutdownToStandby(void);
void Sys_LoadMenu(void);
void Sys_BackToLoader(void);
void Sys_LoadHBC(void);
bool IsFromHBC();
int GetIOS_Rev(u32 ios);
bool FindTitle(u64 titleid);

#ifdef __cplusplus
}
#endif

#endif
