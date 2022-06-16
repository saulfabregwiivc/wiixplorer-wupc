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
#include "Prompts/PromptWindows.h"
#include "Prompts/ProgressWindow.h"
#include "network/HTML_Stream.h"
#include "FileOperations/fileops.h"
#include "TextOperations/TextEditor.h"
#include "ChangeLog.h"
#include "svnrev.h"

ChangeLog::ChangeLog()
{
}

ChangeLog::~ChangeLog()
{
}

char * ChangeLog::GetChangeLogRange(int fromRev, int tillRev, bool backwards)
{
	char * changelog = (char*) malloc(32);
	if(!changelog)
		return NULL;

	strcpy(changelog, "");

	//for '\0'
	int logsize = 1;
	int RevCounter;
	int StopRev;

	if(backwards)
	{
		RevCounter = tillRev;
		StopRev = fromRev-1;
	}
	else
	{
		RevCounter = fromRev;
		StopRev = tillRev+1;
	}

	StartProgress(tr("Getting Changelog..."));
	ProgressWindow::Instance()->SetUnit(tr("revs"));

	int RevsDone = 0;
	int TotalRevs = labs(StopRev-RevCounter);

	while(RevCounter != StopRev)
	{
		char ProgressText[50];
		snprintf(ProgressText, sizeof(ProgressText), "Revision %i", RevCounter);
		ShowProgress(RevsDone, TotalRevs, ProgressText);

		char * RevChangelog = this->GetChanges(RevCounter);
		if(!RevChangelog)
			continue;

		logsize += strlen(RevChangelog)+2;

		char * tmpMem = (char*) realloc(changelog, logsize);
		if(!tmpMem)
		{
			StopProgress();
			free(tmpMem);
			tmpMem = NULL;
			free(changelog);
			free(RevChangelog);
			return NULL;
		}

		changelog = tmpMem;

		strcat(changelog, RevChangelog);
		strcat(changelog, "\n\n");

		free(RevChangelog);

		if(backwards)
			RevCounter--;
		else
			RevCounter++;

		RevsDone++;
	}
	// finish up the progress
	FinishProgress(TotalRevs);

	StopProgress();
	ProgressWindow::Instance()->SetUnit(NULL);

	if(strlen(changelog) < 5)
	{
		//something went wrong
		free(changelog);
		changelog = NULL;
	}

	return changelog;
}

char * ChangeLog::GetChanges(int Rev)
{
	HTML_Stream html;

	char * changelog = (char*) malloc(32);
	if(!changelog)
		return NULL;

	strcpy(changelog, "");

	//for '\0'
	int logsize = 1;
	char RevTxt[50];
	char URL[80];

	sprintf(URL, "http://code.google.com/p/wiixplorer/source/detail?r=%i", Rev);
	if(!html.LoadLink(URL))
	{
		free(changelog);
		return NULL;;
	}

	html.FindStringEnd("<pre class=\"wrap\" style=\"margin-left:1em\">");
	char * RevChangelog = html.CopyString("</pre>");

	snprintf(RevTxt, sizeof(RevTxt), "Revision: %i\n", Rev);
	logsize += strlen(RevChangelog)+strlen(RevTxt);
	char * tmpMem = (char*) realloc(changelog, logsize);
	if(!tmpMem)
	{
		StopProgress();
		free(tmpMem);
		tmpMem = NULL;
		free(changelog);
		free(RevChangelog);
		return NULL;
	}

	changelog = tmpMem;
	strcat(changelog, RevTxt);
	strcat(changelog, RevChangelog);
	free(RevChangelog);

	if(strlen(changelog) < 5)
	{
		//something went wrong
		free(changelog);
		changelog = NULL;
	}

	return changelog;
}


bool ChangeLog::DownloadChangeLog(int fromRev, int tillRev, bool backwards)
{
	char * Changelog = GetChangeLogRange(fromRev, tillRev, backwards);

	if(!Changelog)
		return false;

	//! append slash if it is missing
	std::string writePath = Settings.UpdatePath;
	if(writePath.size() > 0 && writePath[writePath.size()-1] != '/')
		writePath += '/';

	writePath += "ChangeLog.txt";

	CreateSubfolder(Settings.UpdatePath);

	FILE * f = fopen(writePath.c_str(), "wb");
	if(!f)
		return false;

	fwrite(Changelog, 1, strlen(Changelog), f);
	fclose(f);
	free(Changelog);

	return true;
}

int ChangeLog::GetSavedChangeLogRev()
{
	//! append slash if it is missing
	std::string changelogPath = Settings.UpdatePath;
	if(changelogPath.size() > 0 && changelogPath[changelogPath.size()-1] != '/')
		changelogPath += '/';

	changelogPath += "ChangeLog.txt";

	FILE * f = fopen(changelogPath.c_str(), "rb");
	if(!f)
		return -1;

	char ChagelogRev[20];
	fseek(f, 10, SEEK_SET);
	fread(&ChagelogRev, 1, 4, f);
	fclose(f);

	return atoi(ChagelogRev);
}

bool ChangeLog::Show()
{
	//! append slash if it is missing
	std::string changelogPath = Settings.UpdatePath;
	if(changelogPath.size() > 0 && changelogPath[changelogPath.size()-1] != '/')
		changelogPath += '/';

	changelogPath += "ChangeLog.txt";

	if(GetSavedChangeLogRev() < atoi(SvnRev()))
	{
		if(!DownloadChangeLog(atoi(SvnRev())-5, atoi(SvnRev())))
			return false;
	}

	if(!CheckFile(changelogPath.c_str()))
		return false;

	TextEditor::LoadFile(changelogPath.c_str());

	return true;
}
