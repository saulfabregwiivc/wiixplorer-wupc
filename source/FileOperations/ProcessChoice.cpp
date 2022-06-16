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
#include "Menus/Explorer.h"
#include "ProcessChoice.h"
#include "Controls/Clipboard.h"
#include "Controls/Application.h"
#include "Controls/Taskbar.h"
#include "Controls/ThreadedTaskHandler.hpp"
#include "Prompts/Properties.h"
#include "Prompts/ArchiveProperties.h"
#include "Prompts/ProgressWindow.h"
#include "Prompts/PromptWindows.h"
#include "Prompts/PopUpMenu.h"
#include "FileStartUp/FileStartUp.h"
#include "FileOperations/fileops.h"
#include "FileOperations/CopyTask.h"
#include "FileOperations/MoveTask.h"
#include "FileOperations/DeleteTask.h"
#include "FileOperations/MD5Task.h"
#include "FileOperations/PackTask.h"
#include "FileOperations/UnpackTask.h"

void Explorer::ProcessArcChoice(int choice, const char * destCandidat)
{
	ArchiveBrowser * browser = (ArchiveBrowser *) curBrowser;
	if(!browser)
		return;

	if(choice == ArcPasteItems)
	{
		int ret = WindowPrompt(tr("Paste the item(s) into this directory?"), Clipboard::Instance()->GetItemName(Clipboard::Instance()->GetItemcount()-1), tr("Yes"), tr("Cancel"));
		if(ret <= 0)
			return;

		PackTask *task = new PackTask(Clipboard::Instance(), browser->GetCurrentPath(), browser->GetArchive(), Settings.CompressionLevel);
		task->TaskEnd.connect(this, &Explorer::OnFinishedTask);
		this->explorerTasks++;
		Taskbar::Instance()->AddTask(task);
		ThreadedTaskHandler::Instance()->AddTask(task);

		Clipboard::Instance()->Reset();
	}

	else if(choice == ArcExtractFile)
	{
		int ret = WindowPrompt(tr("Extract the selected item(s)?"), browser->GetCurrentName(), tr("Yes"), tr("Cancel"));
		if(ret <= 0)
			return;

		char dest[MAXPATHLEN];
		snprintf(dest, sizeof(dest), "%s", destCandidat);

		int result = OnScreenKeyboard(dest, sizeof(dest));
		if(result)
		{
			//append selected Item
			browser->MarkCurrentItem();
			//Get ItemMarker
			ItemMarker * IMarker = browser->GetItemMarker();
			//switch between browser index and archive file index
			for(int i = 0; i < IMarker->GetItemcount(); i++)
				IMarker->GetItem(i)->itemindex = browser->GetItemStructure(IMarker->GetItem(i)->itemindex)->fileindex;

			UnpackTask *task = new UnpackTask(IMarker, dest, browser->GetArchive(), false);
			task->TaskEnd.connect(this, &Explorer::OnFinishedTask);
			this->explorerTasks++;
			Taskbar::Instance()->AddTask(task);
			ThreadedTaskHandler::Instance()->AddTask(task);

			IMarker->Reset();
		}
	}
	else if(choice == ArcExtractAll)
	{
		int ret = WindowPrompt(tr("Extract full archive?"), 0, tr("Yes"), tr("Cancel"));
		if(ret <= 0)
			return;

		char dest[MAXPATHLEN];
		snprintf(dest, sizeof(dest), "%s", destCandidat);
		if(dest[strlen(dest)-1] != '/')
			strcat(dest, "/");

		strncat(dest, browser->GetArchiveName(), sizeof(dest));

		char * ext = strrchr(dest, '.');
		if(ext)
			ext[0] = 0;
		strcat(dest, "/");

		int result = OnScreenKeyboard(dest, sizeof(dest));
		if(result)
		{
			UnpackTask *task = new UnpackTask(browser->GetItemMarker(), dest, browser->GetArchive(), true);
			task->TaskEnd.connect(this, &Explorer::OnFinishedTask);
			this->explorerTasks++;
			Taskbar::Instance()->AddTask(task);
			ThreadedTaskHandler::Instance()->AddTask(task);

			browser->GetItemMarker()->Reset();
		}
	}
	else if(choice == ArcProperties)
	{
		browser->MarkCurrentItem();
		ItemMarker * Marker = browser->GetItemMarker();

		ArchiveProperties * Prompt = new ArchiveProperties(browser->GetArchive(), Marker);
		Prompt->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
		Prompt->DimBackground(true);
		Application::Instance()->SetUpdateOnly(Prompt);
		Application::Instance()->Append(Prompt);

		Marker->Reset();
	}
}

void Explorer::ProcessChoice(int choice)
{
	FileBrowser * browser = (FileBrowser *) curBrowser;
	if(!browser)
		return;

	else if(browser->GetCurrentFilename() && strcmp(browser->GetCurrentFilename(),"..") != 0)
	{
		if(choice == CUT)
		{
			choice = WindowPrompt(browser->GetCurrentFilename(), tr("Cut current marked item(s)?"), tr("Yes"), tr("Cancel"));
			if(choice == 1)
			{
				Clipboard::Instance()->Reset();
				//append selected Item
				browser->MarkCurrentItem();
				//Get ItemMarker
				ItemMarker * IMarker = browser->GetItemMarker();

				for(int i = 0; i < IMarker->GetItemcount(); i++)
					Clipboard::Instance()->AddItem(IMarker->GetItem(i));

				IMarker->Reset();
				Clipboard::Instance()->Operation = OP_MOVE;
			}
		}

		else if(choice == COPY)
		{
			choice = WindowPrompt(browser->GetCurrentFilename(), tr("Copy current marked item(s)?"), tr("Yes"), tr("Cancel"));
			if(choice == 1)
			{
				Clipboard::Instance()->Reset();
				//append selected Item
				browser->MarkCurrentItem();
				//Get ItemMarker
				ItemMarker * IMarker = browser->GetItemMarker();

				for(int i = 0; i < IMarker->GetItemcount(); i++)
					Clipboard::Instance()->AddItem(IMarker->GetItem(i));

				IMarker->Reset();
				Clipboard::Instance()->Operation = OP_COPY;
			}
		}

		else if(choice == RENAME)
		{
			char srcpath[MAXPATHLEN];
			char destdir[MAXPATHLEN];
			snprintf(srcpath, sizeof(srcpath), "%s", browser->GetCurrentSelectedFilepath());
			char entered[151];
			snprintf(entered, sizeof(entered), "%s", browser->GetCurrentFilename());
			int result = OnScreenKeyboard(entered, 150);
			if(result == 1)
			{
				snprintf(destdir, sizeof(destdir), "%s/%s", browser->GetCurrentPath(), entered);
				if(!RenameFile(srcpath, destdir))
					WindowPrompt(tr("Failed renaming item"), tr("Name might already exists."), tr("OK"));
				//! Update browser
				guiBrowser->Refresh();
			}
		}

		else if(choice == DELETE)
		{
			char currentpath[MAXPATHLEN];
			snprintf(currentpath, sizeof(currentpath), "%s/", browser->GetCurrentSelectedFilepath());
			choice = WindowPrompt(browser->GetCurrentFilename(), tr("Delete the selected item(s) and its content?"), tr("Yes"), tr("Cancel"));
			if(choice == 1)
			{
				if(ProgressWindow::Instance()->IsRunning())
					choice = WindowPrompt(tr("Currently a process is running."), tr("Do you want to append this delete to the queue?"), tr("Yes"), tr("Cancel"));

				if(choice == 1)
				{
					//append selected Item
					browser->MarkCurrentItem();
					//Get ItemMarker
					ItemMarker * IMarker = browser->GetItemMarker();
					DeleteTask *task = new DeleteTask(IMarker);
					task->TaskEnd.connect(this, &Explorer::OnFinishedTask);
					this->explorerTasks++;
					Taskbar::Instance()->AddTask(task);
					ThreadedTaskHandler::Instance()->AddTask(task);
					IMarker->Reset();
				}
			}
		}
	}
	else if(choice >= 0 && choice != PASTE && choice != NEWFOLDER && choice != PROPERTIES)
		WindowPrompt(tr("You cant use this operation on:"), tr("Directory .."), tr("OK"));

	if(choice == PASTE)
	{
		choice = WindowPrompt(Clipboard::Instance()->GetItemName(Clipboard::Instance()->GetItemcount()-1), tr("Paste item(s) into current directory?"), tr("Yes"), tr("Cancel"));
		if(choice == 1)
		{
			if(ProgressWindow::Instance()->IsRunning())
				choice = WindowPrompt(tr("Currently a process is running."), tr("Do you want to append this paste to the queue?"), tr("Yes"), tr("Cancel"));

			if(choice == 1)
			{
				if(Clipboard::Instance()->Operation == OP_COPY)
				{
					CopyTask *task = new CopyTask(Clipboard::Instance(), browser->GetCurrentPath());
					task->TaskEnd.connect(this, &Explorer::OnFinishedTask);
					this->explorerTasks++;
					Taskbar::Instance()->AddTask(task);
					ThreadedTaskHandler::Instance()->AddTask(task);
				}
				else if(Clipboard::Instance()->Operation == OP_MOVE)
				{
					MoveTask *task = new MoveTask(Clipboard::Instance(), browser->GetCurrentPath());
					task->TaskEnd.connect(this, &Explorer::OnFinishedTask);
					this->explorerTasks++;
					Taskbar::Instance()->AddTask(task);
					ThreadedTaskHandler::Instance()->AddTask(task);
				}
			}
		}
	}

	else if(choice == ADDTOZIP)
	{
		int ret = WindowPrompt(browser->GetCurrentFilename(), tr("Would you like to add/append the selected item(s) to a zip?"), tr("Yes"), tr("No"));
		if(ret <= 0)
			return;

		char DestZipPath[MAXPATHLEN];
		snprintf(DestZipPath, sizeof(DestZipPath), "%s", browser->GetCurrentPath());
		if(DestZipPath[strlen(DestZipPath)-1] != '/')
			strncat(DestZipPath, "/", sizeof(DestZipPath));
		strncat(DestZipPath, tr("NewZip.zip"), sizeof(DestZipPath));

		if(!OnScreenKeyboard(DestZipPath, sizeof(DestZipPath)))
			return;

		std::string DestPath = DestZipPath;
		size_t pos = DestPath.rfind('/');
		if(pos != std::string::npos)
			DestPath.erase(pos);

		CreateSubfolder(DestPath.c_str());

		//append selected Item
		browser->MarkCurrentItem();
		//Get ItemMarker
		ItemMarker * IMarker = browser->GetItemMarker();

		ZipFile *Zip = new ZipFile(DestZipPath, CheckFile(DestZipPath) ? ZipFile::APPEND : ZipFile::CREATE);
		ArchiveHandle * archive = new ArchiveHandle(Zip);

		PackTask *task = new PackTask(IMarker, "", archive, Settings.CompressionLevel);
		task->TaskEnd.connect(this, &Explorer::OnFinishedTask);
		this->explorerTasks++;
		Taskbar::Instance()->AddTask(task);
		ThreadedTaskHandler::Instance()->AddTask(task);

		//! Update browser
		IMarker->Reset();
	}

	else if(choice == CHECK_MD5)
	{
		int md5Choice = 1;

		if(ProgressWindow::Instance()->IsRunning())
			md5Choice = WindowPrompt(tr("Currently a process is running."), tr("Do you want to append this process to the queue?"), tr("Yes"), tr("Cancel"));

		if(md5Choice == 1)
		{
			char LogPath[1024];
			snprintf(LogPath, sizeof(LogPath), "%s/MD5.log", browser->GetCurrentPath());

			browser->MarkCurrentItem();
			MD5Task *task = new MD5Task(browser->GetItemMarker(), LogPath);
			task->TaskEnd.connect(this, &Explorer::OnFinishedTask);
			this->explorerTasks++;
			Taskbar::Instance()->AddTask(task);
			ThreadedTaskHandler::Instance()->AddTask(task);
			browser->GetItemMarker()->Reset();
		}
	}

	else if(choice == NEWFOLDER)
	{
		char entered[151];
		snprintf(entered, sizeof(entered), tr("New Folder"));
		int result = OnScreenKeyboard(entered, 150);
		if(result == 1)
		{
			char currentpath[MAXPATHLEN];
			snprintf(currentpath, sizeof(currentpath), "%s/%s/", browser->GetCurrentPath(), entered);
			bool ret = CreateSubfolder(currentpath);
			if(ret == false)
				ShowError(tr("Unable to create folder."));

			//! Update browser
			guiBrowser->Refresh();
		}
	}
	else if(choice == PROPERTIES)
	{
		browser->MarkCurrentItem();
		ItemMarker * Marker = browser->GetItemMarker();
		Properties * Prompt = new Properties(Marker);
		Prompt->SetAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
		Prompt->DimBackground(true);
		Application::Instance()->SetUpdateOnly(Prompt);
		Application::Instance()->Append(Prompt);
		Marker->Reset();
	}
}
