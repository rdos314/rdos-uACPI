/*#######################################################################
# RDOS operating system
# Copyright (C) 1988-2002, Leif Ekblad
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
# pciseg.cpp
# PCI segment device
#
########################################################################*/

#include <stdio.h>
#include <uacpi/acpi.h>
#include "acpi.h"
#include "pciseg.h"
#include "pcibrg.h"

char in_byte(int port);
#pragma aux in_byte = \
    "in al,dx" \
    parm [ edx ] \
    value [ al ]

short in_word(int port);
#pragma aux in_word = \
    "in ax,dx" \
    parm [ edx ] \
    value [ ax ]

int in_dword(int port);
#pragma aux in_dword = \
    "in eax,dx" \
    parm [ edx ] \
    value [ eax ]

void out_byte(int port, char val);
#pragma aux out_byte = \
    "out dx,al" \
    parm [ edx ] [ al ]

void out_word(int port, short val);
#pragma aux out_word = \
    "out dx,ax" \
    parm [ edx ] [ ax ]

void out_dword(int port, int val);
#pragma aux out_dword = \
    "out dx,eax" \
    parm [ edx ] [ eax ]

/*##########################################################################
#
#   Name       : TPciSegment::TPciSegment
#
#   Purpose....: Constructor for TPciSegment
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciSegment::TPciSegment(int segment)
{
    FSegment = segment;
    FStartBus = 0;
    FBusCount = 256;
    FMem = 0;

    Init();
}

/*##########################################################################
#
#   Name       : TPciSegment::TPciSegment
#
#   Purpose....: Constructor for TPciSegment
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciSegment::TPciSegment(struct acpi_mcfg_allocation *mfcg)
{
    FSegment = mfcg->segment;
    FStartBus = mfcg->start_bus;
    FBusCount = mfcg->end_bus - FStartBus + 1;

    if (mfcg->address)
        FMem = (char *)ServUacpiMap(mfcg->address, FBusCount << 20);
    else
        FMem = 0;

    Init();
}

/*##########################################################################
#
#   Name       : TPciSegment::~TPciSegment
#
#   Purpose....: Destructor for TPciSegment
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciSegment::~TPciSegment()
{
    int i;

    if (FMem)
        ServUacpiUnmap(FMem, FBusCount << 20);

    for (i = 0; i < FBusCount; i++)
        if (FBusArr[i])
            delete FBusArr[i];

    delete FBusArr;
}

/*##########################################################################
#
#   Name       : TPciSegment::Init
#
#   Purpose....: Init
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciSegment::Init()
{
    int i;

    FBusArr = new TPciBridge *[FBusCount];

    for (i = 0; i < FBusCount; i++)
        FBusArr[i] = 0;
    
    if (FMem)
        FIo = 0;
    else
    {
        FIo = 0xCF8;
        ServUacpiEnableIo(0xCF8, 4);
        ServUacpiEnableIo(0xCFC, 4);
    }
}

/*##########################################################################
#
#   Name       : TPciSegment::GetSegment
#
#   Purpose....: Get segment
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciSegment::GetSegment()
{
    return FSegment;
}

/*##########################################################################
#
#   Name       : TPciSegment::Add
#
#   Purpose....: Add bridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciSegment::Add(TPciBridge *bridge, int bus)
{
    int index = bus - FStartBus;

    if (index >= 0 && index < FBusCount)
    {
        FBusArr[index] = bridge;
    }
}

/*##########################################################################
#
#   Name       : TPciSegment::Get
#
#   Purpose....: Get bridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciBridge *TPciSegment::Get(int bus)
{
    int index = bus - FStartBus;

    if (index >= 0 && index < FBusCount)
        return FBusArr[index];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciBridge::CalcOffs
#
#   Purpose....: Convert to offset
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciSegment::CalcOffs(int bus, int dev, int func, int reg)
{
    int offs = -1;
    int rel = bus - FStartBus;

    if (rel >= 0 && rel < FBusCount && FBusArr[rel])
    {
        if (FMem)
        {
            offs = rel << 20;
            offs |= (dev & 0x1F) << 15;
            offs |= (func & 0x7) << 12;
            offs |= reg & 0xFFF;
        }
        else
        {
            offs = rel << 16;
            offs |= (dev & 0x1F) << 11;
            offs |= (func & 0x7) << 8;
            offs |= reg & 0xFF;
        }
    }
    return offs;
}

/*##########################################################################
#
#   Name       : TPciBridge::GetHandle
#
#   Purpose....: Calc handle
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciSegment::GetHandle(int bus, int dev, int func)
{
    int offs = -1;
    int rel = bus - FStartBus;
    
    if (dev < 0 || dev >= 32)
        return -1;
    
    if (func < 0 || func >= 8)
        return -1;

    if (rel >= 0 && rel < FBusCount)
    {
        if (FMem)
        {
            offs = rel << 20;
            offs |= (dev & 0x1F) << 15;
            offs |= (func & 0x7) << 12;
        }
        else
        {
            offs = rel << 16;
            offs |= (dev & 0x1F) << 11;
            offs |= (func & 0x7) << 8;
        }
    }
    return offs;
}

/*##########################################################################
#
#   Name       : TPciBridge::ReadConfigByte
#
#   Purpose....: Read config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char TPciSegment::ReadConfigByte(int offs)
{
    char *ptr;

    if (offs >= 0)
    {
        if (FMem)
        {
            ptr = FMem + offs;
            return *ptr;
        }
        else
        {
            offs |= 0x80000000;
            out_dword(FIo, offs & 0xFFFFFFFC);
            return in_byte(FIo + 4 + (offs & 3));
        }
    }
    else
        return -1;
}

/*##########################################################################
#
#   Name       : TPciSegment::ReadConfigWord
#
#   Purpose....: Read config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short TPciSegment::ReadConfigWord(int offs)
{
    short *ptr;

    if (offs >= 0)
    {
        if (FMem)
        {
            ptr = (short *)(FMem + offs);
            return *ptr;
        }
        else
        {
            offs |= 0x80000000;
            out_dword(FIo, offs & 0xFFFFFFFC);
            return in_word(FIo + 4 + (offs & 3));
        }
    }
    else
        return -1;
}

/*##########################################################################
#
#   Name       : TPciSegment::ReadConfigDword
#
#   Purpose....: Read config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciSegment::ReadConfigDword(int offs)
{
    int *ptr;

    if (offs >= 0)
    {
        if (FMem)
        {
            ptr = (int *)(FMem + offs);
            return *ptr;
        }
        else
        {
            offs |= 0x80000000;
            out_dword(FIo, offs & 0xFFFFFFFC);
            return in_dword(FIo + 4 + (offs & 3));
        }
    }
    else
        return -1;
}

/*##########################################################################
#
#   Name       : TPciSegment::WriteConfigByte
#
#   Purpose....: Write config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciSegment::WriteConfigByte(int offs, char val)
{
    char *ptr;

    if (offs >= 0)
    {
        if (FMem)
        {
            ptr = FMem + offs;
            *ptr = val;
        }
        else
        {
            offs |= 0x80000000;
            out_dword(FIo, offs & 0xFFFFFFFC);
            out_byte(FIo + 4 + (offs & 3), val);
        }
    }
}

/*##########################################################################
#
#   Name       : TPciSegment::WriteConfigWord
#
#   Purpose....: Write config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciSegment::WriteConfigWord(int offs, short val)
{
    short *ptr;

    if (offs >= 0)
    {
        if (FMem)
        {
            ptr = (short *)(FMem + offs);
            *ptr = val;
        }
        else
        {
            offs |= 0x80000000;
            out_dword(FIo, offs & 0xFFFFFFFC);
            out_word(FIo + 4 + (offs & 3), val);
        }
    }
}

/*##########################################################################
#
#   Name       : TPciSegment::WriteConfigDword
#
#   Purpose....: Write config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciSegment::WriteConfigDword(int offs, int val)
{
    int *ptr;

    if (offs >= 0)
    {
        if (FMem)
        {
            ptr = (int *)(FMem + offs);
            *ptr = val;
        }
        else
        {
            offs |= 0x80000000;
            out_dword(FIo, offs & 0xFFFFFFFC);
            out_dword(FIo + 4 + (offs & 3), val);
        }
    }
}

/*##########################################################################
#
#   Name       : TPciBridge::ReadConfigByte
#
#   Purpose....: Read config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char TPciSegment::ReadConfigByte(int handle, int reg)
{
    char *ptr;

    if (handle >= 0)
    {
        if (FMem)
        {
            if (reg <= 0xFFF)
            {
                ptr = FMem + handle + reg;
                return *ptr;
            }
        }
        else
        {
            if (reg <= 0xFF)
            {
                handle += reg;
                handle |= 0x80000000;
                out_dword(FIo, handle & 0xFFFFFFFC);
                return in_byte(FIo + 4 + (handle & 3));
            }
        }
    }
    return -1;
}

/*##########################################################################
#
#   Name       : TPciBridge::ReadConfigWord
#
#   Purpose....: Read config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short TPciSegment::ReadConfigWord(int handle, int reg)
{
    short *ptr;

    if (handle >= 0)
    {
        if (FMem)
        {
            if (reg <= 0xFFF)
            {
                ptr = (short *)(FMem + handle + reg);
                return *ptr;
            }
        }
        else
        {
            if (reg <= 0xFF)
            {
                handle += reg;
                handle |= 0x80000000;
                out_dword(FIo, handle & 0xFFFFFFFC);
                return in_word(FIo + 4 + (handle & 3));
            }
        }
    }
    return -1;
}

/*##########################################################################
#
#   Name       : TPciBridge::ReadConfigDword
#
#   Purpose....: Read config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciSegment::ReadConfigDword(int handle, int reg)
{
    int *ptr;

    if (handle >= 0)
    {
        if (FMem)
        {
            if (reg <= 0xFFF)
            {
                ptr = (int *)(FMem + handle + reg);
                return *ptr;
            }
        }
        else
        {
            if (reg <= 0xFF)
            {
                handle += reg;
                handle |= 0x80000000;
                out_dword(FIo, handle & 0xFFFFFFFC);
                return in_dword(FIo + 4 + (handle & 3));

            }
        }
    }
    return -1;
}

/*##########################################################################
#
#   Name       : TPciBridge::WriteConfigByte
#
#   Purpose....: Write config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciSegment::WriteConfigByte(int handle, int reg, char val)
{
    char *ptr;

    if (handle >= 0)
    {
        if (FMem)
        {
            if (reg <= 0xFFF)
            {
                ptr = FMem + handle + reg;
                *ptr = val;
            }
        }
        else
        {
            if (reg <= 0xFF)
            {
                handle += reg;
                handle |= 0x80000000;
                out_dword(FIo, handle & 0xFFFFFFFC);
                out_byte(FIo + 4 + (handle & 3), val);
            }
        }
    }
}

/*##########################################################################
#
#   Name       : TPciBridge::WriteConfigWord
#
#   Purpose....: Write config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciSegment::WriteConfigWord(int handle, int reg, short val)
{
    short *ptr;

    if (handle >= 0)
    {
        if (FMem)
        {
            if (reg <= 0xFFF)
            {
                ptr = (short *)(FMem + handle + reg);
                *ptr = val;
            }
        }
        else
        {
            if (reg <= 0xFF)
            {
                handle += reg;
                handle |= 0x80000000;
                out_dword(FIo, handle & 0xFFFFFFFC);
                out_word(FIo + 4 + (handle & 3), val);
            }
        }
    }
}

/*##########################################################################
#
#   Name       : TPciBridge::WriteConfigDword
#
#   Purpose....: Write config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciSegment::WriteConfigDword(int handle, int reg, int val)
{
    int *ptr;

    if (handle >= 0)
    {
        if (FMem)
        {
            if (reg <= 0xFFF)
            {
                ptr = (int *)(FMem + handle + reg);
                *ptr = val;
            }
        }
        else
        {
            if (reg <= 0xFF)
            {
                handle += reg;
                handle |= 0x80000000;
                out_dword(FIo, handle & 0xFFFFFFFC);
                out_dword(FIo + 4 + (handle & 3), val);
            }
        }
    }
}

/*##########################################################################
#
#   Name       : TPciSegment::HasDev
#
#   Purpose....: Check for device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciSegment::HasDev(TPciBridge *bridge, int device)
{
    int offs = CalcOffs(bridge->GetBridgeBus(), device, 0, 0);
    int func;
    bool found = false;
    int *ptr;
    int val;

    if (offs >= 0)
    {
        if (FMem)
        {
            ptr = (int *)(FMem + offs);

            for (func = 0; func < 8 && !found; func++)
            {
                val = *ptr;
                if (val == 0xFFFFFFFF)
                    ptr += 0x400;
                else
                    found = true;
            }
        }
        else
        {
            offs |= 0x80000000;

            for (func = 0; func < 8 && !found; func++)
            {
                out_dword(FIo, offs);
                val = in_dword(FIo + 4);

                if (val == 0xFFFFFFFF)
                    offs += 0x100;
                else
                    found = true;
            }
        }
    }
    return found;
}
