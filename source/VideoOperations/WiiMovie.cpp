/****************************************************************************
 * Copyright (C) 2009 - 2013 Dimok
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
#include <asndlib.h>
#include <unistd.h>
#include "Controls/Application.h"
#include "WiiMovie.hpp"
#include "ImageOperations/TextureConverter.h"
#include "SoundOperations/MusicPlayer.h"
#include "VideoOperations/video.h"

static BufferCircle * soundbuffer = NULL;

WiiMovie::WiiMovie(const char * filepath)
{
	currentFrame = 0.0f;
	fps = 0.0f;
	whichLoad = 0;
	bDecoding = false;
	ExitRequested = false;
	Playing = false;
	volume = 255*Settings.MusicVolume/100;
	ReadThread = LWP_THREAD_NULL;
	DecThread = LWP_THREAD_NULL;
	ReadStackBuf = DecStackBuf = NULL;
	FrameBufCount = 0;

	for(int i = 0; i < FRAME_BUFFERS; ++i)
		FrameBuf[i] = NULL;

	background = new GuiImage(screenwidth, screenheight, (GXColor){0, 0, 0, 255});

	trigB = new GuiTrigger();
	trigB->SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);

	exitBtn = new GuiButton(1, 1);
	exitBtn->SetTrigger(trigB);
	exitBtn->Clicked.connect(this, &WiiMovie::OnExitClick);

	MusicPlayer::Instance()->Pause();

	string file(filepath);
	Video = openVideo(file);
	if(!Video)
	{
		ShowError(tr("Unsupported format!"));
		ExitRequested = true;
		Application::Instance()->PushForDelete(this);
		return;
	}

	SndChannels = Video->getNumChannels();
	SndFrequence = Video->getFrequency();
	fps = Video->getFps();
	maxSoundSize = Video->getMaxAudioSamples()*Video->getNumChannels()*2;

	if(Video->hasSound())
	{
		soundbuffer = &SoundBuffer;
		SoundBuffer.Resize(SND_BUFFERS);
		SoundBuffer.SetBufferBlockSize(maxSoundSize*FRAME_BUFFERS);
	}

	const int ReadStackBufSize = 32768;
	const int DecStackBufSize = 16384;

	ReadStackBuf = (u8 *) memalign(32, ReadStackBufSize + DecStackBufSize);
	if(!ReadStackBuf)
	{
		ShowError(tr("Not enough memory"));
		ExitRequested = true;
		Application::Instance()->PushForDelete(this);
		return;
	}

	DecStackBuf = ReadStackBuf + ReadStackBufSize;

	LWP_CreateThread (&ReadThread, UpdateThread, this, ReadStackBuf, ReadStackBufSize, 75);
	LWP_CreateThread (&DecThread, DecodeThread, this, DecStackBuf, DecStackBufSize, 70);
}

WiiMovie::~WiiMovie()
{
	Playing = false;
	ExitRequested = true;

	ASND_StopVoice(0);
	MusicPlayer::Instance()->Resume();

	if(ReadThread != LWP_THREAD_NULL)
	{
		LWP_ResumeThread(ReadThread);
		LWP_JoinThread(ReadThread, NULL);
	}
	if(DecThread != LWP_THREAD_NULL)
	{
		LWP_ResumeThread(DecThread);
		LWP_JoinThread(DecThread, NULL);
	}

	for(int i = 0; i < FRAME_BUFFERS; ++i)
	{
		if(FrameBuf[i])
			free(FrameBuf[i]);
	}

	if(ReadStackBuf != NULL)
		free(ReadStackBuf);

	delete background;
	delete exitBtn;
	delete trigB;

	if(Video)
	   closeVideo(Video);
}

bool WiiMovie::Play()
{
	if(!Video)
		return false;

	Playing = true;
	PlayTime.reset();

	LWP_ResumeThread(ReadThread);
	LWP_ResumeThread(DecThread);

	return true;
}

void WiiMovie::Stop()
{
	ExitRequested = true;
}

void WiiMovie::SetVolume(int vol)
{
	volume = 255*vol/100;
	ASND_ChangeVolumeVoice(0, volume, volume);
}

void WiiMovie::OnExitClick(GuiButton *sender UNUSED, int pointer UNUSED, const POINT &p UNUSED)
{
	Application::Instance()->PushForDelete(this);
}

void WiiMovie::SetFullscreen()
{
	if(!Video)
		return;

	float newscale = 1.0f;

	if(width < screenwidth && height < screenheight)
	{
		if((screenheight - height) > (screenwidth - width))
		{
			newscale = (float) screenheight / (float) height;
		}
		else
		{
			newscale = (float) screenwidth / (float) width;
		}
	}
	else
	{
		if((height - screenheight) > (width - screenwidth))
		{
			newscale = (float) screenheight / (float) height;
		}
		else
		{
			newscale = (float) screenwidth / (float) width;
		}
	}

	SetScale(newscale);
}

void WiiMovie::SetFrameSize(int w, int h)
{
	if(!Video)
		return;

	SetScaleX((float) w /(float) GetWidth());
	SetScaleY((float) h /(float) GetHeight());
}

void WiiMovie::SetAspectRatio(float Aspect)
{
	if(!Video)
		return;

	float vidwidth = (float) GetHeight()*GetScaleY()*Aspect;

	SetScaleX((float) GetWidth()/vidwidth);
}

extern "C" void THPSoundCallback(int voice UNUSED)
{
	if(!soundbuffer || !soundbuffer->IsBufferReady())
		return;

	if(ASND_AddVoice(0, soundbuffer->GetBuffer(), soundbuffer->GetBufferSize()) != SND_OK)
	{
		return;
	}

	soundbuffer->LoadNext();
}

void * WiiMovie::UpdateThread(void *arg)
{
	WiiMovie * Movie = static_cast<WiiMovie *>(arg);

	while(!Movie->ExitRequested)
	{
		Movie->ReadNextFrame();

		usleep(5000);
	}
	return NULL;
}

void * WiiMovie::DecodeThread(void *arg)
{
	WiiMovie * Movie = static_cast<WiiMovie *>(arg);

	int oldFrame = 0;

	while(!Movie->ExitRequested)
	{
		if(!Movie->Playing)
			LWP_SuspendThread(Movie->DecThread);

		oldFrame = Movie->Video->getCurrentFrameNr();
		Movie->DecodeNextFrame();

		while(   !Movie->ExitRequested
			  && (   (Movie->FrameBufCount >= FRAME_BUFFERS)
				  || (oldFrame == Movie->Video->getCurrentFrameNr())))
		{
			usleep(100);
		}
	}

	return NULL;
}

void WiiMovie::ReadNextFrame()
{
	if(!Playing)
		LWP_SuspendThread(ReadThread);

	float FrameExpected = PlayTime.elapsed() * fps; // float is enough for up to 74 hours straight playing in 60 fps

	while(currentFrame < FrameExpected)
	{
		readDecodeMutex.lock();
		Video->loadNextFrame();
		readDecodeMutex.unlock();

		currentFrame += 1.0f;

		if(Video->hasSound())
		{
			// check if we are not at the pre-last buffer (last buffer is playing)
			if(	(whichLoad == (SoundBuffer.Which()-2))
				|| ((SoundBuffer.Which() == 0) && (whichLoad == SoundBuffer.Size()-2))
				|| ((SoundBuffer.Which() == 1) && (whichLoad == SoundBuffer.Size()-1)))
			{
				return;
			}

			int currentSize = SoundBuffer.GetBufferSize(whichLoad);

			currentSize += Video->getCurrentBuffer((s16 *) (&SoundBuffer.GetBuffer(whichLoad)[currentSize]))*SndChannels*2;
			SoundBuffer.SetBufferSize(whichLoad, currentSize);

			if(currentSize >= (FRAME_BUFFERS-1) * maxSoundSize)
			{
				SoundBuffer.SetBufferReady(whichLoad, true);

				if(++whichLoad >= SoundBuffer.Size())
					whichLoad = 0;
			}

			if(SoundBuffer.IsBufferReady() && ASND_StatusVoice(0) == SND_UNUSED)
			{
				ASND_StopVoice(0);
				ASND_SetVoice(0, (SndChannels == 2) ? VOICE_STEREO_16BIT : VOICE_MONO_16BIT, SndFrequence, 0,
							  SoundBuffer.GetBuffer(), SoundBuffer.GetBufferSize(), volume, volume, THPSoundCallback);
				SoundBuffer.LoadNext();
			}
		}
	}
}

void WiiMovie::DecodeNextFrame()
{
	if(!Video || !Playing)
		return;

	std::vector<u8> frameBuffer;

	readDecodeMutex.lock();
	Video->copyCurrentFrame(frameBuffer);
	readDecodeMutex.unlock();

	Video->decodeVideoFrame(VideoF, frameBuffer);

	if(!VideoF.getData())
		return;

	bDecoding = true;

	//! remember width to avoid unnecessary deallocate
	if(width != VideoF.getWidth())
	{
		width = VideoF.getWidth();
		height = VideoF.getHeight();
		SetFullscreen();
		//! release buffer as we might need more now
		for(int i = 0; i < FRAME_BUFFERS; ++i)
		{
			if(FrameBuf[i])
				free(FrameBuf[i]);
			FrameBuf[i] = NULL;
		}
	}

	if(!FrameBuf[FrameBufCount])
		FrameBuf[FrameBufCount] = (u8 *) memalign(32, (width * height) << 1);

	RGB8ToRGB565(VideoF.getData(), FrameBuf[FrameBufCount], width, height);
	FrameBufCount++;

	bDecoding = false;
}

void WiiMovie::Draw()
{
	if(!Video)
		return;

	background->Draw();

	if(FrameBufCount > 0)
	{
		if(FrameBuf[0])
			Menu_DrawImg(FrameBuf[0], width, height, GX_TF_RGB565, GetLeft(), GetTop(), 0.0f, 0.0f, scaleX, scaleY, alpha);

		//! rotate FIFO
		//! we don't need to lock as render thread is highest prio and interrupts the decode thread
		//! check if decode thread wasn't interrupted in the middle of decoding though
		if(FrameBufCount > 1 && !bDecoding)
		{
			u8 *tmp = FrameBuf[0];

			for(int i = 1; i < FrameBufCount; ++i)
				FrameBuf[i-1] = FrameBuf[i];

			//! set first on the last position to avoid unnecessary deallocate
			FrameBuf[FrameBufCount-1] = tmp;
			FrameBufCount--;
		}
	}
}

void WiiMovie::Update(GuiTrigger * t)
{
	exitBtn->Update(t);
}
