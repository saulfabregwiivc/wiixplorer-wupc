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
#include "gui_optionbrowser.h"
#include "Memory/Resources.h"

#define BROWSERSIZE		8

/**
 * Constructor for the GuiOptionBrowser class.
 */
GuiOptionBrowser::GuiOptionBrowser(int w, int h, OptionList * l)
{
	width = w;
	height = h;
	options = l;
	coL2 = 50;
	selectable = true;
	listOffset = 0;
	listChanged = true; // trigger an initial list update
	selectedItem = 0;

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	btnSoundClick = Resources::GetSound("button_click.wav");

	bgOptions = Resources::GetImageData("bg_browser.png");
	bgOptionsImg = new GuiImage(bgOptions);
	bgOptionsImg->SetParent(this);
	bgOptionsImg->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);

	bgOptionsEntry = Resources::GetImageData("bg_browser_selection.png");

	scrollbar = new Scrollbar(245);
	scrollbar->SetParent(this);
	scrollbar->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	scrollbar->SetPosition(-10, 5);
	scrollbar->SetScrollSpeed(Settings.ScrollSpeed);
	scrollbar->listChanged.connect(this, &GuiOptionBrowser::OnListChange);
	scrollbar->SetButtonScroll(WiiControls.OneButtonScroll | ClassicControls.OneButtonScroll << 16);

	for(int i = 0; i < PAGESIZE; i++)
	{
		optionTxt[i] = new GuiText((char*) NULL, 20, (GXColor){0, 0, 0, 0xff});
		optionTxt[i]->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
		optionTxt[i]->SetPosition(15,0);

		optionVal[i] = new GuiText((char*) NULL, 20, (GXColor){0, 0, 0, 0xff});
		optionVal[i]->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
		optionVal[i]->SetPosition(250,0);
		optionVal[i]->SetMaxWidth(width-optionTxt[i]->GetLeft()-50,DOTTED);

		optionBg[i] = new GuiImage(bgOptionsEntry);

		optionBtn[i] = new GuiButton(512,30);
		optionBtn[i]->SetParent(this);
		optionBtn[i]->SetState(STATE_DISABLED);
		optionBtn[i]->SetLabel(optionTxt[i], 0);
		optionBtn[i]->SetLabel(optionVal[i], 1);
		optionBtn[i]->SetImageOver(optionBg[i]);
		optionBtn[i]->SetPosition(2,30*i+3);
		optionBtn[i]->SetTrigger(trigA);
		optionBtn[i]->SetSoundClick(btnSoundClick);
		optionBtn[i]->Clicked.connect(this, &GuiOptionBrowser::OnButtonClick);
	}
}

/**
 * Destructor for the GuiOptionBrowser class.
 */
GuiOptionBrowser::~GuiOptionBrowser()
{
	delete scrollbar;

	delete bgOptionsImg;

	Resources::Remove(bgOptions);
	Resources::Remove(bgOptionsEntry);
	Resources::Remove(btnSoundClick);

	delete trigA;

	for(int i=0; i<PAGESIZE; i++)
	{
		delete optionTxt[i];
		delete optionVal[i];
		delete optionBg[i];
		delete optionBtn[i];
	}
}

void GuiOptionBrowser::SetCol2Position(int x)
{
	for(int i=0; i<PAGESIZE; i++)
		optionVal[i]->SetPosition(x,0);
}

void GuiOptionBrowser::OnButtonClick(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	for(int i = 0; i < PAGESIZE; i++)
	{
		if(sender == optionBtn[i])
		{
			Clicked(this, listOffset+i);
			return;
		}
	}
}

/**
 * Draw the button on screen
 */
void GuiOptionBrowser::Draw()
{
	if(!this->IsVisible())
		return;

	bgOptionsImg->Draw();

	for(int i = 0; i < PAGESIZE && listOffset+i < options->GetLength(); i++)
	{
		optionBtn[i]->Draw();
	}

	scrollbar->Draw();

	this->UpdateEffects();
}

void GuiOptionBrowser::OnListChange(int selItem, int selIndex)
{
	selectedItem = selItem;
	listOffset = selIndex;
	int maxNameWidth = 0;

	for(int i = 0; i < PAGESIZE; i++)
	{
		if(listOffset+i < options->GetLength())
		{
			if(optionBtn[i]->GetState() == STATE_DISABLED)
			{
				optionBtn[i]->SetVisible(true);
				optionBtn[i]->SetState(STATE_DEFAULT);
			}

			optionTxt[i]->SetText(options->GetName(listOffset+i));
			optionVal[i]->SetText(options->GetValue(listOffset+i));

			if(maxNameWidth < optionTxt[i]->GetTextWidth())
				maxNameWidth = optionTxt[i]->GetTextWidth();

			if(coL2 < (24+maxNameWidth+16))
				coL2 = 24+maxNameWidth+16;
		}
		else
		{
			optionBtn[i]->SetVisible(false);
			optionBtn[i]->SetState(STATE_DISABLED);
		}
	}

	for(int i = 0; i < PAGESIZE; i++)
	{
		optionVal[i]->SetPosition(coL2,0);
		optionVal[i]->SetMaxWidth(width-coL2-50, DOTTED);
	}
}

void GuiOptionBrowser::Update(GuiTrigger * t)
{
	if(state == STATE_DISABLED || !t)
		return;

	scrollbar->Update(t);

	if(options->IsChanged())
		OnListChange(selectedItem, listOffset);

	for(int i=0; i<PAGESIZE; i++)
	{
		if(i != selectedItem && optionBtn[i]->GetState() == STATE_SELECTED)
		{
			optionBtn[i]->ResetState();
			optionVal[i]->SetMaxWidth(width-coL2-50, DOTTED);
		}
		else if(i == selectedItem && optionBtn[i]->GetState() == STATE_DEFAULT)
		{
			optionBtn[selectedItem]->SetState(STATE_SELECTED, t->chan);
			optionVal[selectedItem]->SetMaxWidth(width-coL2-60, SCROLL_HORIZONTAL);
		}

		int currChan = t->chan;

		if(t->wpad.ir.valid && !optionBtn[i]->IsInside(t->wpad.ir.x, t->wpad.ir.y))
			t->chan = -1;

		optionBtn[i]->Update(t);
		t->chan = currChan;

		if(i != selectedItem && optionBtn[i]->GetState() == STATE_SELECTED)
		{
			selectedItem = i;
			optionVal[selectedItem]->SetMaxWidth(width-coL2-60, SCROLL_HORIZONTAL);
		}
	}

	scrollbar->SetEntrieCount(options->GetLength());
	scrollbar->SetPageSize(PAGESIZE);
	scrollbar->SetRowSize(0);
	scrollbar->SetSelectedItem(selectedItem);
	scrollbar->SetSelectedIndex(listOffset);
}
