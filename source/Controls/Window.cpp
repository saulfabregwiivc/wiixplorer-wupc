/***************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 * Window.cpp
 *
 * for WiiXplorer 2010
 ***************************************************************************/
#if 0  //currently not used

#include "Memory/Resources.h"
#include "Window.h"
#include "main.h"

bool Window::busyMoving = false;

Window::Window(int w, int h)
	: GuiFrame(0, 0)
{
	width = w;
	height = h;

	/** Windowframe resources **/
	window_tile_horizontalDat = Resources::GetImageData(window_tile_horizontal_png, window_tile_horizontal_png_size);
	window_tile_verticalDat = Resources::GetImageData(window_tile_vertical_png, window_tile_vertical_png_size);
	window_corner_leftDat = Resources::GetImageData(window_corner_left_png, window_corner_left_png_size);
	window_corner_rightDat = Resources::GetImageData(window_corner_right_png, window_corner_right_png_size);

	/** Windowframe images **/
	window_tile_vertical_left = new GuiImage(window_tile_verticalDat);
	window_tile_vertical_right = new GuiImage(window_tile_verticalDat);
	window_tile_vertical_right->SetAngle(180);
	window_tile_horizontal_top = new GuiImage(window_tile_horizontalDat);
	window_tile_horizontal_bottom = new GuiImage(window_tile_horizontalDat);
	window_tile_horizontal_bottom->SetAngle(180);
	window_corner_right_top = new GuiImage(window_corner_rightDat);
	window_corner_left_bottom = new GuiImage(window_corner_rightDat);
	window_corner_left_bottom->SetAngle(180);
	window_corner_left_top = new GuiImage(window_corner_leftDat);
	window_corner_right_bottom = new GuiImage(window_corner_leftDat);
	window_corner_right_bottom->SetAngle(180);

	isMovable = true;
	isMoving = false;

	titleBar = new GuiFrame(width-18, 45);
	titleBar -> SetPosition(9, 10);

	trigA = new GuiTrigger();
	trigA -> SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	trigHeldA = new GuiTrigger();
	trigHeldA -> SetHeldTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	titleBarButton = new GuiButton(565, 45);
	titleBarButton -> SetPosition(0, 0);
	titleBarButton -> SetTrigger(trigHeldA);
	titleBarButton -> SetHoldable(true);
	titleBarButton -> Held.connect(this, &Window::OnTitleHeld);
	titleBarButton -> Released.connect(this, &Window::OnTitleReleased);

	titleText = new GuiText("", 20, (GXColor) {0, 0, 0, 255});
	titleText -> SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	titleText -> SetPosition(18, 0);
	titleText -> SetMaxWidth(width-40, SCROLL_HORIZONTAL);

	icon = new GuiImage();
	icon -> SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	icon -> SetPosition(48, 62);

	titleBar -> Append(titleText);
	titleBar -> Append(icon);
	titleBar -> Append(titleBarButton);

	SizeChanged();

	Append(window_tile_horizontal_top);
	Append(window_tile_horizontal_bottom);
	Append(window_tile_vertical_left);
	Append(window_tile_vertical_right);
	Append(window_corner_right_top);
	Append(window_corner_right_bottom);
	Append(window_corner_left_top);
	Append(window_corner_left_bottom);
	Append(titleBar);
}

Window::~Window()
{
	Closing(this);

	titleBarButton -> Held.disconnect(this);
	titleBarButton -> Released.disconnect(this);
	delete titleBarButton;
	delete titleBar;
	delete titleText;
	delete icon;

	delete window_tile_horizontal_top;
	delete window_tile_horizontal_bottom;
	delete window_tile_vertical_left;
	delete window_tile_vertical_right;
	delete window_corner_left_top;
	delete window_corner_left_bottom;
	delete window_corner_right_top;
	delete window_corner_right_bottom;

	Resources::Remove(window_tile_horizontalDat);
	Resources::Remove(window_tile_verticalDat);
	Resources::Remove(window_corner_leftDat);
	Resources::Remove(window_corner_rightDat);

	delete trigA;

	Closed(this);
}

void Window::Draw()
{
	// If the title is held, move the window
	if (isMovable && isMoving)
	{
		POINT prevPoint = clickedPoint;

		// Get the current point
		clickedPoint.x = userInput[clickedChannel].wpad->ir.x;
		clickedPoint.y = userInput[clickedChannel].wpad->ir.y;

		// Move the window, based on the top and left of this window
		// Therefor, we need to calculate the difference between the mouse down point and the topleft point of this window
		int newX = GetLeft() + (clickedPoint.x - prevPoint.x);
		int newY = GetTop() + (clickedPoint.y - prevPoint.y);

		// Move the window to newX and newY, but stay within the boundaries
		if (newX > 0 && newY > 0 && (GetParent() == NULL || (newX + GetWidth() < GetParent() -> GetWidth() && newY + GetHeight() < GetParent() -> GetHeight()))) {
			SetPosition(newX, newY);
		}
	}

	GXColor backgroundColor = (GXColor){226, 230, 237, 255};

	Menu_DrawRectangle(this->GetLeft()+window_tile_vertical_left->GetWidth(), this->GetTop()+window_tile_horizontal_top->GetHeight(), GetZPosition(), width-window_tile_vertical_left->GetWidth()*2, height-window_tile_horizontal_top->GetHeight()*2, &backgroundColor, false, true);

	GuiFrame::Draw();
}

void Window::OnTitleHeld(GuiElement *, int channel, POINT point)
{
	if (busyMoving) // We are already moving something else...
	{
		return;
	}

	//BringToFront();

	isMoving = true;
	busyMoving = true;
	clickedPoint = point;
	clickedChannel = channel;
}

void Window::OnTitleReleased(GuiElement *, int channel)
{
	isMoving = false;
	busyMoving = false;
	clickedPoint.x = clickedPoint.y = -1;
	clickedChannel = -1;
}

void Window::SetIcon(GuiImageData *imgData)
{
	icon -> SetImage(imgData);
}

void Window::SetTitle(std::string text)
{
	titleText -> SetText(text.c_str());
}

void Window::SetWidth(int w)
{
	width = w;
}
void Window::SetHeight(int h)
{
	height = h;
}

void Window::SizeChanged()
{
	/** Windowframe tiles **/
	int tilecountH = (width-window_corner_leftDat->GetWidth()*2)/4;
	int tilecountV = (height-window_corner_leftDat->GetHeight()*2)/4;
	window_tile_horizontal_top->SetTileHorizontal(tilecountH);
	window_tile_horizontal_bottom->SetTileHorizontal(tilecountH);
	window_tile_vertical_left->SetTileVertical(tilecountV);
	window_tile_vertical_right->SetTileVertical(tilecountV);

	/** Windowframe positions **/
	window_corner_left_top->SetPosition(0, 0);
	window_corner_right_top->SetPosition(window_corner_left_top->GetLeft()+window_corner_leftDat->GetWidth()+tilecountH*window_tile_horizontalDat->GetWidth(), 0);
	window_corner_left_bottom->SetPosition(0, window_corner_left_top->GetTop()+window_corner_leftDat->GetHeight()+tilecountV*window_tile_verticalDat->GetHeight());
	window_corner_right_bottom->SetPosition(window_corner_left_top->GetLeft()+window_corner_leftDat->GetWidth()+tilecountH*window_tile_horizontalDat->GetWidth(),
											window_corner_left_top->GetTop()+window_corner_leftDat->GetHeight()+tilecountV*window_tile_verticalDat->GetHeight());
	window_tile_horizontal_top->SetPosition(window_corner_left_top->GetLeft()+window_corner_leftDat->GetWidth(), 0);
	window_tile_horizontal_bottom->SetPosition(window_corner_left_top->GetLeft()+window_corner_leftDat->GetWidth(), window_corner_left_bottom->GetTop()+window_corner_left_bottom->GetHeight()-window_tile_horizontalDat->GetHeight());
	window_tile_vertical_left->SetPosition(0, window_corner_right_top->GetTop()+window_corner_right_top->GetHeight());
	window_tile_vertical_right->SetPosition(window_corner_right_top->GetLeft()+window_corner_right_top->GetWidth()-window_tile_vertical_right->GetWidth(), window_corner_right_top->GetTop()+window_corner_right_top->GetHeight());
}

#endif
