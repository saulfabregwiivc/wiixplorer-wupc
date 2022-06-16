/****************************************************************************
 * Copyright (C) 2009-2011 Dimok
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
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <string>
#include <stdio.h>
#include <gctypes.h>
#include "SettingsEnums.h"
#include "Controls/AppControls.hpp"
#include "FileStartUp/FileExtensions.h"

class CSettings
{
	public:
		//!Constructor
		CSettings();
		//!Destructor
		~CSettings();
		//!Set Default Settings
		void SetDefault();
		//!Load Settings
		bool Load();
		//!Find the config file in the default paths
		bool FindConfig();
		//!Save Settings
		bool Save();
		//!Reset Settings
		bool Reset();
		//!Set a Setting
		//!\param name Settingname
		//!\param value Settingvalue
		bool SetSetting(char * name, char * value);
		//!Load a languagefile
		//!\param language
		bool LoadLanguage(const char *path, int language = -1);
		//!Set the default ColorSettings
		void DefaultColors();
		//!This is set only once on start up to remember the entrace IOS
		u8 EntraceIOS;

		/**************** Color Variables ************************/

		u32 BackgroundUL;
		u32 BackgroundUR;
		u32 BackgroundBR;
		u32 BackgroundBL;
		u32 ProgressUL;
		u32 ProgressUR;
		u32 ProgressBR;
		u32 ProgressBL;
		u32 ProgressEmptyUL;
		u32 ProgressEmptyUR;
		u32 ProgressEmptyBR;
		u32 ProgressEmptyBL;

		/**************** Settings Variables ************************/


		//!BootDevice determines from which devices to load the config
		char	BootDevice[10];
		//!Boot IOS
		short  	BootIOS;
		//!Current selected SMB user
		short  	CurrentSMBUser;
		//!Current selected FTP user
		short  	CurrentFTPUser;
		//!Current selected NFS user
		short  	CurrentNFSUser;
		//!Music Volume.
		short  	MusicVolume;
		//!BGM Loop Mode
		short  	BGMLoopMode;
		//!AutoConnect Network on StartUp
		short  	AutoConnect;
		//!Update the meta.xml
		short  	UpdateMetaxml;
		//!Update the icon.png
		short  	UpdateIconpng;
		//!Clock mode
		short  	ClockMode;
		//!Scrolling speed
		short  	ScrollSpeed;
		//!Browser mode
		short  	BrowserMode;
		//!SoundblockCount
		short   SoundblockCount;
		//!SoundblockSize
		int	 SoundblockSize;
		//!Screenshot image format
		short  	ScreenshotFormat;
		//!Load music to memory or play it from device
		short  	LoadMusicToMem;
		//!Slideshow time between images
		short   SlideshowDelay;
		//!Fade speed between images
		short   ImageFadeSpeed;
		//!Keyboard delete delay
		short   KeyboardDeleteDelay;
		//!Delete temporary path on exit
		short   DeleteTempPath;
		//!CopyThread priority
		short   CopyThreadPrio;
		//!CopyThread background priority
		short   CopyThreadBackPrio;
		//!Rumble of WiiMotes
		short   Rumble;
		//!Should system files be hidden or not
		short   HideSystemFiles;
		//!Should parition formatter be shown
		short   ShowFormatter;
		//!Zip compression level
		short   CompressionLevel;
		//!USB Port to mount
		short   USBPort;
		//!Allow ISFS mount
		short   MountISFS;
		//!Allow ISFS write access
		short   ISFSWriteAccess;
		//!Tooltip delay
		short   TooltipDelay;
		//!Allow resample sound to 48 kHz
		short   ResampleTo48kHz;
		//!Overrider priiloader on exit
		short   OverridePriiloader;
		//!PDF processing zoom
		float   PDFLoadZoom;
		//!WiiMote pointer speed factor
		float   PointerSpeed;
		//!Path to MPlayerPath
		char	MPlayerPath[150];
		//!Path to WiiMC
		char	WiiMCPath[150];
		//!Languagefile path
		char	MusicPath[150];
		//!Languagefile path
		char	LanguagePath[150];
		//!Path to the customfont file.
		char	CustomFontPath[150];
		//!Temporary path for file extraction
		char	TempPath[150];
		//!Path to where the app is located
		//!Default is sd:/apps/WiiExplorer/
		char	UpdatePath[150];
		//!Path to where the homebrew apps are
		char	HomebrewAppsPath[150];
		//!Path to where the screenshots are written
		char	ScreenshotPath[150];
		//!Path to where the url list in xml form is
		char	LinkListPath[150];
		//!Last used path to which the app returns to
		std::string LastUsedPath;
		//!SMB users informations
		SMBData SMBUser[MAXSMBUSERS];
		//!FTP users informations
		FTPData FTPUser[MAXFTPUSERS];
		//!FTP server informations
		FTPServerData FTPServer;
		//!NFS server informations
		NFSData NFSUser[MAXNFSUSERS];
		//!Internal WiiXplorer FileExtensions
		FilesExtensions FileExtensions;
		//!The controls of the App
		AppControls Controls;
		//!Path to the configuration file
		//!only for internal use
		char	ConfigPath[150];
	protected:

		void ParseLine(char *line);
		void TrimLine(char *dest, char *src, int size);
};

extern CSettings Settings;

#define WiiControls			Settings.Controls.WiiControls
#define ClassicControls		Settings.Controls.ClassicControls
#define GCControls			Settings.Controls.GCControls

#endif
