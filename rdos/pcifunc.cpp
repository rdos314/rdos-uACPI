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

#include "pci.h"
#include "pcifunc.h"
#include "pcidev.h"

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
#   Name       : TPciFunction::ReadConfigByte
#
#   Purpose....: Read config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char TPciFunction::ReadConfigByte(char reg)
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
short TPciFunction::ReadConfigWord(char reg)
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
int TPciFunction::ReadConfigDword(char reg)
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
void TPciFunction::WriteConfigByte(char reg, char val)
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
void TPciFunction::WriteConfigWord(char reg, short val)
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
void TPciFunction::WriteConfigDword(char reg, int val)
{
    if (FPciDevice)
        FPciDevice->WriteConfigDword(FPciFunction, reg, val);
}
