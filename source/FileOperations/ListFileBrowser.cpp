/***************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * Original ListBrowser by Tantric (C) 2009
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
#include "ListFileBrowser.hpp"
#include "Memory/Resources.h"
#include "FileStartUp/FileExtensions.h"

/**
 * Constructor for the ListFileBrowser class.
 */
ListFileBrowser::ListFileBrowser(Browser * filebrowser, int w, int h)
	: GuiFileBrowser(filebrowser, w, h)
{
	width = w;
	height = h;
	selectedItem = 0;
	numEntries = 0;
	browser = filebrowser;

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);

	btnSoundOver = Resources::GetSound("button_over.wav");
	btnSoundClick = Resources::GetSound("button_click.wav");

	bgFileSelectionEntry = Resources::GetImageData("bg_browser_selection.png");
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

	scrollbar = new Scrollbar(245);
	scrollbar->SetParent(this);
	scrollbar->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	scrollbar->SetPosition(-10, 5);
	scrollbar->SetScrollSpeed(Settings.ScrollSpeed);
	scrollbar->listChanged.connect(this, &ListFileBrowser::OnListChange);
	scrollbar->SetButtonScroll(WiiControls.OneButtonScroll | ClassicControls.OneButtonScroll << 16);
}

/**
 * Destructor for the ListFileBrowser class.
 */
ListFileBrowser::~ListFileBrowser()
{
	browser = NULL;
	Resources::Remove(btnSoundOver);
	Resources::Remove(btnSoundClick);
	Resources::Remove(bgFileSelectionEntry);
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

	delete trigA;

	delete scrollbar;

	while(fileBtn.size() > 0)
		RemoveButton(0);
}

void ListFileBrowser::AddButton()
{
	int size = fileBtn.size();

	fileBtnText.resize(size+1);
	fileBtnText[size] = new GuiText((char*) NULL, 20, (GXColor){0, 0, 0, 255});
	fileBtnText[size]->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	fileBtnText[size]->SetPosition(32,0);
	fileBtnText[size]->SetMaxWidth(this->GetWidth() - 200, DOTTED);

	fileBtnTextOver.resize(size+1);
	fileBtnTextOver[size] = new GuiText((char*) NULL, 20, (GXColor){0, 0, 0, 255});
	fileBtnTextOver[size]->SetAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
	fileBtnTextOver[size]->SetPosition(32,0);
	fileBtnTextOver[size]->SetMaxWidth(this->GetWidth() - 220, SCROLL_HORIZONTAL);

	fileSizeText.resize(size+1);
	fileSizeText[size] = new GuiText((char*) NULL, 20, (GXColor){0, 0, 0, 255});
	fileSizeText[size]->SetAlignment(ALIGN_RIGHT | ALIGN_MIDDLE);
	fileSizeText[size]->SetPosition(0,0);

	fileSelectionImg.resize(size+1);
	fileSelectionImg[size] = new GuiImage(bgFileSelectionEntry);

	fileBtnIcon.resize(size+1);
	fileBtnIcon[size] = NULL;

	fileBtn.resize(size+1);
	fileBtn[size] = new GuiButton(507,30);
	fileBtn[size]->SetParent(this);
	fileBtn[size]->SetLabel(fileBtnText[size], 0);
	fileBtn[size]->SetLabel(fileSizeText[size], 1);
	fileBtn[size]->SetLabelOver(fileBtnTextOver[size]);
	fileBtn[size]->SetImageOver(fileSelectionImg[size]);
	fileBtn[size]->SetPosition(15,30*size+3);
	fileBtn[size]->SetTrigger(trigA);
	fileBtn[size]->SetSoundClick(btnSoundClick);
	fileBtn[size]->Clicked.connect(this, &ListFileBrowser::OnClicked);
}

void ListFileBrowser::RemoveButton(int i)
{
	if(i < 0 || i >= (int) fileBtn.size())
		return;

	if(fileBtnText[i])
		delete fileBtnText[i];
	if(fileBtnTextOver[i])
		delete fileBtnTextOver[i];
	if(fileSizeText[i])
		delete fileSizeText[i];
	if(fileSelectionImg[i])
		delete fileSelectionImg[i];
	if(fileBtnIcon[i])
		delete fileBtnIcon[i];
	if(fileBtn[i])
		delete fileBtn[i];

	fileBtnText.erase(fileBtnText.begin()+i);
	fileBtnTextOver.erase(fileBtnTextOver.begin()+i);
	fileSizeText.erase(fileSizeText.begin()+i);
	fileSelectionImg.erase(fileSelectionImg.begin()+i);
	fileBtnIcon.erase(fileBtnIcon.begin()+i);
	fileBtn.erase(fileBtn.begin()+i);
}

void ListFileBrowser::SetButton(int i, const char * name, u64 filesize, bool dir, bool enable)
{
	if(i < 0)
		return;

	else if(i >= (int) fileBtn.size())
	{
		AddButton();
	}

	if(!enable)
	{
		fileBtn[i]->SetVisible(false);
		fileBtn[i]->SetState(STATE_DISABLED);
		return;
	}

	if(fileBtnIcon[i])
		delete fileBtnIcon[i];

	fileBtnIcon[i] = GetIconFromExt((name ? strrchr(name, '.') : NULL), dir);

	fileBtnText[i]->SetText(name);
	fileBtnTextOver[i]->SetText(name);

	if(!dir)
	{
		char temp[100];
		if(filesize > GBSIZE)
			sprintf(temp, "%0.2fGB", filesize/GBSIZE);
		else if(filesize > MBSIZE)
			sprintf(temp, "%0.2fMB", filesize/MBSIZE);
		else if(filesize > KBSIZE)
			sprintf(temp, "%0.2fKB", filesize/KBSIZE);
		else
			sprintf(temp, "%iB", (u32) filesize);

		fileSizeText[i]->SetText(temp);
	}
	else
	{
		fileSizeText[i]->SetText((char *) NULL);
	}

	fileBtn[i]->SetIcon(fileBtnIcon[i]);
	fileBtn[i]->SetVisible(true);

	if(fileBtn[i]->GetState() == STATE_DISABLED)
		fileBtn[i]->SetState(STATE_DEFAULT);
}

GuiImage * ListFileBrowser::GetIconFromExt(const char * fileext, bool dir)
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

void ListFileBrowser::OnClicked(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	for(u32 i = 0; i < fileBtn.size(); i++)
	{
		if(fileBtn[i] == sender)
		{
			Clicked(i);
			break;
		}
	}
}

void ListFileBrowser::SetSelected(int i)
{
	if(i < 0 || i >= (int) fileBtn.size())
		return;

	selectedItem = i;
	fileBtn[selectedItem]->SetState(STATE_SELECTED);
}

void ListFileBrowser::ResetState()
{
	state = STATE_DEFAULT;
	stateChan = -1;
	selectedItem = 0;

	for(u32 i = 0; i< fileBtn.size(); i++)
	{
		fileBtn[i]->ResetState();
	}
}

void ListFileBrowser::OnListChange(int selItem, int selIndex)
{
	scrollbar->SetSelectedItem(selItem);
	scrollbar->SetSelectedIndex(selIndex);
	selectedItem = selItem;
	browser->Lock();
	browser->SetPageIndex(selIndex);

	for(int i = 0; i < PAGESIZE; i++)
	{
		if(browser->GetPageIndex()+i < browser->GetEntrieCount())
		{
			SetButton(i, browser->GetItemName(browser->GetPageIndex()+i), browser->GetFilesize(browser->GetPageIndex()+i), browser->IsDir(browser->GetPageIndex()+i), true);
		}
		else
		{
			SetButton(i, NULL, 0, false, false);
		}
	}
	browser->Unlock();
}

/**
 * Draw the button on screen
 */
void ListFileBrowser::Draw()
{
	if(!this->IsVisible())
		return;

	for(u32 i = 0; i < fileBtn.size(); i++)
	{
		fileBtn[i]->Draw();
	}

	scrollbar->Draw();

	UpdateEffects();
}

void ListFileBrowser::Update(GuiTrigger * t)
{
	if(state == STATE_DISABLED || !t)
		return;

	scrollbar->Update(t);

	//! Updating the rest only makes sense with a browser
	if(!browser)
		return;

	browser->UpdateMarker(t);
	if(browser->listChanged())
		OnListChange(selectedItem, browser->GetPageIndex());

	if(numEntries != browser->GetEntrieCount())
	{
		numEntries = browser->GetEntrieCount();
		scrollbar->SetEntrieCount(numEntries);
	}

	for(int i = 0; i < (int) fileBtn.size(); i++)
	{
		if(i != selectedItem && fileBtn[i]->GetState() == STATE_SELECTED)
			fileBtn[i]->ResetState();
		else if(i == selectedItem && fileBtn[i]->GetState() == STATE_DEFAULT)
			fileBtn[selectedItem]->SetState(STATE_SELECTED, t->chan);

		int currChan = t->chan;

		if(t->wpad.ir.valid && !fileBtn[i]->IsInside(t->wpad.ir.x, t->wpad.ir.y))
			t->chan = -1;

		fileBtn[i]->Update(t);
		t->chan = currChan;

		if(fileBtn[i]->GetState() == STATE_SELECTED)
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
				fileBtn[i]->SetState(STATE_SELECTED);
			}
		}
	}

	scrollbar->SetPageSize(fileBtn.size());
	scrollbar->SetRowSize(0);
	scrollbar->SetSelectedItem(selectedItem);
	scrollbar->SetSelectedIndex(browser->GetPageIndex());
}
