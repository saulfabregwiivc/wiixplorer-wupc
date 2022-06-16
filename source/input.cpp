/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * input.cpp
 * Wii/GameCube controller management
 ***************************************************************************/
#include "stdafx.h"
#include "Controls/Application.h"
#include "VideoOperations/video.h"
#include "ImageOperations/Screenshot.h"
#include "GUI/gui_trigger.h"
#include "Settings.h"
#include "input.h"

static int rumbleCount[4] = {0,0,0,0};
static int rumbleRequest[4] = {0,0,0,0};
GuiTrigger userInput[4];

/****************************************************************************
 * UpdatePads
 *
 * Scans pad and wpad
 ***************************************************************************/
void UpdatePads()
{
	WUPC_UpdateButtonStats();
	WPAD_ScanPads();
	PAD_ScanPads();

	for(int i=3; i >= 0; i--)
	{
		memcpy(&userInput[i].wpad, WPAD_Data(i), sizeof(WPADData));

		userInput[i].pad.btns_d = PAD_ButtonsDown(i);
		userInput[i].pad.btns_u = PAD_ButtonsUp(i);
		userInput[i].pad.btns_h = PAD_ButtonsHeld(i);
		userInput[i].pad.stickX = PAD_StickX(i);
		userInput[i].pad.stickY = PAD_StickY(i);
		userInput[i].pad.substickX = PAD_SubStickX(i);
		userInput[i].pad.substickY = PAD_SubStickY(i);
		userInput[i].pad.triggerL = PAD_TriggerL(i);
		userInput[i].pad.triggerR = PAD_TriggerR(i);

		userInput[i].wupc.btns_d = WUPC_ButtonsDown(i);
		userInput[i].wupc.btns_h = WUPC_ButtonsHeld(i);
		userInput[i].wupc.btns_u = WUPC_ButtonsUp(i);
		userInput[i].wupc.stickX = WUPC_lStickX(i);
		userInput[i].wupc.stickY = WUPC_lStickY(i);
		userInput[i].wupc.substickX = WUPC_rStickX(i);
		userInput[i].wupc.substickY = WUPC_rStickY(i);

		if(Settings.Rumble)
			DoRumble(i);

		if((userInput[i].wpad.btns_h & Settings.Controls.ScreenshotHoldButton) && (userInput[i].wpad.btns_d & Settings.Controls.ScreenshotClickButton))
		{
			Screenshot();
		}
	}
}

/****************************************************************************
 * Initialize Stuff
 ***************************************************************************/
extern "C" void __Wpad_PowerCallback(s32 chan UNUSED)
{
	Application::shutdownSystem();
}

/****************************************************************************
 * SetupPads
 *
 * Sets up userInput triggers for use
 ***************************************************************************/
void SetupPads()
{
	PAD_Init();
	WUPC_Init();
	WPAD_Init();

	WPAD_SetPowerButtonCallback(__Wpad_PowerCallback);

	// read wiimote accelerometer and IR data
	WPAD_SetDataFormat(WPAD_CHAN_ALL,WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_ALL, screenwidth, screenheight);

	for(int i=0; i < 4; i++)
		userInput[i].chan = i;
}

/****************************************************************************
 * Shutdown Pads
 ***************************************************************************/
void ShutdownPads()
{
	ShutoffRumble();
	WUPC_Shutdown();
	WPAD_Shutdown();
}

/****************************************************************************
 * Request for Rumble
 ***************************************************************************/
void RequestRumble(int chan)
{
	rumbleRequest[chan] = 1;
}

/****************************************************************************
 * DoRumble
 ***************************************************************************/
void DoRumble(int i)
{
	if(rumbleRequest[i] && rumbleCount[i] == 0)
	{
		WUPC_Rumble(i, 1);
		WPAD_Rumble(i, 1); // rumble on
		rumbleRequest[i] = 0;
		rumbleCount[i] = 8;
	}
	else
	{
		if(rumbleCount[i] > 0)
			--rumbleCount[i];

		if(rumbleCount[i] < 4)
			WPAD_Rumble(i, 0); // rumble off

		rumbleRequest[i] = 0;
	}
}

/****************************************************************************
 * ShutoffRumble
 ***************************************************************************/
void ShutoffRumble()
{
	for(int i=0;i<4;i++)
	{
		WUPC_Rumble(i, 0);
		WPAD_Rumble(i, 0);
		rumbleCount[i] = 0;
	}
}
