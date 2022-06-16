#ifndef __STDAFX_H_
#define __STDAFX_H_

#include <gccore.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>
#include <math.h>
#include <wiiuse/wpad.h>
#include <gd.h>
#include "Tools/gecko.h"
#include "Tools/wifi_gecko.h"
#include "Tools/tools.h"
#include "Language/gettext.h"

#ifdef __cplusplus
#include <string>
#include <vector>
#include "sigslot.h"
#include "Tools/StringTools.h"
#include "Settings.h"
#endif

#define UNUSED  __attribute__((unused))

extern u32 frameCount;
extern int screenwidth;
extern int screenheight;
extern Mtx GXmodelView2D;

#endif
