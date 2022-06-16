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
#include "SoundSettingsMenu.h"
#include "Prompts/PromptWindows.h"
#include "SoundOperations/MusicPlayer.h"
#include "Settings.h"

SoundSettingsMenu::SoundSettingsMenu(GuiFrame *r)
	: SettingsMenu(tr("Sound Settings"), r)
{
	SetupOptions();
}

SoundSettingsMenu::~SoundSettingsMenu()
{
}

void SoundSettingsMenu::SetupOptions()
{
	int i = 0;

	options.SetName(i++, tr("Music Volume"));
	options.SetName(i++, tr("Music Loop Mode"));
	options.SetName(i++, tr("Resample to 48 kHz"));
	options.SetName(i++, tr("Load Music to Memory"));
	options.SetName(i++, tr("Soundblocks"));
	options.SetName(i++, tr("Soundblock Size"));

	SetOptionValues();
}

void SoundSettingsMenu::SetOptionValues()
{
	int i = 0;

	if(Settings.MusicVolume > 0)
		options.SetValue(i++, "%i", Settings.MusicVolume);
	else
		options.SetValue(i++, tr("OFF"));

	if (Settings.BGMLoopMode == LOOP) options.SetValue(i++,tr("Loop"));
	else if (Settings.BGMLoopMode == RANDOM_MUSIC) options.SetValue(i++,tr("Random"));
	else if (Settings.BGMLoopMode == PLAYLIST_LOOP) options.SetValue(i++,tr("Loop Playlist"));
	else options.SetValue(i++,tr("Play Once"));

	options.SetValue(i++, "%s", Settings.ResampleTo48kHz ? tr("ON") : tr("OFF"));

	if(Settings.LoadMusicToMem == 1) options.SetValue(i++, tr("ON"));
	else options.SetValue(i++, tr("OFF"));

	if(Settings.LoadMusicToMem == 1) options.SetValue(i++, tr("Memory Buffer"));
	else options.SetValue(i++, "%i (%0.1f KB)", Settings.SoundblockCount, Settings.SoundblockCount*Settings.SoundblockSize/1024.0f);

	if(Settings.LoadMusicToMem == 1) options.SetValue(i++, tr("Memory Buffer"));
	else options.SetValue(i++, "%i Bytes", Settings.SoundblockSize);

}

void SoundSettingsMenu::OnOptionClick(GuiOptionBrowser *sender UNUSED, int option)
{
	char entered[300];

	switch (option)
	{
		case 0:
			snprintf(entered, sizeof(entered), "%i", Settings.MusicVolume);
			if(OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.MusicVolume = LIMIT(atoi(entered), 0, 100);
			}
			MusicPlayer::Instance()->SetVolume(Settings.MusicVolume);
			break;
		case 1:
			Settings.BGMLoopMode++;
			if(Settings.BGMLoopMode >= MAX_LOOP_MODES)
				Settings.BGMLoopMode = 0;
			MusicPlayer::Instance()->SetLoop(Settings.BGMLoopMode);
			break;
		case 2:
			MusicPlayer::Instance()->Stop();
			Settings.ResampleTo48kHz = (Settings.ResampleTo48kHz + 1) % 2;
			MusicPlayer::Instance()->Play();
			break;
		case 3:
			Settings.LoadMusicToMem = (Settings.LoadMusicToMem+1) % 2;
			break;
		case 4:
			snprintf(entered, sizeof(entered), "%i", Settings.SoundblockCount);
			if(Settings.LoadMusicToMem != 1 && OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.SoundblockCount = atoi(entered);
				if(Settings.SoundblockCount < 3)
					Settings.SoundblockCount = 3;
				WindowPrompt(tr("Warning:"), tr("The effect will take with next music load. It might break music playback."), tr("OK"));
				if(Settings.SoundblockSize*Settings.SoundblockCount > 512*1024)
					WindowPrompt(tr("WARNING:"), tr("The buffer size is really high. If the app doesn't start after this delete your config files."), tr("OK"));
			}
			break;
		case 5:
			snprintf(entered, sizeof(entered), "%i", Settings.SoundblockSize);
			if(Settings.LoadMusicToMem != 1 && OnScreenKeyboard(entered, sizeof(entered)))
			{
				Settings.SoundblockSize = LIMIT(atoi(entered), 0x1000, 0xFFFF) & ~0x03;
				WindowPrompt(tr("Warning:"), tr("The effect will take with next music load. It might break music playback."), tr("OK"));
				if(Settings.SoundblockSize*Settings.SoundblockCount > 512*1024)
					WindowPrompt(tr("WARNING:"), tr("The buffer size is really high. If the app doesn't start after this delete your config files."), tr("OK"));
			}
			break;
	}

	SetOptionValues();
}
