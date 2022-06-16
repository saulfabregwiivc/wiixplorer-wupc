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
 * for WiiXplorer 2010
 ***************************************************************************/
#include "ColorSetPrompt.h"
#include "Controls/Application.h"

ColorSetPrompt::ColorSetPrompt(const char * title, GXColor * c, int pos)
	: PromptWindow(title, 0, tr("Done"), tr("Cancel"))
{
	ColorPos = pos;
	color = c;

	for(int i = 0; i < 4; i++)
		OrigColor[i] = color[i];

	arrowOption.ClickedLeft.connect(this, &ColorSetPrompt::OnOptionLeftClick);
	arrowOption.ClickedRight.connect(this, &ColorSetPrompt::OnOptionRightClick);
	arrowOption.ClickedButton.connect(this, &ColorSetPrompt::OnOptionButtonClick);

	int PositionX = 102;

	arrowOption.AddOption(tr("Red"), PositionX, 130);
	arrowOption.AddOption(tr("Green"), PositionX+130, 130);
	arrowOption.AddOption(tr("Blue"), PositionX+260, 130);
	arrowOption.SetScale(1.3);

	UpdateOptionValues();

	Append(&arrowOption);

	this->DimBackground(true);
}

void ColorSetPrompt::Show(const char * title, GXColor * c, int pos)
{
	int userChoice = -1;
	ColorSetPrompt *ColorPrompt = new ColorSetPrompt(title, c, pos);
	Application::Instance()->SetUpdateOnly(ColorPrompt);
	Application::Instance()->Append(ColorPrompt);

	while(userChoice < 0)
	{
		userChoice = ColorPrompt->GetChoice();
		Application::Instance()->updateEvents();
	}

	if(userChoice == 0)
	{
		for(int i = 0; i < 4; i++)
			ColorPrompt->color[i] = ColorPrompt->OrigColor[i];
	}

	Application::Instance()->PushForDelete(ColorPrompt);
}

void ColorSetPrompt::UpdateOptionValues()
{
	arrowOption.SetOptionValue(0, fmt("%i", color[ColorPos].r));
	arrowOption.SetOptionValue(1, fmt("%i", color[ColorPos].g));
	arrowOption.SetOptionValue(2, fmt("%i", color[ColorPos].b));
}

void ColorSetPrompt::OnOptionLeftClick(GuiElement *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	sender->ResetState();

	for(int i = 0; i < arrowOption.GetOptionCount(); i++)
	{
		if(sender == arrowOption.GetButtonLeft(i))
		{
			if(i == 0)
			{
				--color[ColorPos].r;
			}
			else if(i == 1)
			{
				--color[ColorPos].g;
			}
			else if(i == 2)
			{
				--color[ColorPos].b;
			}
			UpdateOptionValues();
			break;
		}
	}
}

void ColorSetPrompt::OnOptionRightClick(GuiElement *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	sender->ResetState();

	for(int i = 0; i < arrowOption.GetOptionCount(); i++)
	{
		if(sender == arrowOption.GetButtonRight(i))
		{
			if(i == 0)
			{
				color[ColorPos].r = (color[ColorPos].r+1) % 255;
			}
			else if(i == 1)
			{
				color[ColorPos].g = (color[ColorPos].g+1) % 255;
			}
			else if(i == 2)
			{
				color[ColorPos].b = (color[ColorPos].b+1) % 255;
			}
			UpdateOptionValues();
			break;
		}
	}
}

void ColorSetPrompt::OnOptionButtonClick(GuiElement *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	sender->ResetState();

	for(int i = 0; i < arrowOption.GetOptionCount(); i++)
	{
		if(sender == arrowOption.GetButton(i))
		{
			char Text[150];
			if(i == 0)
			{
				sprintf(Text, "%i", color[ColorPos].r);
			}
			else if(i == 1)
			{
				sprintf(Text, "%i", color[ColorPos].g);
			}
			else if(i == 2)
			{
				sprintf(Text, "%i", color[ColorPos].b);
			}

			if(OnScreenKeyboard(Text, 150))
			{
				if(i == 0)
					color[ColorPos].r = (u8) atoi(Text);
				else if(i == 1)
					color[ColorPos].g = (u8) atoi(Text);
				else if(i == 2)
					color[ColorPos].b = (u8) atoi(Text);
			}

			UpdateOptionValues();
			break;
		}
	}

}
