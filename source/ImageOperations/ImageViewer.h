/****************************************************************************
 * Copyright (C) 2009 r-win
 * Copyright (C) 2009 - 2012 Dimok
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
#ifndef __IMAGEVIEWER_H
#define __IMAGEVIEWER_H

#include "GUI/gui.h"
#include "Controls/CThread.h"
#include "FileOperations/DirListAsync.h"
#include "Controls/CMutex.h"

class ImageViewer : public GuiFrame, public CThread, public sigslot::has_slots<>
{
	public:
		//!Constructor
		//!\param path Path from where to load the filelist of all images or the image itself
		ImageViewer(const char *filepath);
		//!Destructor
		~ImageViewer();
		//!Zoom into the image (currently 20% till a limit of screenwidth or screenheight)
		virtual void ZoomIn();
		//!Zoom out of the image (currently 20% till a limit of MIN_IMAGE_WIDTH or MIN_IMAGE_HEIGHT)
		virtual void ZoomOut();
		//!Set image size  (limits described above)
		//!\param scale of the image (%/100)
		virtual void SetImageSize(float scale);
		//!Set the startup image size when loading an image
		//!Sets up imagesize at 1.0 scale and limits it to screenwidth/screenheight if it's over size
		virtual void SetStartUpImageSize();
		//!Load NextImage in the Directory
		virtual bool NextImage(bool silent = false);
		//!Load PreviousImage in the directory
		virtual bool PreviousImage(bool silent = false);
		//!Start a Slideshow of the images in the directory (slidespeed: TIME_BETWEEN_IMAGES)
		virtual void StartSlideShow();
		//!Stop a Slideshow
		virtual void StopSlideShow();
		//!Replacement for draw function
		virtual void Draw();
	protected:
		//!Setup the needed images/buttons/texts for the ImageViewer
		virtual void Setup();
		//!Intern image loading funtion.
		//!\param index of image in the directory
		virtual bool LoadImage(int index, bool silent = false);
		//!Intern function to seperate from constuctor
		//!\param path Path from where to load the filelist of all images or the image itself
		virtual bool LoadImageList(const char * filepath);
		//!OnButtonClick intern callback for buttonclicks.
		virtual void OnButtonClick(GuiButton *sender, int pointer, const POINT &p);
		//!OnButtonHeld intern callback for button hold.
		virtual void OnButtonHeld(GuiButton *sender, int pointer, const POINT &p);
		//!OnButtonReleased intern callback for button release.
		virtual void OnButtonReleased(GuiButton *sender, int pointer, const POINT &p);
		//!This function is called when the listing of directory is finished
		void OnDirListLoaded(DirListAsync *dirList);
		//!This function is called when loading the image in the background is finished
		void OnFinishedImageLoad(u8 *buffer, u32 buffer_size);
		//!Thread for loading images
		virtual void executeThread();
		//!Variables of the ImageViewer
		DirListAsync * imageDir;
		//!variable for internal thread to detect a task needs to be done
		queue<ThreadedTask *> threadTasks;
		CMutex threadMutex;

		int currentImage;
		int clickPosX;
		int clickPosY;
		int buttonAlpha;
		int rotateRight;
		int rotateLeft;
		bool isPointerVisible;
		bool bExitRequested;
		bool isAButtonPressed[4];
		bool updateAlpha;
		bool bSlideShowFadeStart;
		float currentAngle;

		time_t SlideShowStart;

		string currentImagePath;

		GuiButton * prevButton;
		GuiButton * nextButton;
		GuiButton * stopSlideshowButton;
		GuiButton * slideshowButton;
		GuiButton * zoominButton;
		GuiButton * zoomoutButton;
		GuiButton * rotateLButton;
		GuiButton * rotateRButton;
		GuiButton * backButton;
		GuiButton * moveButton;
		GuiButton * trashButton;

		GuiImage * image;
		GuiImageData *imageData;
		GuiImageData *newImageData;

		GuiImageData * nextButtonData;
		GuiImageData * nextButtonOverData;
		GuiImageData * prevButtonData;
		GuiImageData * prevButtonOverData;
		GuiImageData * slideshowButtonData;
		GuiImageData * slideshowButtonOverData;
		GuiImageData * backButtonData;
		GuiImageData * backButtonOverData;
		GuiImageData * zoominButtonData;
		GuiImageData * zoominButtonOverData;
		GuiImageData * zoomoutButtonData;
		GuiImageData * zoomoutButtonOverData;
		GuiImageData * rotateRButtonData;
		GuiImageData * rotateRButtonOverData;
		GuiImageData * rotateLButtonData;
		GuiImageData * rotateLButtonOverData;
		GuiImageData * trashImgData;

		GuiImage * backGround;
		GuiImage * nextButtonImage;
		GuiImage * nextButtonOverImage;
		GuiImage * prevButtonImage;
		GuiImage * prevButtonOverImage;
		GuiImage * slideshowButtonImage;
		GuiImage * slideshowButtonOverImage;
		GuiImage * backButtonImage;
		GuiImage * backButtonOverImage;
		GuiImage * zoominButtonImage;
		GuiImage * zoominButtonOverImage;
		GuiImage * zoomoutButtonImage;
		GuiImage * zoomoutButtonOverImage;
		GuiImage * rotateRButtonImage;
		GuiImage * rotateRButtonOverImage;
		GuiImage * rotateLButtonImage;
		GuiImage * rotateLButtonOverImage;
		GuiImage * trashImg;

		GuiTrigger * trigger;
		GuiTrigger * trigNext;
		GuiTrigger * trigPrev;
		GuiTrigger * trigB;
		GuiTrigger * trigA_Held;
		GuiTrigger * trigPlus_Held;
		GuiTrigger * trigMinus_Held;
		GuiTrigger * trigRotateL;
		GuiTrigger * trigRotateR;
		GuiTrigger * trigSlideshow;
};

#endif
