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
#ifndef GUI_SOUND_H_
#define GUI_SOUND_H_

#include <gccore.h>

//!Sound conversion and playback. A wrapper for other sound libraries - ASND, libmad, ltremor, etc
class GuiSound
{
	public:
		//!Constructor
		//!\param sound Pointer to the sound data
		//!\param filesize Length of sound data
		GuiSound(const char * filepath);
		GuiSound(const u8 * sound, int filesize, bool allocated = false, int voice = -1);
		//!Destructor
		virtual ~GuiSound();
		//!Load a file and replace the old one
		bool Load(const char * filepath);
		//!Load a file and replace the old one
		bool Load(const u8 * sound, int filesize, bool allocated = true);
		//!For quick playback of the internal soundeffects
		bool LoadSoundEffect(const u8 * snd, s32 len);
		//!Start sound playback
		void Play();
		//!Stop sound playback
		void Stop();
		//!Pause sound playback
		void Pause();
		//!Resume sound playback
		void Resume();
		//!Checks if the sound is currently playing
		//!\return true if sound is playing, false otherwise
		bool IsPlaying();
		//!Rewind the music
		void Rewind();
		//!Set sound volume
		//!\param v Sound volume (0-100)
		void SetVolume(int v);
		//!\param l Loop (true to loop)
		void SetLoop(u8 l);
		//!Special sound case for sound.bin
		void UncompressSoundbin(const u8 * snd, int len, bool isallocated);
	protected:
		//!Stops sound and frees all memory/closes files
		void FreeMemory();
		u8 * sound; //!< Pointer to the sound data
		int length; //!< Length of sound data
		s32 voice; //!< Currently assigned ASND voice channel
		int volume; //!< Sound volume (0-100)
		u8 loop; //!< Loop sound playback
		u32 SoundEffectLength; //!< Check if it is an app soundeffect for faster playback
		bool allocated; //!< Is the file allocated or not
};

#endif
