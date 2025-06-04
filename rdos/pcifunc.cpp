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
# pcifunc.cpp
# PCI function
#
########################################################################*/

#include <stdio.h>
#include <string.h>
#include "pci.h"
#include "pcifunc.h"
#include "pcidev.h"

int TPciFunction::FFuncCount = 0;
int TPciFunction::FFuncSize = 0;
TPciFunction **TPciFunction::FFuncArr = 0;

/*##########################################################################
#
#   Name       : TPciFunction::TPciFunction
#
#   Purpose....: Constructor for TPciFunction
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciFunction::TPciFunction()
{
    FPciDevice = 0;
    FPciFunction = 0;
    FVendor = 0;
    FDevice = 0;
    FClass = 0;
    FSubClass = 0;
    FProtocol = 0;

    Add(this);
}

/*##########################################################################
#
#   Name       : TPciFunction::TPciFunction
#
#   Purpose....: Constructor for TPciFunction
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciFunction::TPciFunction(TPciDevice *device, int function, int vendor_device, unsigned char class_code, unsigned char sub_class)
{
    FPciDevice = device;
    FPciFunction = function;
    Init(vendor_device, class_code, sub_class);
}

/*##########################################################################
#
#   Name       : TPciFunction::~TPciFunction
#
#   Purpose....: Destructor for TPciFunction
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciFunction::~TPciFunction()
{
}

/*##########################################################################
#
#   Name       : TPciFunction::Init
#
#   Purpose....: Init device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::Init(int vendor_device, unsigned char class_code, unsigned char sub_class)
{
    FVendor = (unsigned short)(vendor_device & 0xFFFF);
    FDevice = (unsigned short)((vendor_device >> 16) & 0xFFFF);
    FClass = class_code;
    FSubClass = sub_class;
    FProtocol = (unsigned char)ReadConfigByte(PCI_progIF);

    Add(this);
}

/*##########################################################################
#
#   Name       : TPciFunction::SetDevice
#
#   Purpose....: Set device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::SetDevice(TPciDevice *device)
{
    int vendor_device = ReadConfigDword(PCI_vendorID);
    unsigned char class_code = (unsigned char)ReadConfigByte(PCI_classcode);;
    unsigned char sub_class = (unsigned char)ReadConfigByte(PCI_subclass);;

    FPciDevice = device;
    Init(vendor_device, class_code, sub_class);
}

/*##########################################################################
#
#   Name       : TPciFunction::IsPciFunction
#
#   Purpose....: Is PCI function?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciFunction::IsPciFunction()
{
    return true;
}

/*##########################################################################
#
#   Name       : TPciFunction::Count
#
#   Purpose....: Get function count
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::Count()
{
    return FFuncCount;
}

/*##########################################################################
#
#   Name       : TPciFunction::Get
#
#   Purpose....: Get function #
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciFunction *TPciFunction::Get(int index)
{
    if (index >= 0 && index < FFuncCount)
        return FFuncArr[index];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::FindClass
#
#   Purpose....: Get class
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::FindClass(int index, unsigned char class_code, unsigned char sub_class)
{
    int i;
    TPciFunction *func;

    for (i = index; i < FFuncCount; i++)
    {
        func = FFuncArr[i];
        if (func->FClass == class_code && func->FSubClass == sub_class)
            return i + 1;
    }

    return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::FindClassProtocol
#
#   Purpose....: Get class & protocol
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::FindClassProtocol(int index, unsigned char class_code, unsigned char sub_class, unsigned char protocol)
{
    int i;
    TPciFunction *func;

    for (i = index; i < FFuncCount; i++)
    {
        func = FFuncArr[i];
        if (func->FClass == class_code && func->FSubClass == sub_class && func->FProtocol == protocol)
            return i + 1;
    }

    return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::FindDevice
#
#   Purpose....: Find device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::FindDevice(int index, unsigned short vendor, unsigned short device)
{
    int i;
    TPciFunction *func;

    for (i = index; i < FFuncCount; i++)
    {
        func = FFuncArr[i];
        if (func->FDevice == device && func->FVendor == vendor)
            return i + 1;
    }

    return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::FindDevice
#
#   Purpose....: Find device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::GetHandle(unsigned char segment, unsigned char bus, unsigned device, unsigned function)
{
    int i;
    TPciFunction *func;

    for (i = 0; i < FFuncCount; i++)
    {
        func = FFuncArr[i];
        if (func->GetSegment() == segment && func->GetBus() == bus && func->GetPciDevice() == device && func->GetPciFunction() == function)
            return i + 1;
    }

    return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetParam
#
#   Purpose....: Get param
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::GetParam(int handle)
{
    TPciFunction *func = 0;
    int val;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
    {
        val = func->GetSegment();
        val = val << 8;

        val |= func->GetBus();
        val = val << 8;

        val |= func->GetPciDevice();
        val = val << 8;

        val |= func->FPciFunction;

        return val;
    }
    else
        return -1;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetIrq
#
#   Purpose....: Get IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned char TPciFunction::GetIrq(int handle)
{
    TPciFunction *func = 0;
    TPciIrqRoute *irq = 0;
    unsigned char nr = 0;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
        irq = func->GetIrq();

    if (irq)
        nr = (unsigned char)irq->Irq;

    if (!nr)
    {
        nr = func->ReadConfigByte(60);
        if (nr == 0xFF)
            nr = 0;
    }

    return nr;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetCap
#
#   Purpose....: Get capability
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short int TPciFunction::GetCap(int handle, unsigned char cap)
{
    TPciFunction *func = 0;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
        return func->GetCap(cap);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetPciName
#
#   Purpose....: Get PCI name
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::GetPciName(int handle, char *buf, int maxsize)
{
    TPciFunction *func = 0;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
        return func->GetPciName(buf, maxsize);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::ReadPciConfigByte
#
#   Purpose....: Read PCI config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char TPciFunction::ReadPciConfigByte(int issuer, int handle, int reg)
{
    TPciFunction *func = 0;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
        return func->ReadConfigByte(reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : TPciFunction::ReadPciConfigWord
#
#   Purpose....: Read PCI config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short int TPciFunction::ReadPciConfigWord(int issuer, int handle, int reg)
{
    TPciFunction *func = 0;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
        return func->ReadConfigWord(reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : TPciFunction::ReadPciConfigDword
#
#   Purpose....: Read PCI config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::ReadPciConfigDword(int issuer, int handle, int reg)
{
    TPciFunction *func = 0;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
        return func->ReadConfigDword(reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : TPciFunction::WritePciConfigByte
#
#   Purpose....: Write PCI config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::WritePciConfigByte(int issuer, int handle, int reg, char val)
{
    TPciFunction *func = 0;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
        func->WriteConfigByte(reg, val);
}

/*##########################################################################
#
#   Name       : TPciFunction::WritePciConfigWord
#
#   Purpose....: Write PCI config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::WritePciConfigWord(int issuer, int handle, int reg, short int val)
{
    TPciFunction *func = 0;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
        func->WriteConfigWord(reg, val);
}

/*##########################################################################
#
#   Name       : TPciFunction::WritePciConfigDword
#
#   Purpose....: Write PCI config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::WritePciConfigDword(int issuer, int handle, int reg, int val)
{
    TPciFunction *func = 0;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
        func->WriteConfigDword(reg, val);
}

/*##########################################################################
#
#   Name       : TPciFunction::Add
#
#   Purpose....: Add function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::Add(TPciFunction *func)
{
    TPciFunction **arr;
    int size;
    int i;

    if (!func->FPciDevice)
        return;

    if (FFuncSize == FFuncCount)
    {
        if (FFuncSize)
        {
            size = 2 * FFuncSize;
            arr = new TPciFunction *[size];

            for (i = 0; i < FFuncSize; i++)
                arr[i] = FFuncArr[i];

            delete FFuncArr;
            FFuncArr = arr;
            FFuncSize = size;
        }
        else
        {
            FFuncSize = 4;
            FFuncArr = new TPciFunction *[FFuncSize];
        }
    }
    FFuncArr[FFuncCount] = func;
    FFuncCount++;
}

/*##########################################################################
#
#   Name       : TPciFunction::FindPciFunction
#
#   Purpose....: Find PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiObject *TPciFunction::FindPciFunction(int device, int function)
{
    if (FPciDevice)
        return FPciDevice->Find(device, function);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetSegment
#
#   Purpose....: Get PCI segment
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::GetSegment()
{
    if (FPciDevice)
        return FPciDevice->GetSegment();
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetBus
#
#   Purpose....: Get PCI bus
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::GetBus()
{
    if (FPciDevice)
        return FPciDevice->GetBus();
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetPciDevice
#
#   Purpose....: Get PCI device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::GetPciDevice()
{
    if (FPciDevice)
        return FPciDevice->GetDevice();
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetPciFunction
#
#   Purpose....: Get PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::GetPciFunction()
{
    return FPciFunction;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetVendor
#
#   Purpose....: Get PCI vendor
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned short TPciFunction::GetVendor()
{
    return FVendor;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetDevice
#
#   Purpose....: Get PCI device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned short TPciFunction::GetDevice()
{
    return FDevice;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetClass
#
#   Purpose....: Get PCI class
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned char TPciFunction::GetClass()
{
    return FClass;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetSubClass
#
#   Purpose....: Get PCI subclass
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned char TPciFunction::GetSubClass()
{
    return FSubClass;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetProtocol
#
#   Purpose....: Get PCI protocol
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned char TPciFunction::GetProtocol()
{
    return FProtocol;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetIrq
#
#   Purpose....: Get IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciIrqRoute *TPciFunction::GetIrq()
{
    char pin = ReadConfigByte(61);

    if (FPciDevice)
        return FPciDevice->GetIrq(pin);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetCap
#
#   Purpose....: Get capability
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short int TPciFunction::GetCap(unsigned char cap)
{
    int i;
    int reg;
    char ch;
    unsigned char uch;

    ch = ReadConfigByte(6);

    if (ch & 0x10)
    {
        ch = ReadConfigByte(0x34);

        for (i = 0; i < 48; i++)
        {
            reg = (unsigned char)(ch & 0xFC);

            if (reg >= 0x40)
            {
                uch = (unsigned char)ReadConfigByte(reg);
                if (uch == cap)
                    return reg;
                else
                    ch = ReadConfigByte(reg + 1);
            }
            else
                return -1;
        }
    }
    return -1;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetPciName
#
#   Purpose....: Get PCI name
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::GetPciName(char *buf, int maxsize)
{
    const uacpi_char *path;
    int size;

    if (FNode)
    {
        path = uacpi_namespace_node_generate_absolute_path(FNode);
        size = strlen(path);
        strncpy(buf, path, maxsize);
        uacpi_free_absolute_path(path);
        return size + 1;
    }
    else
    {
        buf[0] = 0;
        return 1;
    }
}

/*##########################################################################
#
#   Name       : TPciFunction::ReadConfigByte
#
#   Purpose....: Read config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char TPciFunction::ReadConfigByte(int reg)
{
    if (FPciDevice)
        return FPciDevice->ReadConfigByte(FPciFunction, reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : TPciFunction::ReadConfigWord
#
#   Purpose....: Read config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short TPciFunction::ReadConfigWord(int reg)
{
    if (FPciDevice)
        return FPciDevice->ReadConfigWord(FPciFunction, reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : TPciFunction::ReadConfigDword
#
#   Purpose....: Read config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::ReadConfigDword(int reg)
{
    if (FPciDevice)
        return FPciDevice->ReadConfigDword(FPciFunction, reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : TPciFunction::WriteConfigByte
#
#   Purpose....: Write config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::WriteConfigByte(int reg, char val)
{
    if (FPciDevice)
        FPciDevice->WriteConfigByte(FPciFunction, reg, val);
}

/*##########################################################################
#
#   Name       : TPciFunction::WriteConfigWord
#
#   Purpose....: Write config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::WriteConfigWord(int reg, short val)
{
    if (FPciDevice)
        FPciDevice->WriteConfigWord(FPciFunction, reg, val);
}

/*##########################################################################
#
#   Name       : TPciFunction::WriteConfigDword
#
#   Purpose....: Write config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::WriteConfigDword(int reg, int val)
{
    if (FPciDevice)
        FPciDevice->WriteConfigDword(FPciFunction, reg, val);
}
