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
#include "gui_sound.h"
#include "SoundOperations/MusicPlayer.h"
#include "SoundOperations/SoundHandler.hpp"
#include "SoundOperations/WavDecoder.hpp"
#include "FileOperations/fileops.h"
#include "Tools/uncompress.h"

#define MAX_SND_VOICES	  16

static bool VoiceUsed[MAX_SND_VOICES] =
{
	true, false, false, false, false, false,
	false, false, false, false, false, false,
	false, false, false, false
};

static inline int GetFirstUnusedVoice()
{
	for(int i = 1; i < MAX_SND_VOICES; i++)
	{
		if(VoiceUsed[i] == false)
			return i;
	}

	return -1;
}

extern "C" void SoundCallback(s32 voice)
{
	SoundDecoder * decoder = SoundHandler::Instance()->Decoder(voice);
	if(!decoder)
		return;

	if(decoder->IsBufferReady())
	{
		if(ASND_AddVoice(voice, decoder->GetBuffer(), decoder->GetBufferSize()) == SND_OK)
		{
			decoder->LoadNext();
			SoundHandler::Instance()->ThreadSignal();
		}
	}
	else if(decoder->IsEOF())
	{
		ASND_StopVoice(voice);
		if(voice == 0)
			MusicPlayer::Instance()->SetPlaybackFinished(true); //see if next music must be played
	}
	else
	{
		SoundHandler::Instance()->ThreadSignal();
	}
}

GuiSound::GuiSound(const char * filepath)
{
	sound = NULL;
	length = 0;
	voice = GetFirstUnusedVoice();
	if(voice > 0)
		VoiceUsed[voice] = true;

	volume = 255;
	SoundEffectLength = 0;
	loop = false;
	allocated = false;
	Load(filepath);
}

GuiSound::GuiSound(const u8 * snd, s32 len, bool isallocated, int v)
{
	sound = NULL;
	length = 0;
	if(v < 0)
		voice = GetFirstUnusedVoice();
	else
		voice = v;

	if(voice > 0)
		VoiceUsed[voice] = true;

	volume = 255;
	SoundEffectLength = 0;
	loop = false;
	allocated = false;
	Load(snd, len, isallocated);
}

GuiSound::~GuiSound()
{
	FreeMemory();
	if(voice > 0)
		VoiceUsed[voice] = false;
}

void GuiSound::FreeMemory()
{
	this->Stop();

	SoundHandler::Instance()->RemoveDecoder(voice);

	if(allocated && sound != NULL)
	{
		free(sound);
		sound = NULL;
		allocated = false;
	}

	SoundEffectLength = 0;
}

bool GuiSound::Load(const char * filepath)
{
	FreeMemory();

	if(!filepath)
		return false;

	u32 magic;
	FILE * f = fopen(filepath, "rb");
	if(!f)
		return false;

	fread(&magic, 1, 4, f);
	fclose(f);

	if(magic == 'IMD5')
	{
		u8 * snd = NULL;
		u32 filesize = 0;
		LoadFileToMem(filepath, &snd, &filesize);
		return Load(snd, filesize, true);
	}

	SoundHandler::Instance()->AddDecoder(voice, filepath);

	SoundDecoder * decoder = SoundHandler::Instance()->Decoder(voice);
	if(!decoder)
		return false;

	if(!decoder->IsBufferReady())
	{
		SoundHandler::Instance()->RemoveDecoder(voice);
		return false;
	}

	SetLoop(loop);

	return true;
}

bool GuiSound::Load(const u8 * snd, s32 len, bool isallocated)
{
	FreeMemory();

	if(!snd)
		return false;

	if(!isallocated && *((u32 *) snd) == 'RIFF')
	{
		return LoadSoundEffect(snd, len);
	}

	if(*((u32 *) snd) == 'IMD5')
	{
		UncompressSoundbin(snd, len, isallocated);
	}
	else
	{
		sound = (u8 *) snd;
		length = len;
		allocated = isallocated;
	}

	SoundHandler::Instance()->AddDecoder(voice, sound, length);

	SoundDecoder * decoder = SoundHandler::Instance()->Decoder(voice);
	if(!decoder)
		return false;

	if(!decoder->IsBufferReady())
	{
		SoundHandler::Instance()->RemoveDecoder(voice);
		return false;
	}

	SetLoop(loop);

	return true;
}

bool GuiSound::LoadSoundEffect(const u8 * snd, s32 len)
{
	WavDecoder decoder(snd, len);
	decoder.Rewind();

	u32 done = 0;
	sound = (u8 *) malloc(4096);
	memset(sound, 0, 4096);

	while(1)
	{
		u8 * tmpsnd = (u8 *) realloc(sound, done+4096);
		if(!tmpsnd)
		{
			free(sound);
			sound = NULL;
			return false;
		}

		sound = tmpsnd;

		int read = decoder.Read(sound+done, 4096, done);
		if(read <= 0)
			break;

		done += read;
	}

	sound = (u8 *) realloc(sound, done);
	SoundEffectLength = done;
	allocated = true;

	return true;
}

void GuiSound::Play()
{
	if(SoundEffectLength > 0)
	{
		ASND_StopVoice(voice);
		ASND_SetVoice(voice, VOICE_STEREO_16BIT, 32000, 0, sound, SoundEffectLength, volume, volume, NULL);
		return;
	}

	if(IsPlaying())
		return;

	if(voice < 0 || voice >= 16)
		return;

	SoundDecoder * decoder = SoundHandler::Instance()->Decoder(voice);
	if(!decoder)
		return;

	if(decoder->IsEOF())
	{
		ASND_StopVoice(voice);
		decoder->ClearBuffer();
		decoder->Rewind();
		decoder->Decode();
	}

	u8 * curbuffer = decoder->GetBuffer();
	int bufsize = decoder->GetBufferSize();
	decoder->LoadNext();
	SoundHandler::Instance()->ThreadSignal();

	ASND_SetVoice(voice, decoder->GetFormat(), decoder->GetSampleRate(), 0, curbuffer, bufsize, volume, volume, SoundCallback);
}

void GuiSound::Stop()
{
	if(voice < 0 || voice >= 16)
		return;

	ASND_StopVoice(voice);

	SoundDecoder * decoder = SoundHandler::Instance()->Decoder(voice);
	if(!decoder)
		return;

	decoder->ClearBuffer();
	Rewind();
	SoundHandler::Instance()->ThreadSignal();
}

void GuiSound::Pause()
{
	if(voice < 0 || voice >= 16)
		return;

	ASND_StopVoice(voice);
}

void GuiSound::Resume()
{
	Play();
}

bool GuiSound::IsPlaying()
{
	if(voice < 0 || voice >= 16)
		return false;

	int result = ASND_StatusVoice(voice);

	if(result == SND_WORKING || result == SND_WAITING)
		return true;

	return false;
}

void GuiSound::SetVolume(int vol)
{
	if(voice < 0 || voice >= 16)
		return;

	if(vol < 0)
		return;

	volume = LIMIT((255*vol)/100, 0, 255);
	ASND_ChangeVolumeVoice(voice, volume, volume);
}

void GuiSound::SetLoop(u8 l)
{
	loop = l;

	SoundDecoder * decoder = SoundHandler::Instance()->Decoder(voice);
	if(!decoder)
		return;

	decoder->SetLoop(l == 1);
}

void GuiSound::Rewind()
{
	SoundDecoder * decoder = SoundHandler::Instance()->Decoder(voice);
	if(!decoder)
		return;

	decoder->Rewind();
}

void GuiSound::UncompressSoundbin(const u8 * snd, int len, bool isallocated)
{
	const u8 * file = snd+32;
	if(*((u32 *) file) == 'LZ77')
	{
		u32 size = 0;
		sound = uncompressLZ77(file, len-32, &size);
		length = size;
	}
	else
	{
		length = len-32;
		sound = (u8 *) malloc(length);
		memcpy(sound, file, length);
	}

	if(isallocated)
		free((u8 *) snd);

	allocated = true;
}
