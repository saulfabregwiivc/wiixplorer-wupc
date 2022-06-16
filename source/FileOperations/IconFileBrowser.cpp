/****************************************************************************
 * Copyright (C) 2010-2011 Dimok
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
#include "IconFileBrowser.hpp"
#include "Memory/Resources.h"
#include "FileStartUp/FileExtensions.h"

/**
 * Constructor for the IconFileBrowser class.
 */
IconFileBrowser::IconFileBrowser(Browser * filebrowser, int w, int h)
	: GuiFileBrowser(filebrowser, w, h)
{
	width = w;
	height = h;
	selectedItem = 0;
	browser = filebrowser;

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	btnSoundOver = Resources::GetSound("button_over.wav");
	btnSoundClick = Resources::GetSound("button_click.wav");

	bgFileSelection = Resources::GetImageData("deviceselection.png");
	fileArchives = Resources::GetImageData("icon_archives.png");
	fileDefault = Resources::GetImageData("icon_default.png");
	fileFolder = Resources::GetImageData("icon_folder.png");
	fileGFX = Resources::GetImageData("icon_gfx.png");
	filePLS = Resources::GetImageData("icon_pls.png");
	fileSFX = Resources::GetImageData("icon_sfx.png");
	fileTXT = Resources::GetImageData("icon_txt.png");
	fileXML = Resources::GetImageData("icon_xml.png");
	fileVID = Resources::GetImageData("icon_video.png");
	filePDF = Resources::GetImageData("icon_pdf.png");

	scrollbar = new Scrollbar(245, Scrollbar::ICONMODE);
	scrollbar->SetParent(this);
	scrollbar->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	scrollbar->SetPosition(-10, 5);
	scrollbar->SetScrollSpeed(Settings.ScrollSpeed);
	scrollbar->listChanged.connect(this, &IconFileBrowser::OnListChange);
	scrollbar->SetButtonScroll(WiiControls.OneButtonScroll | ClassicControls.OneButtonScroll << 16);

	RowSize = (width-25)/(fileDefault->GetWidth()+50);
	PageSize = RowSize*height/(fileDefault->GetHeight()+50);
}

/**
 * Destructor for the IconFileBrowser class.
 */
IconFileBrowser::~IconFileBrowser()
{
	browser = NULL;
	Resources::Remove(btnSoundOver);
	Resources::Remove(btnSoundClick);
	Resources::Remove(bgFileSelection);
	Resources::Remove(fileArchives);
	Resources::Remove(fileDefault);
	Resources::Remove(fileFolder);
	Resources::Remove(fileGFX);
	Resources::Remove(filePLS);
	Resources::Remove(fileSFX);
	Resources::Remove(fileTXT);
	Resources::Remove(fileXML);
	Resources::Remove(fileVID);
	Resources::Remove(filePDF);

	delete scrollbar;

	delete trigA;

	for(u32 i = 0; i < Buttons.size(); i++)
	{
		if(Buttons[i])
			delete Buttons[i];
		if(ButtonText[i])
			delete ButtonText[i];
		if(FileSelectionImg[i])
			delete FileSelectionImg[i];
		if(ButtonImg[i])
			delete ButtonImg[i];
		if(Tooltip[i])
			delete Tooltip[i];
	}

	Buttons.clear();
	FileSelectionImg.clear();
	ButtonText.clear();
	ButtonImg.clear();
	Tooltip.clear();
}

GuiImage * IconFileBrowser::GetIconFromExt(const char * fileext, bool dir)
{
	if(dir)
		return (new GuiImage(fileFolder));

	if(fileext)
	{
		if(Settings.FileExtensions.CompareImage(fileext) == 0)
		{
			return (new GuiImage(fileGFX));
		}
		else if(Settings.FileExtensions.CompareAudio(fileext) == 0)
		{
			return (new GuiImage(fileSFX));
		}
		else if(strcasecmp(fileext, ".pls") == 0 || strcasecmp(fileext, ".m3u") == 0)
		{
			return (new GuiImage(filePLS));
		}
		else if(strcasecmp(fileext, ".txt") == 0)
		{
			return (new GuiImage(fileTXT));
		}
		else if(strcasecmp(fileext, ".xml") == 0)
		{
			return (new GuiImage(fileXML));
		}
		else if(Settings.FileExtensions.CompareWiiBinary(fileext) == 0 || Settings.FileExtensions.CompareArchive(fileext) == 0)
		{
			return (new GuiImage(fileArchives));
		}
		else if(Settings.FileExtensions.CompareVideo(fileext) == 0)
		{
			return (new GuiImage(fileVID));
		}
		else if(Settings.FileExtensions.ComparePDF(fileext) == 0)
		{
			return (new GuiImage(filePDF));
		}
	}

	return (new GuiImage(fileDefault));
}

void IconFileBrowser::AddButton()
{
	GuiImage * BtnImg = NULL;
	ButtonImg.push_back(BtnImg);

	GuiText * BtnTxt = new GuiText((char *) NULL, 14, (GXColor){0, 0, 0, 255});
	BtnTxt->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	BtnTxt->SetPosition(0, -10);
	BtnTxt->SetLinesToDraw(2);
	BtnTxt->SetMaxWidth(fileDefault->GetWidth()+38, WRAP);
	ButtonText.push_back(BtnTxt);

	GuiImage * Marker = new GuiImage(bgFileSelection);
	Marker->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	Marker->SetPosition(0, -17);
	FileSelectionImg.push_back(Marker);

	GuiTooltip * tmpToolTip = new GuiTooltip((char *) NULL);
	tmpToolTip->SetPosition(0, 0);
	Tooltip.push_back(tmpToolTip);

	GuiButton * Btn = new GuiButton(fileDefault->GetWidth()+40, fileDefault->GetHeight()+32);
	Btn->SetParent(this);
	Btn->SetLabel(BtnTxt);
	Btn->SetIcon(BtnImg);
	Btn->SetImageOver(Marker);
	Btn->SetTrigger(trigA);
	Btn->SetSoundClick(btnSoundClick);
	Btn->SetTooltip(tmpToolTip);
	Btn->Clicked.connect(this, &IconFileBrowser::OnButtonClicked);
	Buttons.push_back(Btn);
}

void IconFileBrowser::SetButton(int i, const char * name, bool dir, bool enable, int x, int y)
{
	if(i < 0) {
		return;
	}

	while(i >= (int) Buttons.size())
		AddButton();

	if(!enable)
	{
		Buttons[i]->SetVisible(false);
		Buttons[i]->SetState(STATE_DISABLED);
		return;
	}

	if(ButtonImg[i])
		delete ButtonImg[i];

	ButtonImg[i] = GetIconFromExt((name ? strrchr(name, '.') : NULL), dir);
	ButtonImg[i]->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	ButtonText[i]->SetText(name);
	Tooltip[i]->SetText(name);
	if(40+x+Tooltip[i]->GetWidth() > width)
	{
		Tooltip[i]->SetPosition(width-(40+x+Tooltip[i]->GetWidth()), -30);
	}
	else
	{
		Tooltip[i]->SetPosition(40, -30);
	}

	if(Tooltip[i]->GetLeft() < 0)
	{
		Tooltip[i]->SetPosition(10-x-GetLeft(), -30);
	}

	Buttons[i]->SetIcon(ButtonImg[i]);
	Buttons[i]->SetVisible(true);
	Buttons[i]->SetPosition(x, y);

	if(Buttons[i]->GetState() == STATE_DISABLED)
		Buttons[i]->SetState(STATE_DEFAULT);
}

void IconFileBrowser::RemoveButton(int i)
{
	if(i < 0 || i >= (int) Buttons.size())
		return;

	if(ButtonImg.at(i))
		delete ButtonImg.at(i);
	ButtonImg.erase(ButtonImg.begin()+i);

	if(ButtonText.at(i))
		delete ButtonText.at(i);
	ButtonText.erase(ButtonText.begin()+i);

	if(FileSelectionImg.at(i))
		delete FileSelectionImg.at(i);
	FileSelectionImg.erase(FileSelectionImg.begin()+i);

	if(Buttons.at(i))
		delete Buttons.at(i);
	Buttons.erase(Buttons.begin()+i);
}

void IconFileBrowser::OnButtonClicked(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	for(u32 i = 0; i < Buttons.size(); i++)
	{
		if(Buttons[i] == sender)
		{
			Clicked(i);
			break;
		}
	}
}

void IconFileBrowser::SetSelected(int i)
{
	if(i < 0 || i >= PAGESIZE)
		return;

	selectedItem = i;
	Buttons[selectedItem]->SetState(STATE_SELECTED);
}

void IconFileBrowser::ResetState()
{
	state = STATE_DEFAULT;
	stateChan = -1;
	selectedItem = 0;

	for(u32 i=0; i < Buttons.size(); i++)
	{
		Buttons[i]->ResetState();
	}
}

void IconFileBrowser::OnListChange(int selItem, int selIndex)
{
	scrollbar->SetSelectedItem(selItem);
	scrollbar->SetSelectedIndex(selIndex);
	selectedItem = selItem;
	browser->Lock();
	browser->SetPageIndex(selIndex);

	u16 x = 25, y = 7;

	for(int i = 0; i < PageSize; i++)
	{
		if(browser->GetPageIndex()+i < browser->GetEntrieCount())
		{
			const char * name = browser->GetItemName(browser->GetPageIndex()+i);
			bool dir = browser->IsDir(browser->GetPageIndex()+i);

			SetButton(i, name, dir, true, x, y);
		}
		else
		{
			SetButton(i, NULL, false, false, 0, 0);
		}
		x += fileDefault->GetWidth()+50;
		if(x >= (u32) width-80)
		{
			x = 25;
			y += fileDefault->GetHeight()+50;
		}
	}
	browser->Unlock();
}

/**
 * Draw the button on screen
 */
void IconFileBrowser::Draw()
{
	if(!IsVisible())
		return;

	scrollbar->Draw();

	for(u32 i = 0; i < Buttons.size(); i++)
		Buttons[i]->Draw();

	// Draw tooltips on top of buttons
	for(u32 i = 0; i < Buttons.size(); i++)
		Tooltip[i]->Draw();

	UpdateEffects();
}

void IconFileBrowser::Update(GuiTrigger * t)
{
	if(state == STATE_DISABLED || !t)
		return;

	scrollbar->Update(t);

	//! Updating the rest only makes sense with a browser
	if(!browser)
		return;

	if(browser->listChanged())
		OnListChange(selectedItem, browser->GetPageIndex());

	browser->UpdateMarker(t);

	for(int i = 0; i < PageSize && i < (int) Buttons.size(); i++)
	{
		if(i != selectedItem && Buttons[i]->GetState() == STATE_SELECTED)
			Buttons[i]->ResetState();
		else if(i == selectedItem && Buttons[i]->GetState() == STATE_DEFAULT)
			Buttons[selectedItem]->SetState(STATE_SELECTED, t->chan);

		int currChan = t->chan;

		if(t->wpad.ir.valid && !Buttons[i]->IsInside(t->wpad.ir.x, t->wpad.ir.y))
			t->chan = -1;

		Buttons[i]->Update(t);
		t->chan = currChan;

		if(Buttons[i]->GetState() == STATE_SELECTED)
		{
			selectedItem = i;
			browser->SetSelectedIndex(browser->GetPageIndex() + i);
		}

		ItemMarker * IMarker = browser->GetItemMarker();
		int itemCount = IMarker->GetItemcount();

		for(int n = 0; n < itemCount; n++)
		{
			if(browser->GetPageIndex() + i == IMarker->GetItemIndex(n))
			{
				Buttons[i]->SetState(STATE_SELECTED);
				break;
			}
		}
	}

	scrollbar->SetEntrieCount(browser->GetEntrieCount());
	scrollbar->SetPageSize(PageSize);
	scrollbar->SetRowSize(RowSize);
	scrollbar->SetSelectedItem(selectedItem);
	scrollbar->SetSelectedIndex(browser->GetPageIndex());
}
