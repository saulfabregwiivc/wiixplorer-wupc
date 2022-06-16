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
#ifndef SETTINGS_ENUMS_H_
#define SETTINGS_ENUMS_H_

#define MAXSMBUSERS		 10
#define MAXFTPUSERS		 10
#define MAXNFSUSERS		 10

enum
{
	OFF = 0,
	ON,
};

enum {
	APP_DEFAULT = 0,
	CONSOLE_DEFAULT,
	JAPANESE,
	ENGLISH,
	GERMAN,
	FRENCH,
	SPANISH,
	ITALIAN,
	DUTCH,
	S_CHINESE,
	T_CHINESE,
	KOREAN,
	MAX_LANGUAGE
};

enum
{
	ICONBROWSER = 0,
	LISTBROWSER,
};

typedef struct _SMBData
{
	char	Host[50];
	char	User[50];
	char	Password[50];
	char	SMBName[50];
} SMBData;

typedef struct _FTPData
{
	char	Host[50];
	char	User[50];
	char	Password[50];
	char	FTPPath[50];
	unsigned short	Port;
	short	Passive;
} FTPData;

typedef struct _FTPServerData
{
	short   AutoStart;
	char	Password[50];
	unsigned short	Port;
} FTPServerData;

typedef struct _NFSData
{
	char 	Host[16]; // IP address for now
	char	Mountpoint[50];
} NFSData;

#endif
