/****************************************************************************
 * Copyright (C) 2013 Dimok
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
#include "UpdateTask.h"
#include "ArchiveOperations/ZipFile.h"
#include "Controls/Application.h"
#include "FileOperations/fileops.h"
#include "Controls/Application.h"
#include "Prompts/PromptWindow.h"
#include "Prompts/ProgressWindow.h"
#include "network/ChangeLog.h"
#include "network/FileDownloader.h"
#include "network/networkops.h"
#include "network/http.h"
#include "network/URL_List.h"
#include "svnrev.h"
#include "sys.h"

static const char * appUpdateURL = "http://wiixplorer.googlecode.com/svn/trunk/HBC/update.xml";
static const char * metaUpdateURL = "http://wiixplorer.googlecode.com/svn/trunk/HBC/meta.xml";
static const char * iconUpdateURL = "http://wiixplorer.googlecode.com/svn/trunk/HBC/icon.png";
static const char * languageUpdateURL = "http://wiixplorer.googlecode.com/svn/trunk/Languages/";

UpdateTask::UpdateTask(bool updateApp, bool updateLang, bool silent)
	: Task(tr("Checking for updates")), bAutoDelete(false),
	  bUpdateApp(updateApp), bUpdateLang(updateLang), bSilent(silent)
{
}

UpdateTask::~UpdateTask()
{
}

void UpdateTask::Execute(void)
{
	TaskBegin(this);

	if(bUpdateApp)
	{
		int res = CheckForUpdate();
		if(!bSilent)
		{
			if(res == 0)
				ThrowMsg(tr("No new updates available"), 0);
			else if(res < 0)
				ThrowMsg(tr("Error:"), tr("Can't connect to update server"));
		}
	}

	if(bUpdateLang)
		UpdateLanguageFiles();

	if(bAutoDelete)
		Application::Instance()->PushForDelete(this);
}

/****************************************************************************
 * Checking if an Update is available
 ***************************************************************************/
int UpdateTask::CheckForUpdate(void)
{
	if(!IsNetworkInit())
		return -1;

	int revnumber = 0;
	int currentrev = atoi(SvnRev());

	struct block file = downloadfile(appUpdateURL);
	if(!file.data)
		return -2;

	file.data[file.size-1] = 0;

	char *revPtr = strstr((char*)file.data, "rev=\"");
	if(!revPtr) {
		free(file.data);
		return -3;
	}
	revPtr += strlen("rev=\"");

	revnumber = atoi(revPtr);

	if (revnumber <= currentrev)
		return 0;

	char *downloadLink = strstr(revPtr, "link=\"");
	if(!downloadLink) {
		free(file.data);
		return -4;
	}
	downloadLink += strlen("link=\"");

	char *ptr = strchr(downloadLink, '"');
	if(!ptr) {
		free(file.data);
		return -5;
	}
	*ptr = 0;

	char text[100];
	sprintf(text, tr("Update to Rev%i available"), revnumber);

	PromptWindow * Prompt = new PromptWindow(text, tr("Do you want to update now ?"), tr("Yes"), tr("Show Changelog"), tr("Cancel"));
	Prompt->DimBackground(true);
	Application::Instance()->Append(Prompt);
	Application::Instance()->SetUpdateOnly(Prompt);

	int choice;

	while((choice = Prompt->GetChoice()) < 0)
		usleep(10000);

	Application::Instance()->PushForDelete(Prompt);

	if(choice == 1)
	{
		if(DownloadApp(downloadLink) >= 0)
		{
			Prompt = new PromptWindow(tr("Update successfully finished"), tr("Do you want to reboot now ?"), tr("Yes"), tr("No"));
			Prompt->DimBackground(true);
			Application::Instance()->Append(Prompt);
			Application::Instance()->SetUpdateOnly(Prompt);

			while((choice = Prompt->GetChoice()) < 0)
				usleep(10000);

			Application::Instance()->PushForDelete(Prompt);

			if(choice)
				RebootApp();
		}
	}
	else if(choice == 2)
	{
		ChangeLog Changelog;
		Changelog.DownloadChangeLog(revnumber-5, revnumber);
		if(!Changelog.Show())
			ThrowMsg(tr("Error:"), tr("Failed to get the Changelog."));
	}

	return revnumber;
}


/****************************************************************************
 * UpdateApp from a given url. The dol is downloaded and overwrites the old one.
 ***************************************************************************/
int UpdateTask::DownloadApp(const char *url)
{
	if(!url)
	{
		ThrowMsg(tr("Error"), tr("URL is empty."));
		return -1;
	}

	//! append slash if it is missing
	std::string destPath = Settings.UpdatePath;
	if(destPath.size() > 0 && destPath[destPath.size()-1] != '/')
		destPath += '/';

	destPath += "boot.zip";

	CreateSubfolder(Settings.UpdatePath);

	int res = DownloadFileToPath(url, destPath.c_str(), false);
	if(res < 102400)
	{
		RemoveFile(destPath.c_str());
		ThrowMsg(tr("Update failed"), tr("Could not download file."));
		return -1;
	}
	else
	{
		StartProgress(tr("Extracting file..."));

		ZipFile zip(destPath.c_str());
		zip.ExtractAll(Settings.UpdatePath);

		RemoveFile(destPath.c_str());

		StopProgress();

		if(Settings.UpdateMetaxml)
			DownloadMetaXml();
		if(Settings.UpdateIconpng)
			DownloadIconPNG();
	}

	return 1;
}

/****************************************************************************
 * Update the Meta.xml file
 ***************************************************************************/
bool UpdateTask::DownloadMetaXml(void)
{
	if(!IsNetworkInit())
		return false;

	struct block file = downloadfile(metaUpdateURL);
	if(!file.data)
		return false;

	CreateSubfolder(Settings.UpdatePath);

	//! append slash if it is missing
	std::string destPath = Settings.UpdatePath;
	if(destPath.size() > 0 && destPath[destPath.size()-1] != '/')
		destPath += '/';

	destPath += "meta.xml";

	FILE * pFile = fopen(destPath.c_str(), "wb");
	if(!pFile)
		return false;

	fwrite(file.data, 1, file.size, pFile);
	fclose(pFile);
	free(file.data);

	return true;
}

/****************************************************************************
 * Update the Icon.png file
 ***************************************************************************/
bool UpdateTask::DownloadIconPNG(void)
{
	if(!IsNetworkInit())
		return false;

	struct block file = downloadfile(iconUpdateURL);
	if(!file.data)
		return false;

	CreateSubfolder(Settings.UpdatePath);

	//! append slash if it is missing
	std::string destPath = Settings.UpdatePath;
	if(destPath.size() > 0 && destPath[destPath.size()-1] != '/')
		destPath += '/';

	destPath += "icon.png";

	FILE * pFile = fopen(destPath.c_str(), "wb");
	if(!pFile)
		return false;

	fwrite(file.data, 1, file.size, pFile);
	fclose(pFile);
	free(file.data);

	return true;
}

/****************************************************************************
 * Update the language files
 ***************************************************************************/
bool UpdateTask::UpdateLanguageFiles(void)
{
	if(!IsNetworkInit())
		return false;

	char langpath[150];
	snprintf(langpath, sizeof(langpath), "%s", Settings.LanguagePath);
	if(langpath[strlen(langpath)-1] != '/')
	{
		char * ptr = strrchr(langpath, '/');
		if(ptr)
			ptr[1] = '\0';
	}

	if(!CreateSubfolder(langpath))
	{
		ThrowMsg(tr("Error:"), "%s", tr("Cannot create directory: "), langpath);
		return -1;
	}

	URL_List LinkList(languageUpdateURL);

	if(LinkList.GetURLCount() <= 0)
	{
		ThrowMsg(tr("Error:"), tr("No files found."));
		return -1;
	}

	ProgressWindow::Instance()->StartProgress(tr("Downloading files..."));
	ProgressWindow::Instance()->SetUnit(tr("files"));

	u32 FilesDownloaded = 0;

	for (int i = 0; i < LinkList.GetURLCount(); i++)
	{
		if(ProgressWindow::Instance()->IsCanceled())
			continue;

		ShowProgress(i, LinkList.GetURLCount(), LinkList.GetURL(i));

		char * fileext = strrchr(LinkList.GetURL(i), '.');
		if(!fileext)
			continue;

		if (strcasecmp(fileext, ".lang") != 0)
			continue;

		char fullURL[MAXPATHLEN];
		if(LinkList.IsFullURL(i))
			snprintf(fullURL, sizeof(fullURL), "%s", LinkList.GetURL(i));
		else
			snprintf(fullURL, sizeof(fullURL), "%s%s", languageUpdateURL, LinkList.GetURL(i));

		struct block file = downloadfile(fullURL);

		if (file.data && file.size > 0)
		{
			snprintf(fullURL, sizeof(fullURL), "%s%s", langpath, LinkList.GetURL(i));
			FILE * filePtr = fopen(fullURL, "wb");
			if(filePtr)
			{
				fwrite(file.data, 1, file.size, filePtr);
				fclose(filePtr);
				FilesDownloaded++;
			}
		}

		if(file.data)
			free(file.data);
	}

	// finish up the progress for this file
	ProgressWindow::Instance()->FinishProgress((LinkList.GetURLCount()-1)*20*1024);
	ProgressWindow::Instance()->StopProgress();
	ProgressWindow::Instance()->SetUnit(NULL);

	return FilesDownloaded;
}
