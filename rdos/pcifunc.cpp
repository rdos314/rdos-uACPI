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
#include "rdos.h"
#include "acpi.h"
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
    Init();
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
    Init();

    FPciDevice = device;
    FPciFunction = function;

    Setup(vendor_device, class_code, sub_class);
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
    if (FOwnerName)
        delete FOwnerName;
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
void TPciFunction::Init()
{
    int i;

    FPciDevice = 0;
    FPciFunction = 0;
    FVendor = 0;
    FDevice = 0;
    FClass = 0;
    FSubClass = 0;
    FProtocol = 0;

    FIssuer = 0;
    FOwnerName = 0;

    FMsiBase = 0;
    FMsiVectors = 0;
    FUseMsi = false;

    FMsiXBase = 0;
    FMsiXVectors = 0;
    FMsiXVectorArr = 0;
    FUseMsiX = false;

    FIrqCount = 0;

    for (i = 0; i < MAX_PCI_BARS; i++)
        FBarArr[i] = 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::Setup
#
#   Purpose....: Setup device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::Setup(int vendor_device, unsigned char class_code, unsigned char sub_class)
{
    int i;
    int reg;
    char ch;

    FVendor = (unsigned short)(vendor_device & 0xFFFF);
    FDevice = (unsigned short)((vendor_device >> 16) & 0xFFFF);
    FClass = class_code;
    FSubClass = sub_class;
    FProtocol = (unsigned char)ReadConfigByte(PCI_progIF);

    ch = ReadConfigByte(PCI_status_reg);

    if (ch & 0x10)
    {
        ch = ReadConfigByte(0x34);

        for (i = 0; i < 48; i++)
        {
            reg = (unsigned char)(ch & 0xFC);

            if (reg >= 0x40)
            {
                ch = ReadConfigByte(reg);
                switch (ch)
                {
                    case 5:
                        FMsiBase = (unsigned char)(ch + 2);
                        ch = ReadConfigByte(FMsiBase);
                        ch = (ch >> 1) & 0x3;
                        FMsiVectors = 1 << ch;
                        break;

                    case 0x11:
                        FMsiXBase = (unsigned char)(ch + 2);
                        FMsiXVectors = ReadConfigWord(FMsiXBase) + 1;
                        break;
                }
                ch = ReadConfigByte(reg + 1);
            }
        }
    }

    SetupBars();

    Add(this);
}

/*##########################################################################
#
#   Name       : TPciFunction::SetupBars
#
#   Purpose....: Setup BAR physical address array
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::SetupBars()
{
    int index;
    int low, high;
    long long val;

    for (index = 0; index < 6; index++)
    {
        low = ReadConfigDword(0x10 + 4 * index);
        if (low & 1)
            FBarArr[index] = low;
        else
        {
            if (low & 4)
            {
                high = ReadConfigDword(0x14 + 4 * index);
                val = (long long)high << 32;
                val |= low;
                FBarArr[index] = val;
                index++;
            }
            else
                FBarArr[index] = low;
        }
    }
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
    Setup(vendor_device, class_code, sub_class);
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
        if (func->FClass == class_code && func->FSubClass == sub_class && !func->FIssuer)
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
        if (func->FClass == class_code && func->FSubClass == sub_class && func->FProtocol == protocol && !func->FIssuer)
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
        if (func->FDevice == device && func->FVendor == vendor && !func->FIssuer)
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
#   Name       : TPciFunction::GetFunction
#
#   Purpose....: Get function from handle
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciFunction *TPciFunction::GetFunction(int handle)
{
    TPciFunction *func = 0;
    int val;

    if (handle > 0 && handle <= FFuncCount)
        return FFuncArr[handle - 1];
    else
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
#   Name       : TPciFunction::GetBar
#
#   Purpose....: Get BAR
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
long long TPciFunction::GetBar(int index)
{
    if (index >= 0 && index < 6)
        return FBarArr[index];
    else
        return 0;
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
unsigned char TPciFunction::GetIrq(int index)
{
    TPciIrqRoute *irq = 0;
    unsigned char nr = 0;

    if (FIrqCount)
    {
        if (index < FIrqCount)
            nr = FIrqArr[index];
    }
    else
    {
        if (index == 0)
        {
            irq = GetIrq();

            if (irq)
                nr = (unsigned char)irq->Irq;

            if (!nr)
            {
                nr = ReadConfigByte(PCI_interrupt_line);
                if (nr == 0xFF)
                    nr = 0;
            }
        }
    }

    return nr;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetMsi
#
#   Purpose....: Get MSI
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned char TPciFunction::GetMsi()
{
    if (FMsiBase)
        return (unsigned char)FMsiVectors;
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetMsiX
#
#   Purpose....: Get MSI-X
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned char TPciFunction::GetMsiX()
{
    if (FMsiXBase)
    {
        if (FMsiXVectors < 32)
            return (unsigned char)FMsiXVectors;
        else
            return 32;
    }
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::LockPci
#
#   Purpose....: Lock PCI
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciFunction::LockPci(int issuer, int handle, const char *name)
{
    TPciFunction *func = 0;
    bool ok = false;

    if (handle > 0 && handle <= FFuncCount)
        func = FFuncArr[handle - 1];

    if (func)
        if (func->FIssuer)
            if (!func->IsAllowed(issuer))
                func = 0;

    if (func)
    {
        func->LockPci(issuer, name);
        return true;
    }
    else
        return false;
}

/*##########################################################################
#
#   Name       : TPciFunction::SetupIrq
#
#   Purpose....: SetupIrq
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::SetupIrq(int core, int prio)
{
    unsigned char irq = 0;
    TPciIrqRoute *route = 0;
    short int cntrl;
    int val;
    int bar;
    long long phys;

    if (FMsiBase && FMsiVectors)
    {
        irq = ServUacpiAllocateInts(1, (unsigned char)prio);
        if (irq)
        {
            val = ServUacpiGetMsiAddress(core);
            WriteConfigDword(FMsiBase + 2, val);

            val = ServUacpiGetMsiData(irq);

            cntrl = ReadConfigWord(FMsiBase);
            if (cntrl & 0x80)
            {
                WriteConfigDword(FMsiBase + 6, 0);
                WriteConfigWord(FMsiBase + 10, (short int)val);

                if (cntrl & 0x100)
                    WriteConfigDword(FMsiBase + 14, 0);
            }
            else
            {
                WriteConfigWord(FMsiBase + 6, (short int)val);

                if (cntrl & 0x100)
                    WriteConfigDword(FMsiBase + 10, 0);
            }

            FUseMsi = true;
        }
    }

    if (FMsiXBase && FMsiXVectors && !irq)
    {
        val = ReadConfigDword(FMsiXBase + 2);
        bar = val & 7;
        val &= 0xFFFFFFF8;

        if (bar < 6)
            phys = FBarArr[bar];
        else
            phys = 0;

        if (phys & 1)
            phys = 0;

        if (phys)
        {
            phys &= 0xFFFFFFFFFFFFFFF0LL;
            phys += val;

            irq = ServUacpiAllocateInts(1, prio);
            if (irq)
            {
                FMsiXVectorArr = (int *)ServUacpiMap(phys, 16);

                val = ServUacpiGetMsiAddress(core);
                FMsiXVectorArr[0] = val;
                FMsiXVectorArr[1] = 0;

                val = ServUacpiGetMsiData(irq);
                FMsiXVectorArr[2] = val;
                FMsiXVectorArr[3] = 0;

                FUseMsiX = true;
            }
        }
    }

    if (!irq)
    {
        route = GetIrq();

        if (route)
            irq = (unsigned char)route->Irq;

        if (!irq)
        {
            irq = ReadConfigByte(PCI_interrupt_line);
            if (irq == 0xFF)
                irq = 0;
        }
    }

    if (irq)
    {
        FIrqCount = 1;
        FIrqArr[0] = irq;
    }

    if (FUseMsi || FUseMsiX)
        return irq | 0x200;
    else
    {
        if (irq)
            return irq | 0x100;
        else
            return 0;
    }
}

/*##########################################################################
#
#   Name       : TPciFunction::SetupMsi
#
#   Purpose....: SetupMsi
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::SetupMsi(int core, int prio, int vectors)
{
    return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::EnableMsi
#
#   Purpose....: Enable Msi
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::EnableMsi()
{
    short int cntrl;

    if (FUseMsi)
    {
        cntrl = ReadConfigWord(FMsiBase);
        cntrl &= 0xFF8F;
        cntrl |= 1;
        WriteConfigDword(FMsiBase, cntrl);
    }

    if (FUseMsiX)
    {
        cntrl = ReadConfigWord(FMsiXBase);
        cntrl &= 0x3FFF;
        cntrl |= 0x8000;
        WriteConfigWord(FMsiXBase, cntrl);
    }
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
    char pin = ReadConfigByte(PCI_interrupt_pin);

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

    ch = ReadConfigByte(PCI_status_reg);

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

    if (FOwnerName)
    {
        size = strlen(FOwnerName);
        strncpy(buf, FOwnerName, maxsize);
        return size + 1;
    }

    if (FNode)
    {
        path = uacpi_namespace_node_generate_absolute_path(FNode);
        size = strlen(path);
        strncpy(buf, path, maxsize);
        uacpi_free_absolute_path(path);
        return size + 1;
    }

    buf[0] = 0;
    return 1;
}

/*##########################################################################
#
#   Name       : TPciFunction::PowerOn
#
#   Purpose....: Set power state to D0
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::PowerOn()
{
    short int reg;
    short int val;

    if (FPciDevice)
        FPciDevice->PowerOn();

    reg = GetCap(1);
    if (reg > 0)
    {
        val = ReadConfigWord(reg + 4);
        WriteConfigWord(reg + 4, 0x8000);

        if (val & 3)
            RdosWaitMilli(10);
    }

    val = ReadConfigWord(PCI_command_reg);
    val |= 7;
    WriteConfigWord(PCI_command_reg, val);
}

/*##########################################################################
#
#   Name       : TPciFunction::IsAllowed
#
#   Purpose....: Check if issuer is allowed to access function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciFunction::IsAllowed(int issuer)
{
    if (FIssuer)
    {
        if (FIssuer == issuer)
            return true;
        else
            return false;
    }
    else
        return true;
}

/*##########################################################################
#
#   Name       : TPciFunction::LockPci
#
#   Purpose....: Lock
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::LockPci(int issuer, const char *name)
{
    int size;

    FIssuer = issuer;

    if (FOwnerName)
        delete FOwnerName;

    size = strlen(name);
    FOwnerName = new char[size + 1];
    strcpy(FOwnerName, name);

    PowerOn();
}

/*##########################################################################
#
#   Name       : TPciFunction::UnlockPci
#
#   Purpose....: Unlock
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciFunction::UnlockPci()
{
    FIssuer = 0;

    if (FOwnerName)
        delete FOwnerName;

    FOwnerName = 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::IsPciLocked
#
#   Purpose....: Check if locked
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciFunction::IsPciLocked()
{
    if (FIssuer)
        return true;
    else
        return false;
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
