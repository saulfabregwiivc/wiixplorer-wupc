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
#include "Controls/Application.h"
#include "Controls/Taskbar.h"
#include "Prompts/PromptWindows.h"
#include "FileOperations/fileops.h"
#include "FileOperations/FileLoadTask.h"
#include "FileStartUp/FileExtensions.h"
#include "ImageOperations/ImageViewer.h"
#include "sys.h"
#include "input.h"

ImageViewer::ImageViewer(const char *filepath)
	: GuiFrame(0, 0)
	, CThread(75, 32768)
{
	currentImage = 0;
	SlideShowStart = 0;
	imageDir = NULL;
	image = NULL;
	imageData = NULL;
	newImageData = NULL;
	rotateLeft = 90;
	rotateRight = 90;
	buttonAlpha = 255;
	updateAlpha = false;
	isPointerVisible = true;
	bExitRequested = false;
	bSlideShowFadeStart = false;

	for (int i = 0; i < 4; i++)
		isAButtonPressed[i] = false;

	width = screenwidth;
	height = screenheight;

	this->Setup();

	SetEffect(EFFECT_FADE, 50);

	startThread();

	if(filepath)
	{
		currentImagePath = filepath;
		this->LoadImageList(filepath);
	}
}

ImageViewer::~ImageViewer()
{
	RemoveAll();

	//! the loading thread is stopped on button click
	while(!threadTasks.empty())
	{
		Taskbar::Instance()->RemoveTask(dynamic_cast<FileLoadTask *>(threadTasks.front()));
		delete threadTasks.front();
		threadTasks.pop();
	}

	for(int i = 0; i < 4; i++)
		Application::Instance()->ResetPointer(i);

	if(imageDir)
		delete imageDir;

	if(image)
		delete image;

	if(imageData)
		delete imageData;

	delete nextButton;
	delete prevButton;
	delete stopSlideshowButton;
	delete slideshowButton;
	delete zoominButton;
	delete zoomoutButton;
	delete rotateLButton;
	delete rotateRButton;
	delete backButton;
	delete moveButton;
	delete trashButton;

	delete trigger;
	delete trigNext;
	delete trigPrev;
	delete trigB;
	delete trigA_Held;
	delete trigPlus_Held;
	delete trigMinus_Held;
	delete trigRotateL;
	delete trigRotateR;
	delete trigSlideshow;

	delete backGround;

	Resources::Remove(backButtonData);
	Resources::Remove(backButtonOverData);
	delete backButtonImage;
	delete backButtonOverImage;
	Resources::Remove(zoominButtonData);
	Resources::Remove(zoominButtonOverData);
	delete zoominButtonImage;
	delete zoominButtonOverImage;
	Resources::Remove(zoomoutButtonData);
	Resources::Remove(zoomoutButtonOverData);
	delete zoomoutButtonImage;
	delete zoomoutButtonOverImage;
	Resources::Remove(rotateRButtonData);
	Resources::Remove(rotateRButtonOverData);
	delete rotateRButtonImage;
	delete rotateRButtonOverImage;
	Resources::Remove(rotateLButtonData);
	Resources::Remove(rotateLButtonOverData);
	delete rotateLButtonImage;
	delete rotateLButtonOverImage;
	Resources::Remove(nextButtonData);
	Resources::Remove(nextButtonOverData);
	delete nextButtonImage;
	delete nextButtonOverImage;
	Resources::Remove(prevButtonData);
	Resources::Remove(prevButtonOverData);
	delete prevButtonImage;
	delete prevButtonOverImage;
	Resources::Remove(slideshowButtonData);
	Resources::Remove(slideshowButtonOverData);
	delete slideshowButtonImage;
	delete slideshowButtonOverImage;
	Resources::Remove(trashImgData);
	delete trashImg;

	// allow user input again
	Application::Instance()->SetGuiInputUpdate(true);
}

void ImageViewer::ZoomIn()
{
	if(!image)
		return;

	SetImageSize(image->GetScale() + image->GetScale() * 0.01f);
}

void ImageViewer::ZoomOut()
{
	if(!image)
		return;

	SetImageSize(image->GetScale() - image->GetScale() * 0.01f);
}

void ImageViewer::SetImageSize(float scale)
{
	if(!image)
		return;

	if(scale < 0.000001f)
		scale = 0.000001f;

	image->SetScale(scale);
}

void ImageViewer::SetStartUpImageSize()
{
	if(!image)
		return;

	float scale = 1.0f;
	int imgwidth = image->GetWidth();
	int imgheight = image->GetHeight();

	//! we only scale down if the image is too big, no upscale on start
	if(imgwidth > width || imgheight > height)
	{
		if((imgheight - height) > (imgwidth - width))
		{
			scale = (float) height / (float) imgheight;
		}
		else
		{
			scale = (float) width / (float) imgwidth;
		}
	}

	int PositionX = GetLeft() + width/2 - imgwidth/2;
	int PositionY = GetTop() + height/2 - imgheight/2;
	image->SetScale(scale);
	image->SetPosition(PositionX, PositionY);
}

bool ImageViewer::NextImage(bool silent)
{
	currentImage++;
	if(currentImage >= imageDir->GetFilecount())
		currentImage = 0;

	return LoadImage(currentImage, silent);
}

bool ImageViewer::PreviousImage(bool silent)
{
	currentImage--;
	if(currentImage < 0)
		currentImage = imageDir->GetFilecount()-1;

	return LoadImage(currentImage, silent);
}

void ImageViewer::StartSlideShow()
{
	//start a slideshow
	SlideShowStart = time(0);

	GXColor color = (GXColor){0, 0, 0, 255};
	backGround->SetImageColor(&color);

	Remove(moveButton);
	Remove(backButton);
	Remove(slideshowButton);
	Remove(zoominButton);
	Remove(zoomoutButton);
	Remove(rotateLButton);
	Remove(rotateRButton);
	Remove(prevButton);
	Remove(nextButton);
	Append(stopSlideshowButton);
}

void ImageViewer::StopSlideShow()
{
	//stop a slideshow
	SlideShowStart = 0;

	GXColor color = (GXColor){0, 0, 0, 0x50};
	backGround->SetImageColor(&color);

	Append(moveButton);
	Append(backButton);
	Append(slideshowButton);
	Append(zoominButton);
	Append(zoomoutButton);
	Append(rotateLButton);
	Append(rotateRButton);
	Append(prevButton);
	Append(nextButton);

	Remove(stopSlideshowButton);
}

void ImageViewer::OnButtonReleased(GuiButton *sender UNUSED, int pointer, const POINT &p UNUSED)
{
	Application::Instance()->ResetPointer(pointer);
}

void ImageViewer::OnButtonHeld(GuiButton *sender, int pointer UNUSED, const POINT &p)
{
	if (sender == moveButton && image)
	{
		image->SetPosition(p.x-clickPosX, p.y-clickPosY);
	}

	else if(sender == zoominButton)
	{
		ZoomIn();
	}

	else if(sender == zoomoutButton)
	{
		ZoomOut();
	}
}

void ImageViewer::OnButtonClick(GuiButton *sender, int pointer, const POINT &p)
{
	if (sender == moveButton && image)
	{
		Application::Instance()->SetGrabPointer(pointer);
		clickPosX = p.x - image->GetLeft();
		clickPosY = p.y - image->GetTop();
	}

	else if(sender == backButton)
	{
		//! disable all gui user inputs
		Application::Instance()->SetGuiInputUpdate(false);
		//! has to be called here because of the LOCK in the delete queue
		//! to avoid lock on autodeleting the running file load tasks
		bExitRequested = true;
	}

	else if(sender == slideshowButton)
	{
		StartSlideShow();
	}

	else if(sender == stopSlideshowButton)
	{
		StopSlideShow();
	}

	else if(sender == rotateRButton)
	{
		if (image && !((int)image->GetAngle()%90))
		{
			rotateRight = 0;
			currentAngle = image->GetAngle();
		}
	}

	else if(sender == rotateLButton)
	{
		if (image && !((int)image->GetAngle()%90))
		{
			rotateLeft = 0;
			currentAngle = image->GetAngle();
		}
	}

	else if(sender == nextButton)
	{
		NextImage();
	}

	else if(sender == prevButton)
	{
		PreviousImage();
	}

	else if (sender == trashButton && imageDir && image)
	{
		int choice = WindowPrompt(tr("Do you want to delete this file:"), imageDir->GetFilename(currentImage), tr("Yes"), tr("Cancel"));
		if (choice)
		{
			if(!RemoveFile(imageDir->GetFilepath(currentImage)))
				ShowError(tr("File could not be deleted."));
			else
			{
				NextImage();
				LoadImageList(imageDir->GetFilepath(currentImage));
			}
		}
	}
}

void ImageViewer::executeThread()
{
	while(!bExitRequested)
	{
		if(!threadTasks.empty())
		{
			threadMutex.lock();
			ThreadedTask *tmpTask = threadTasks.front();
			threadTasks.pop();
			threadMutex.unlock();
			tmpTask->Execute();
		}
		else
		{
			usleep(100000);
		}
	}

	SetEffect(EFFECT_FADE, -50);
	Application::Instance()->PushForDelete(this);
}

bool ImageViewer::LoadImageList(const char * filepath)
{
	char path[strlen(filepath)+1];
	snprintf(path, sizeof(path), "%s", filepath);

	char *ptr = strrchr(path, '/');
	if (!ptr) {
		return false;
	}

	ptr++;
	ptr[0] = '\0';

	if(imageDir)
		delete imageDir;

	imageDir = new DirListAsync(path, Settings.FileExtensions.GetImage(), DirList::Files);
	imageDir->FinishList.connect(this, &ImageViewer::OnDirListLoaded);
	threadMutex.lock();
	threadTasks.push(imageDir);
	threadMutex.unlock();

	return true;
}

void ImageViewer::OnDirListLoaded(DirListAsync *dirList)
{
	const char * filename = FullpathToFilename(currentImagePath.c_str());

	currentImage = dirList->GetFileIndex(filename);
	if (currentImage < 0)
	{
		delete imageDir;
		imageDir = NULL;
		return;
	}

	LoadImage(currentImage);
}

bool ImageViewer::LoadImage(int index, bool silent)
{
	if(!imageDir)
		return false;

	if(index < 0 || index >= imageDir->GetFilecount()) {
		return false;
	}

	const char * filename = imageDir->GetFilename(index);
	const char * filepath = imageDir->GetFilepath(index);

	if (filename == NULL || filepath == NULL) {
		return false;
	}

	currentImagePath = filepath;

	FileLoadTask *task = new FileLoadTask(filepath, silent);
	task->LoadingComplete.connect(this, &ImageViewer::OnFinishedImageLoad);
	task->SetAutoDelete(true);
	Taskbar::Instance()->AddTask(task);
	threadMutex.lock();
	threadTasks.push(task);
	threadMutex.unlock();

	return true;
}

void ImageViewer::OnFinishedImageLoad(u8 *buffer, u32 buffer_size)
{
	GuiImageData *newImage = new GuiImageData(buffer, buffer_size);

	if(buffer)
	{
		free(buffer);
		buffer = NULL;
	}

	if (newImage->GetImage() == NULL)
	{
		delete newImage;
		return;
	}

	// wait for the image to load up if the pointer is in use otherwise it will leak the pointer
	while(newImageData != NULL)
		usleep(1000);

	newImageData = newImage;
}

void ImageViewer::Setup()
{
	int DefaultButtonWidth = 96;

	trigger = new GuiTrigger();
	trigA_Held = new GuiTrigger();
	trigPlus_Held = new GuiTrigger();
	trigMinus_Held = new GuiTrigger();
	trigNext = new GuiTrigger();
	trigPrev = new GuiTrigger();
	trigB = new GuiTrigger();
	trigRotateL = new GuiTrigger();
	trigRotateR = new GuiTrigger();
	trigSlideshow = new GuiTrigger();

	trigA_Held->SetHeldTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigPlus_Held->SetButtonOnlyHeldTrigger(-1, WiiControls.ZoomIn | ClassicControls.ZoomIn << 16, GCControls.ZoomIn);
	trigMinus_Held->SetButtonOnlyHeldTrigger(-1, WiiControls.ZoomOut | ClassicControls.ZoomOut << 16, GCControls.ZoomOut);
	trigger->SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigB->SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);
	trigPrev->SetButtonOnlyTrigger(-1, WiiControls.LeftButton | ClassicControls.LeftButton << 16, GCControls.LeftButton);
	trigNext->SetButtonOnlyTrigger(-1, WiiControls.RightButton | ClassicControls.RightButton << 16, GCControls.RightButton);
	trigRotateL->SetButtonOnlyTrigger(-1, WiiControls.UpButton | ClassicControls.UpButton << 16, GCControls.UpButton);
	trigRotateR->SetButtonOnlyTrigger(-1, WiiControls.DownButton | ClassicControls.DownButton << 16, GCControls.DownButton);
	trigSlideshow->SetButtonOnlyTrigger(-1, WiiControls.SlideShowButton | ClassicControls.SlideShowButton << 16, GCControls.SlideShowButton);

	backGround = new GuiImage(width, height, (GXColor){0, 0, 0, 0x50});
	Append(backGround);

	//! place the image after the background and under everything else
	image = new GuiImage();
	//! cut everything outside of screen with a 200 offset (for rotation)
	//! this resolves the glitches when zooming too far into the texture
	image->SetBounds(iRect(-200, -200, screenwidth + 200, screenheight + 200));
	Append(image);

	backButtonData = Resources::GetImageData("back.png");
	backButtonOverData = Resources::GetImageData("back_over.png");
	backButtonImage = new GuiImage(backButtonData);
	backButtonOverImage = new GuiImage(backButtonOverData);

	backButton = new GuiButton(backButtonImage->GetWidth(), backButtonImage->GetHeight());
	backButton->SetImage(backButtonImage);
	backButton->SetImageOver(backButtonOverImage);
	backButton->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	backButton->SetPosition(-2*DefaultButtonWidth-DefaultButtonWidth/2-2, -16);
	backButton->SetTrigger(trigger);
	backButton->SetTrigger(trigB);
	backButton->Clicked.connect(this, &ImageViewer::OnButtonClick);
	Append(backButton);

	zoominButtonData = Resources::GetImageData("zoomin.png");
	zoominButtonOverData = Resources::GetImageData("zoomin_over.png");
	zoominButtonImage = new GuiImage(zoominButtonData);
	zoominButtonOverImage = new GuiImage(zoominButtonOverData);

	zoominButton = new GuiButton(zoominButtonImage->GetWidth(), zoominButtonImage->GetHeight());
	zoominButton->SetImage(zoominButtonImage);
	zoominButton->SetImageOver(zoominButtonOverImage);
	zoominButton->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	zoominButton->SetPosition(-DefaultButtonWidth/2, -16);
	zoominButton->SetHoldable(true);
	zoominButton->SetTrigger(0, trigPlus_Held);
	zoominButton->SetTrigger(1, trigA_Held);
	zoominButton->Held.connect(this, &ImageViewer::OnButtonHeld);
	Append(zoominButton);

	zoomoutButtonData = Resources::GetImageData("zoomout.png");
	zoomoutButtonOverData = Resources::GetImageData("zoomout_over.png");
	zoomoutButtonImage = new GuiImage(zoomoutButtonData);
	zoomoutButtonOverImage = new GuiImage(zoomoutButtonOverData);

	zoomoutButton = new GuiButton(zoomoutButtonImage->GetWidth(), zoomoutButtonImage->GetHeight());
	zoomoutButton->SetImage(zoomoutButtonImage);
	zoomoutButton->SetImageOver(zoomoutButtonOverImage);
	zoomoutButton->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	zoomoutButton->SetPosition(DefaultButtonWidth/2, -16);
	zoomoutButton->SetHoldable(true);
	zoomoutButton->SetTrigger(0, trigMinus_Held);
	zoomoutButton->SetTrigger(1, trigA_Held);
	zoomoutButton->Held.connect(this, &ImageViewer::OnButtonHeld);
	Append(zoomoutButton);

	rotateRButtonData = Resources::GetImageData("rotateR.png");
	rotateRButtonOverData = Resources::GetImageData("rotateR_over.png");
	rotateRButtonImage = new GuiImage(rotateRButtonData);
	rotateRButtonOverImage = new GuiImage(rotateRButtonOverData);

	rotateRButton = new GuiButton(rotateRButtonImage->GetWidth(), rotateRButtonImage->GetHeight());
	rotateRButton->SetImage(rotateRButtonImage);
	rotateRButton->SetImageOver(rotateRButtonOverImage);
	rotateRButton->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	rotateRButton->SetPosition(DefaultButtonWidth+DefaultButtonWidth/2, -16);
	rotateRButton->SetTrigger(trigger);
	rotateRButton->SetTrigger(trigRotateR);
	rotateRButton->Clicked.connect(this, &ImageViewer::OnButtonClick);
	Append(rotateRButton);

	rotateLButtonData = Resources::GetImageData("rotateL.png");
	rotateLButtonOverData = Resources::GetImageData("rotateL_over.png");
	rotateLButtonImage = new GuiImage(rotateLButtonData);
	rotateLButtonOverImage = new GuiImage(rotateLButtonOverData);

	rotateLButton = new GuiButton(rotateLButtonImage->GetWidth(), rotateLButtonImage->GetHeight());
	rotateLButton->SetImage(rotateLButtonImage);
	rotateLButton->SetImageOver(rotateLButtonOverImage);
	rotateLButton->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	rotateLButton->SetPosition(2*DefaultButtonWidth+DefaultButtonWidth/2+2, -16);
	rotateLButton->SetTrigger(trigger);
	rotateLButton->SetTrigger(trigRotateL);
	rotateLButton->Clicked.connect(this, &ImageViewer::OnButtonClick);
	Append(rotateLButton);

	nextButtonData = Resources::GetImageData("next.png");
	nextButtonOverData = Resources::GetImageData("next_over.png");
	nextButtonImage = new GuiImage(nextButtonData);
	nextButtonOverImage = new GuiImage(nextButtonOverData);

	nextButton = new GuiButton(nextButtonImage->GetWidth(), nextButtonImage->GetHeight());
	nextButton->SetImage(nextButtonImage);
	nextButton->SetImageOver(nextButtonOverImage);
	nextButton->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	nextButton->SetPosition(-16, 198);
	nextButton->SetTrigger(trigger);
	nextButton->SetTrigger(trigNext);
	nextButton->Clicked.connect(this, &ImageViewer::OnButtonClick);
	Append(nextButton);

	prevButtonData = Resources::GetImageData("prev.png");
	prevButtonOverData = Resources::GetImageData("prev_over.png");
	prevButtonImage = new GuiImage(prevButtonData);
	prevButtonOverImage = new GuiImage(prevButtonOverData);

	prevButton = new GuiButton(prevButtonImage->GetWidth(), prevButtonImage->GetHeight());
	prevButton->SetImage(prevButtonImage);
	prevButton->SetImageOver(prevButtonOverImage);
	prevButton->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	prevButton->SetPosition(16, 198);
	prevButton->SetTrigger(trigger);
	prevButton->SetTrigger(trigPrev);
	prevButton->Clicked.connect(this, &ImageViewer::OnButtonClick);
	Append(prevButton);

	slideshowButtonData = Resources::GetImageData("slideshow.png");
	slideshowButtonOverData = Resources::GetImageData("slideshow_over.png");
	slideshowButtonImage = new GuiImage(slideshowButtonData);
	slideshowButtonOverImage = new GuiImage(slideshowButtonOverData);

	slideshowButton = new GuiButton(slideshowButtonImage->GetWidth(), slideshowButtonImage->GetHeight());
	slideshowButton->SetImage(slideshowButtonImage);
	slideshowButton->SetImageOver(slideshowButtonOverImage);
	slideshowButton->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	slideshowButton->SetPosition(-DefaultButtonWidth-DefaultButtonWidth/2, -16);
	slideshowButton->SetTrigger(trigger);
	slideshowButton->SetTrigger(trigSlideshow);
	slideshowButton->Clicked.connect(this, &ImageViewer::OnButtonClick);
	Append(slideshowButton);

	stopSlideshowButton = new GuiButton(Application::Instance()->GetWidth(), Application::Instance()->GetHeight());
	stopSlideshowButton->SetTrigger(trigger);
	stopSlideshowButton->SetTrigger(trigB);
	stopSlideshowButton->Clicked.connect(this, &ImageViewer::OnButtonClick);

	moveButton = new GuiButton(screenwidth-(prevButton->GetLeft()+prevButton->GetWidth())*2, screenheight-backButton->GetHeight()-16);
	moveButton->SetPosition(prevButton->GetLeft()+prevButton->GetWidth(), 0);
	moveButton->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	moveButton->SetHoldable(true);
	moveButton->SetTrigger(trigA_Held);
	moveButton->Clicked.connect(this, &ImageViewer::OnButtonClick);
	moveButton->Held.connect(this, &ImageViewer::OnButtonHeld);
	moveButton->Released.connect(this, &ImageViewer::OnButtonReleased);
	Append(moveButton);

	trashImgData = Resources::GetImageData("trash.png");
	trashImg = new GuiImage(trashImgData);
	trashImg->SetAlpha(120);
	trashImg->SetScale(1.1);
	trashButton = new GuiButton(trashImgData->GetWidth(), trashImgData->GetHeight());
	trashButton->SetImage(trashImg);
	trashButton->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	trashButton->SetPosition(-30, 30);
	trashButton->SetTrigger(trigger);
	trashButton->SetEffectGrow();
	trashButton->Clicked.connect(this, &ImageViewer::OnButtonClick);
	Append(trashButton);

	SetEffect(EFFECT_FADE, 50);
}

void ImageViewer::Draw()
{
	if(SlideShowStart > 0)
	{
		time_t currentTime = time(0);
		if(currentTime-SlideShowStart >= Settings.SlideshowDelay)
		{
			SlideShowStart = currentTime;
			NextImage(true);
		}
	}

	bool PointerVisibleOld = isPointerVisible;
	isPointerVisible = false;

	for(int i = 0; i < 4; ++i)
	{
		if(userInput[i].wpad.ir.valid)
			isPointerVisible = true;
	}

	if(PointerVisibleOld != isPointerVisible)
		updateAlpha = true;

	if (updateAlpha)
	{
		nextButton->SetAlpha(buttonAlpha);
		prevButton->SetAlpha(buttonAlpha);

		zoominButton->SetAlpha(buttonAlpha);
		zoomoutButton->SetAlpha(buttonAlpha);
		backButton->SetAlpha(buttonAlpha);
		slideshowButton->SetAlpha(buttonAlpha);
		rotateLButton->SetAlpha(buttonAlpha);
		rotateRButton->SetAlpha(buttonAlpha);
		trashButton->SetAlpha(buttonAlpha);

		if (isPointerVisible)
		{
			if ((buttonAlpha += 5) >= 255)
				updateAlpha = false;
		}
		else
		{
			if ((buttonAlpha -= 5) <= 0)
				updateAlpha = false;
		}
	}

	if (image && rotateRight < 90)
	{
		image->SetAngle(currentAngle + (rotateRight += 3));
	}

	if (image && rotateLeft < 90)
	{
		image->SetAngle(currentAngle - (rotateLeft += 3));
	}

	if(newImageData)
	{
		if(!bExitRequested && SlideShowStart > 0 && !bSlideShowFadeStart && !image->IsAnimated())
		{
			image->SetEffect(EFFECT_FADE, -Settings.ImageFadeSpeed);
			// mark that an effect is started to run
			bSlideShowFadeStart = true;
		}
		else if(!image->IsAnimated())
		{
			// mark that the effect has finished and image was changed
			bSlideShowFadeStart = false;

			image->SetImage(newImageData);

			if (imageData != NULL)
				delete imageData;

			imageData = newImageData;
			newImageData = NULL;

			//!Set original size first if not over the limits
			SetStartUpImageSize();

			if(!bExitRequested && SlideShowStart > 0)
				image->SetEffect(EFFECT_FADE, Settings.ImageFadeSpeed);

			//!Substract loading time from timer for slideshow
			if(SlideShowStart > 0)
				SlideShowStart = time(0);
		}
	}
	GuiFrame::Draw();
}
