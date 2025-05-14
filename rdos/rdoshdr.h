/*#######################################################################
# RDOS operating system
# Copyright (C) 1988-2010, Leif Ekblad
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version. The only exception to this rule
# is for commercial usage in embedded systems. For information on
# usage in commercial embedded systems, contact embedded@rdos.net
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# The author of this program may be contacted at leif@rdos.net
#
# rdoshdr.h
# Rdos image header types
#
########################################################################*/



#ifndef _RDOSHDR_H
#define _RDOSHDR_H

#pragma pack( push, 1 )

#define RDOS_OBJECT_KERNEL          0
#define RDOS_OBJECT_FONT            1
#define RDOS_OBJECT_SIMPLE_DEVICE   2
#define RDOS_OBJECT_SHUTDOWN        3
#define RDOS_OBJECT_OLD_FILE        6
#define RDOS_OBJECT_COMMAND         7
#define RDOS_OBJECT_SET             8
#define RDOS_OBJECT_PATH            9

#define RDOS_OBJECT_DOS_DEVICE      16
#define RDOS_OBJECT_FILE            17

#define RDOS_OBJECT_DEVICE16        18
#define RDOS_OBJECT_DEVICE32        19
#define RDOS_OBJECT_LONG_MODE       20
#define RDOS_OBJECT_REAL_TIME       21

#define RDOS_OBJECT_SERVER          22

#define RDOS_SIGN    0x5A1E75D4

typedef struct TRdosObjectHeader
{
    long sign;
    long len;
    short int type;
    unsigned short int crc;
} TRdosObjectHeader;

typedef struct TRdosSimpleDeviceHeader
{
    short int StartIp;
} TRdosSimpleDeviceHeader;

typedef struct TRdosDosDeviceHeader
{
    short int Size;
    short int Sel;
    short int StartIp;
    char NameParam;
} TRdosDosDeviceHeader;

typedef struct TRdosDevice16Header
{
    long Size;
    short int CodeSize;
    short int CodeSel;
    short int DataSize;
    short int DataSel;
    short int StartIp;
    char NameParam;
} TRdosDevice16Header;

typedef struct TRdosDevice32Header
{
    long Size;
    long CodeSize;
    short int CodeSel;
    long DataSize;
    short int DataSel;
    long StartIp;
    char NameParam;
} TRdosDevice32Header;

typedef struct TRdosLongModeHeader
{
    long Size;
    long StartIp;
    long ImageBase;
    long ImageSize;
    long IdtBase;
    char NameParam;
} TRdosLongModeHeader;

typedef struct TRdosRealTimeHeader
{
    long Size;
} TRdosRealTimeHeader;

typedef struct TRdosOldFileHeader
{
    char Base[8];
    char Ext[3];
    char Attrib;
    char Resv[10];
    short int Time;
    short int Date;
    short int Cluster;
    int Size;
} TRdosOldFileHeader;

typedef struct TRdosFileHeader
{
    int Size;
    unsigned long LsbTime;
    unsigned long MsbTime;
    int FileSize;
    char Attrib;
    char FileName;
} TRdosFileHeader;

typedef struct TRdosServerHeader
{
    int Size;
    int FileSize;
    char FileName;
} TRdosServerHeader;


#pragma pack( pop )

#endif

