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
#ifndef WII_MOVIE_H_
#define WII_MOVIE_H_

#include "GUI/gui.h"
#include "Tools/BufferCircle.hpp"
#include "Controls/CMutex.h"
#include "gcvid.h"

#define SND_BUFFERS	 8
#define FRAME_BUFFERS	 8

using namespace std;

class WiiMovie : public GuiElement, public sigslot::has_slots<>
{
	public:
		WiiMovie(const char * filepath);
		virtual ~WiiMovie();
		bool Play();
		void Stop();
		void SetVolume(int vol);
		void SetFullscreen();
		void SetFrameSize(int w, int h);
		void SetAspectRatio(float Aspect);
		void Draw();
		void Update(GuiTrigger * t);
	protected:
		void OnExitClick(GuiButton *sender, int pointer, const POINT &p);
		static void * UpdateThread(void *arg);
		static void * DecodeThread(void *arg);
		void ReadNextFrame();
		void DecodeNextFrame();

		lwp_t ReadThread, DecThread;
		u8 *ReadStackBuf, *DecStackBuf;
		CMutex readDecodeMutex;
		bool bDecoding;

		VideoFile * Video;
		VideoFrame VideoF;
		BufferCircle SoundBuffer;
		float fps;
		Timer PlayTime;
		float currentFrame;
		u8 *FrameBuf[FRAME_BUFFERS];
		int FrameBufCount;
		bool Playing;
		bool ExitRequested;
		u16 whichLoad;
		int maxSoundSize;
		int SndChannels;
		int SndFrequence;
		int volume;

		GuiImage * background;
		GuiButton * exitBtn;
		GuiTrigger * trigB;
};

#endif
