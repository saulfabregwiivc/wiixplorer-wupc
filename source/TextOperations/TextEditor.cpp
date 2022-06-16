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
#include <unistd.h>
#include "TextOperations/TextEditor.h"
#include "Controls/Taskbar.h"
#include "FileOperations/fileops.h"
#include "FileOperations/FileLoadTask.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/ProgressWindow.h"
#include "Controls/Application.h"
#include "Memory/Resources.h"
#include "FreeTypeGX.h"
#include "input.h"

#define FONTSIZE	18

/**
 * Constructor for the TextEditor class.
 */
TextEditor::TextEditor(const std::string &path)
	: GuiWindow(screenwidth - 100, 400)
{
	ExitEditor = false;
	FileEdited = false;
	filesize = (u32) FileSize(path.c_str());
	textStartWidth = 0;

	filepath = path;
	char * filename = strrchr(filepath.c_str(), '/')+1;

	trigA = new GuiTrigger;
	trigA->SetSimpleTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigHeldA = new GuiTrigger;
	trigHeldA->SetHeldTrigger(-1, WiiControls.ClickButton | ClassicControls.ClickButton << 16, GCControls.ClickButton);
	trigB = new GuiTrigger;
	trigB->SetButtonOnlyTrigger(-1, WiiControls.BackButton | ClassicControls.BackButton << 16, GCControls.BackButton);
	trigPlus = new GuiTrigger;
	trigPlus->SetButtonOnlyTrigger(-1, WiiControls.EditTextLine | ClassicControls.EditTextLine << 16, GCControls.EditTextLine);

	btnSoundOver = Resources::GetSound("button_over.wav");
	btnSoundClick = Resources::GetSound("button_click.wav");

	closeImgData = Resources::GetImageData("close.png");
	closeImgOverData = Resources::GetImageData("close_over.png");
	closeImg = new GuiImage(closeImgData);
	closeImgOver = new GuiImage(closeImgOverData);
	maximizeImgData = Resources::GetImageData("maximize_dis.png");
	maximizeImg = new GuiImage(maximizeImgData);
	minimizeImgData = Resources::GetImageData("minimize_dis.png");
	minimizeImg = new GuiImage(minimizeImgData);

	textBgImg = new GuiFrameImage(width - 80, height - 104);
	textBgImg->SetPosition(25, 55);

	linestodraw = textBgImg->GetHeight() / 24 - 1;
	displayLineWidth = textBgImg->GetWidth() - 40;

	scrollbar = new Scrollbar(height - 70, Scrollbar::LISTMODE);
	scrollbar->SetParent(this);
	scrollbar->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	scrollbar->SetPosition(-5, 50);
	scrollbar->SetScrollSpeed(Settings.ScrollSpeed);
	scrollbar->SetPageSize(linestodraw);
	scrollbar->SetRowSize(0);
	scrollbar->SetSelectedItem(0);
	scrollbar->SetSelectedIndex(0);
	scrollbar->listChanged.connect(this, &TextEditor::OnListChange);

	horScrollbar = new HorizontalScrollbar(width - 70, Scrollbar::LISTMODE);
	horScrollbar->SetParent(this);
	horScrollbar->SetAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
	horScrollbar->SetPosition(25, -10);
	horScrollbar->SetScrollSpeed(Settings.ScrollSpeed);
	horScrollbar->SetDPadControl(false);
	horScrollbar->listChanged.connect(this, &TextEditor::OnHorScrollChange);

	closeBtn = new GuiButton(closeImg->GetWidth(), closeImg->GetHeight());
	closeBtn->SetImage(closeImg);
	closeBtn->SetImageOver(closeImgOver);
	closeBtn->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	closeBtn->SetPosition(-20, 12);
	closeBtn->SetSoundOver(btnSoundOver);
	closeBtn->SetSoundClick(btnSoundClick);
	closeBtn->SetTrigger(trigA);
	closeBtn->SetTrigger(trigB);
	closeBtn->SetEffectGrow();
	closeBtn->Clicked.connect(this, &TextEditor::OnButtonClick);

	maximizeBtn = new GuiButton(maximizeImg->GetWidth(), maximizeImg->GetHeight());
	maximizeBtn->SetImage(maximizeImg);
	maximizeBtn->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	maximizeBtn->SetPosition(-50, 12);
	maximizeBtn->SetSoundClick(btnSoundClick);

	minimizeBtn = new GuiButton(minimizeImg->GetWidth(), minimizeImg->GetHeight());
	minimizeBtn->SetImage(minimizeImg);
	minimizeBtn->SetAlignment(ALIGN_RIGHT | ALIGN_TOP);
	minimizeBtn->SetPosition(-80, 12);
	minimizeBtn->SetSoundClick(btnSoundClick);

	filenameTxt = new GuiText(filename, 22, (GXColor){0, 0, 0, 255});
	filenameTxt->SetAlignment(ALIGN_CENTER | ALIGN_TOP);
	filenameTxt->SetPosition(-30,15);
	filenameTxt->SetMaxWidth(340, DOTTED);

	MainFileTxt = new GuiLongText((char*)NULL, FONTSIZE, (GXColor){0, 0, 0, 255});
	MainFileTxt->SetAlignment(ALIGN_LEFT | ALIGN_TOP);
	MainFileTxt->SetPosition(0, 0);
	MainFileTxt->SetLinesToDraw(linestodraw);
	MainFileTxt->SetMaxWidth(displayLineWidth);

	TextPointerBtn = new TextPointer(MainFileTxt, linestodraw);
	TextPointerBtn->SetPosition(43, 65);
	TextPointerBtn->SetHoldable(true);
	TextPointerBtn->SetTrigger(trigHeldA);
	TextPointerBtn->PositionChanged(0, 0, 0);
	TextPointerBtn->Held.connect(this, &TextEditor::OnPointerHeld);

	PlusBtn = new GuiButton(0, 0);
	PlusBtn->SetTrigger(trigPlus);
	PlusBtn->SetSoundClick(btnSoundClick);
	PlusBtn->Clicked.connect(this, &TextEditor::OnButtonClick);

	// GuiWindow protected variable
	frameImage.SetColorUpperQuadUpper((GXColor) {0xDC, 0xDD, 0xDE, 0xFF});
	frameImage.SetColorMainQuadUpper((GXColor) {0xC5, 0xC9, 0xCF, 0xFF});
	frameImage.SetColorMainQuadLower((GXColor) {0xA3, 0xA8, 0xB0, 0xFF});
	frameImage.SetColorSideQuadUpper((GXColor) {0xA6, 0xA9, 0xAF, 0xFF});
	frameImage.SetColorSideQuadLower((GXColor) {0x8C, 0x90, 0x96, 0xFF});

	keyboard = new GuiKeyboard();
	keyboard->SetAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
	keyboard->SetState(STATE_DISABLED);
	keyboard->SetVisible(false);
	keyboard->EffectFinished.connect(this, &TextEditor::OnKeyboardEffectFinished);
	keyboard->keyPressed.connect(this, &TextEditor::OnKeyboardKeyPressed);

	this->Append(PlusBtn);
	this->Append(textBgImg);
	this->Append(filenameTxt);
	this->Append(TextPointerBtn);
	this->Append(scrollbar);
	this->Append(horScrollbar);
	this->Append(closeBtn);
	this->Append(maximizeBtn);
	this->Append(minimizeBtn);
	this->Append(keyboard);
}

/**
 * Destructor for the TextEditor class.
 */
TextEditor::~TextEditor()
{
	RemoveAll();

	delete scrollbar;
	delete horScrollbar;
	delete textBgImg;
	delete keyboard;

	/** Buttons **/
	delete maximizeBtn;
	delete minimizeBtn;
	delete closeBtn;
	delete TextPointerBtn;

	/** Images **/
	delete closeImg;
	delete closeImgOver;
	delete maximizeImg;
	delete minimizeImg;

	/** ImageDatas **/
	Resources::Remove(closeImgData);
	Resources::Remove(closeImgOverData);
	Resources::Remove(maximizeImgData);
	Resources::Remove(minimizeImgData);

	/** Sounds **/
	Resources::Remove(btnSoundOver);
	Resources::Remove(btnSoundClick);

	/** Triggers **/
	delete trigHeldA;
	delete trigA;
	delete trigB;
	delete PlusBtn;

	/** Texts **/
	delete filenameTxt;
	delete MainFileTxt;
}

void TextEditor::SetText(const wchar_t *intext)
{
	if(TextPointerBtn)
	{
		delete TextPointerBtn;
		TextPointerBtn = NULL;
	}

	MainFileTxt->SetText(intext);

	TextPointerBtn = new TextPointer(MainFileTxt, 0);
	TextPointerBtn->SetPosition(43, 75);
	TextPointerBtn->SetHoldable(true);
	TextPointerBtn->SetTrigger(trigHeldA);
	TextPointerBtn->Held.connect(this, &TextEditor::OnPointerHeld);
}

void TextEditor::WriteTextFile(const std::string &path)
{
	FILE * f = fopen(path.c_str(), "wb");
	if(!f)
	{
		ShowError(tr("Cannot write to the file."));
		return;
	}

	const std::string &FullText = MainFileTxt->toUTF8();

	fwrite(FullText.c_str(), 1, strlen(FullText.c_str())+1, f);

	fclose(f);
}

void TextEditor::OnButtonClick(GuiButton *sender, int pointer UNUSED, const POINT &p UNUSED)
{
	if(sender == closeBtn)
	{
		int choice = 1;
		if(FileEdited)
		{
			choice = WindowPrompt(tr("File was edited."), tr("Do you want to save changes?"), tr("Yes"), tr("No"), tr("Cancel"));
			if(choice == 1)
				WriteTextFile(filepath);
		}
		if(choice)
		{
			SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_OUT, 50);
			Application::Instance()->PushForDelete(this);
		}
	}
	else if(sender == PlusBtn)
	{
		if(!keyboard->IsVisible())
		{
			keyboard->SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_IN, 50);
			keyboard->SetVisible(true);
			keyboard->SetState(STATE_DEFAULT);
			while(TextPointerBtn->GetCurrentLine() >= linestodraw/2) {
				scrollbar->ScrollOneDown();
				TextPointerBtn->SetCurrentLine(TextPointerBtn->GetCurrentLine()-1);
			}
			scrollbar->listChanged(scrollbar->GetSelectedItem(), scrollbar->GetSelectedIndex());

			TextPointerBtn->SetState(STATE_DISABLED);
			scrollbar->SetDPadControl(false);
		}
		else
		{
			keyboard->SetEffect(EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_OUT, 50);
			keyboard->SetState(STATE_DISABLED);
			TextPointerBtn->SetState(STATE_DEFAULT);
			scrollbar->SetDPadControl(true);
		}
	}
}

void TextEditor::OnKeyboardEffectFinished(GuiElement *e UNUSED)
{
	if(keyboard->GetEffect() == (EFFECT_SLIDE_BOTTOM | EFFECT_SLIDE_OUT))
		keyboard->SetVisible(false);
}

void TextEditor::OnKeyboardKeyPressed(wchar_t charCode)
{
	//! for usb keyboard switch '\r' to '\n'
	if(charCode == L'\r')
		charCode = L'\n';

	else if(((charCode >> 8) == 0xF2) && (charCode & 0xFF) < 0x80) {
		// this is usually a numpad
		charCode = charCode & 0xFF;
	}
	else if(  (charCode >= 0xD800)									// this is usually a normal character
			&& !(charCode >= KS_KP_Left && charCode <= KS_KP_Down)	// up/down/left/right numpad
			&& !(charCode >= KS_Up && charCode <= KS_Right)			// up/down/left/right arrows
			&& (charCode != KS_Home && charCode != KS_End)			// HOME / END
			&& (charCode != KS_Prior && charCode != KS_Next))		// PAGE UP / PAGE DOWN
	{
		//! skip unknown characters
		return;
	}

	// control character UP
	if(charCode == KS_Up || charCode == KS_KP_Up)
	{
		if(TextPointerBtn->GetCurrentLine() == 0) {
			scrollbar->ScrollOneUp();
			scrollbar->listChanged(scrollbar->GetSelectedItem(), scrollbar->GetSelectedIndex());
		}
		else
			TextPointerBtn->SetCurrentLine(TextPointerBtn->GetCurrentLine()-1);
		// update GUI positions
		TextPointerBtn->Refresh();
		if(!TextPointerBtn->IsPointerVisible())
			horScrollbar->SetSelectedItem((textStartWidth + TextPointerBtn->GetPointerPosX() - width/2) >> 4);
	}

	// control character DOWN
	else if(charCode == KS_Down || charCode == KS_KP_Down)
	{
		if(TextPointerBtn->GetCurrentLine() + 1 >= linestodraw) {
			scrollbar->ScrollOneDown();
			scrollbar->listChanged(scrollbar->GetSelectedItem(), scrollbar->GetSelectedIndex());
		}
		else if(MainFileTxt->GetCurrPos() + TextPointerBtn->GetCurrentLine() + 1 < MainFileTxt->GetTotalLinesCount())
			TextPointerBtn->SetCurrentLine(TextPointerBtn->GetCurrentLine()+1);
		// update GUI positions
		TextPointerBtn->Refresh();
		if(!TextPointerBtn->IsPointerVisible())
			horScrollbar->SetSelectedItem((textStartWidth + TextPointerBtn->GetPointerPosX() - width/2) >> 4);
	}

	// control character LEFT
	else if(charCode == KS_Left || charCode == KS_KP_Left)
	{
		// update GUI positions
		TextPointerBtn->SetLetterPosition(TextPointerBtn->GetCurrentLetter()-1);
		if(!TextPointerBtn->IsPointerVisible())
			horScrollbar->SetSelectedItem((textStartWidth + TextPointerBtn->GetPointerPosX() - width/2) >> 4);
	}

	// control character RIGHT
	else if(charCode == KS_Right || charCode == KS_KP_Right)
	{
		// update GUI positions
		TextPointerBtn->SetLetterPosition(TextPointerBtn->GetCurrentLetter()+1);
		if(!TextPointerBtn->IsPointerVisible())
			horScrollbar->SetSelectedItem((textStartWidth + TextPointerBtn->GetPointerPosX() - width/2) >> 4);
	}

	// control character PAGE UP
	else if(charCode == KS_Prior) {

		for(int i = 0; i < linestodraw; i++)
			scrollbar->ScrollOneUp();
		scrollbar->listChanged(scrollbar->GetSelectedItem(), scrollbar->GetSelectedIndex());
		// update GUI positions
		TextPointerBtn->Refresh();
		if(!TextPointerBtn->IsPointerVisible())
			horScrollbar->SetSelectedItem((textStartWidth + TextPointerBtn->GetPointerPosX() - width/2) >> 4);
	}

	// control character PAGE DOWN
	else if(charCode == KS_Next) {

		for(int i = 0; i < linestodraw; i++)
			scrollbar->ScrollOneDown();
		scrollbar->listChanged(scrollbar->GetSelectedItem(), scrollbar->GetSelectedIndex());
		// update GUI positions
		TextPointerBtn->Refresh();
		if(!TextPointerBtn->IsPointerVisible())
			horScrollbar->SetSelectedItem((textStartWidth + TextPointerBtn->GetPointerPosX() - width/2) >> 4);
	}

	// control character HOME
	else if(charCode == KS_Home) {
		// update GUI positions
		TextPointerBtn->SetLetterPosition(0);
		if(!TextPointerBtn->IsPointerVisible())
			horScrollbar->SetSelectedItem((textStartWidth + TextPointerBtn->GetPointerPosX() - width/2) >> 4);
	}

	// control character END
	else if(charCode == KS_End) {
		// update GUI positions
		TextPointerBtn->SetLetterPosition(0xFFFF);
		if(!TextPointerBtn->IsPointerVisible())
			horScrollbar->SetSelectedItem((textStartWidth + TextPointerBtn->GetPointerPosX() - width/2) >> 4);
	}

	// control character DELETE
	else if(charCode == 0x7F)
	{
		FileEdited = true;

		int letter = MainFileTxt->GetLineOffset(MainFileTxt->GetCurrPos() + TextPointerBtn->GetCurrentLine()) + TextPointerBtn->GetCurrentLetter();
		if(letter >= 0)
		{
			MainFileTxt->RemoveText(letter, 1);

			// check if the maximum width is changed
			int line = MainFileTxt->GetLineOffset(MainFileTxt->GetCurrPos() + TextPointerBtn->GetCurrentLine());
			MainFileTxt->CheckMaxLineWidth(line);
		}
	}

	// control character BACKSPACE
	else if(charCode == 0x08)
	{
		FileEdited = true;

		int letter = MainFileTxt->GetLineOffset(MainFileTxt->GetCurrPos() + TextPointerBtn->GetCurrentLine()) + TextPointerBtn->GetCurrentLetter()-1;
		if(letter >= 0)
		{
			// removing linebreak
			if(TextPointerBtn->GetCurrentLetter() == 0)
			{
				if(TextPointerBtn->GetCurrentLine() == 0) {
					scrollbar->ScrollOneUp();
					scrollbar->listChanged(scrollbar->GetSelectedItem(), scrollbar->GetSelectedIndex());
				}
				else
					TextPointerBtn->SetCurrentLine(TextPointerBtn->GetCurrentLine()-1);

				// set last letter in previous line as selected
				TextPointerBtn->SetLetterPosition(0xFFFFFF);
				horScrollbar->SetSelectedItem((TextPointerBtn->GetPointerPosX() - width/2) >> 4);

				MainFileTxt->RemoveText(letter, 1);
			}
			// removing normal character
			else
			{
				MainFileTxt->RemoveText(letter, 1);
				TextPointerBtn->SetLetterPosition(TextPointerBtn->GetCurrentLetter()-1);

				if(!TextPointerBtn->IsPointerVisible())
					horScrollbar->SetSelectedItem((textStartWidth + TextPointerBtn->GetPointerPosX() - width/2) >> 4);
			}

			// check if the maximum width is changed
			int line = MainFileTxt->GetLineOffset(MainFileTxt->GetCurrPos() + TextPointerBtn->GetCurrentLine());
			MainFileTxt->CheckMaxLineWidth(line);
		}
	}
	else
	{
		FileEdited = true;

		int letter = MainFileTxt->GetLineOffset(MainFileTxt->GetCurrPos() + TextPointerBtn->GetCurrentLine()) + TextPointerBtn->GetCurrentLetter();

		MainFileTxt->AddChar(letter, charCode);

		// adding linebreak
		if(charCode == L'\n')
		{
			if(TextPointerBtn->GetCurrentLine() + 1 >= linestodraw/2) {
				scrollbar->ScrollOneDown();
				scrollbar->listChanged(scrollbar->GetSelectedItem(), scrollbar->GetSelectedIndex());
			}
			else
				TextPointerBtn->SetCurrentLine(TextPointerBtn->GetCurrentLine()+1);
			horScrollbar->SetSelectedItem(0);
			TextPointerBtn->SetLetterPosition(0);
		}
		// adding normal character
		else
		{
			if(!TextPointerBtn->IsPointerVisible())
				horScrollbar->SetSelectedItem((textStartWidth + TextPointerBtn->GetPointerPosX() - width/2) >> 4);

			TextPointerBtn->SetLetterPosition(TextPointerBtn->GetCurrentLetter()+1);
		}
		// check if the maximum width is changed
		int line = MainFileTxt->GetLineOffset(MainFileTxt->GetCurrPos() + TextPointerBtn->GetCurrentLine());
		MainFileTxt->CheckMaxLineWidth(line);
	}
}

void TextEditor::OnPointerHeld(GuiButton *sender UNUSED, int pointer, const POINT &p)
{
	if(!userInput[pointer].wpad.ir.valid)
		return;

	TextPointerBtn->PositionChanged(pointer, p.x - TextPointerBtn->GetLeft(), p.y - TextPointerBtn->GetTop());
}

void TextEditor::OnListChange(int selItem, int selIndex)
{
	MainFileTxt->SetTextLine(selItem+selIndex);
	TextPointerBtn->Refresh();
}

void TextEditor::OnHorScrollChange(int selItem)
{
	textStartWidth = selItem << 4;
	MainFileTxt->SetStartWidth(textStartWidth);
	TextPointerBtn->SetLetterPosition(TextPointerBtn->GetCurrentLetter());
}


void TextEditor::Update(GuiTrigger * t)
{
	if(state == STATE_DISABLED || !t)
		return;

	scrollbar->Update(t);
	horScrollbar->Update(t);
	maximizeBtn->Update(t);
	closeBtn->Update(t);
	minimizeBtn->Update(t);
	TextPointerBtn->Update(t);
	PlusBtn->Update(t);
	keyboard->Update(t);

	scrollbar->SetEntrieCount(MainFileTxt->GetTotalLinesCount());

	int maxLineWidth = (MainFileTxt->GetMaxLineWidth() - displayLineWidth + 30);
	if(maxLineWidth < 0)
		maxLineWidth = 0;

	horScrollbar->SetEntrieCount(maxLineWidth >> 4);
}

void TextEditor::OnFinishedFileLoad(u8 *buffer, u32 bufferSize)
{
	if(!buffer)
	{
		ThrowMsg(tr("Error:"), tr("Could not load text file."));
		this->Close();
		return;
	}
	else if(bufferSize > (u32) (4.5*MBSIZE))
	{
		free(buffer);
		ThrowMsg(tr("Error:"), tr("File is too big."));
		this->Close();
		return;
	}


	u8 * tmp = (u8 *) realloc(buffer, bufferSize+1);
	if(!tmp)
	{
		free(buffer);
		ThrowMsg(tr("Error:"), tr("Not enough memory."));
		this->Close();
		return;
	}
	buffer = tmp;
	buffer[bufferSize] = 0;
	bufferSize++;
	wString * filetext = NULL;

	//To check if text is UTF8 or not
	if(utf8Len((char*) buffer) > 0)
	{
		filetext = new (std::nothrow) wString();
		if(filetext)
			filetext->fromUTF8((char*) buffer);

		free(buffer);
		buffer = NULL;
	}
	else
	{
		wchar_t * tmptext = charToWideChar((char*) buffer);

		free(buffer);
		buffer = NULL;

		if(tmptext)
		{
			filetext = new (std::nothrow) wString(tmptext);
			delete [] tmptext;
		}

	}

	if(!filetext)
	{
		ThrowMsg(tr("Error:"), tr("Not enough memory."));
		this->Close();
		return;
	}

	this->MainFileTxt->SetText(filetext->c_str());
	this->DimBackground(true);

	delete filetext;
	filetext = NULL;

	Application::Instance()->SetUpdateOnly(this);
	Application::Instance()->Append(this);

	// stop progress bar in case the user clicked on taskbar widget and opened window during text loading process
	ProgressWindow::Instance()->StopProgress();
}

void TextEditor::LoadFile(const char *filepath)
{
	if(!filepath)
		return;

	TextEditor * Editor = new TextEditor(filepath);
	Editor->SetEffect(EFFECT_SLIDE_TOP | EFFECT_SLIDE_IN, 50);
	Editor->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
	Editor->SetPosition(0, 0);

	FileLoadTask *task = new FileLoadTask(filepath, false);
	task->LoadingComplete.connect(Editor, &TextEditor::OnFinishedFileLoad);
	task->SetAutoDelete(true);
	Taskbar::Instance()->AddTask(task);
	ThreadedTaskHandler::Instance()->AddTask(task);
}
