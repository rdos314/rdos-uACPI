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
# pcidev.cpp
# PCI device
#
########################################################################*/

#include "pci.h"
#include "pcidev.h"
#include "pcibrg.h"

/*##########################################################################
#
#   Name       : TPciDevice::TPciDevice
#
#   Purpose....: Constructor for TPciDevice
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciDevice::TPciDevice(TPciBridge *parent, int device)
{
    int i;
	
    FParent = parent;
    FDevice = device;
	
    for (i = 0; i < 8; i++)
        FFuncArr[i] = 0;
}

/*##########################################################################
#
#   Name       : TPciDevice::~TPciDevice
#
#   Purpose....: Destructor for TPciDevice
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciDevice::~TPciDevice()
{
}

/*##########################################################################
#
#   Name       : TPciDevice::Find
#
#   Purpose....: Find PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiObject *TPciDevice::Find(int device, int function)
{
    if (FParent)
        return FParent->FindPciFunction(device, function);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciDevice::AddBridge
#
#   Purpose....: Add bridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::AddBridge(TPciBridge *bridge)
{
    FParent->AddBridge(bridge);
}

/*##########################################################################
#
#   Name       : TPciDevice::AddFunction
#
#   Purpose....: Add function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciFunction *TPciDevice::AddFunction(int function, int vendor_device)
{
    TPciFunction *func;
    TPciBridge *bridge;
    int bus;
    unsigned char class_code = (unsigned char)ReadConfigByte(function, PCI_classcode);
    unsigned char sub_class = (unsigned char)ReadConfigByte(function, PCI_subclass);
        
    if (class_code == 6 && sub_class == 4)
    {
        bus = (unsigned char)ReadConfigByte(function, 26);
        bridge = new TPciBridge(FParent, bus, this, function, vendor_device, class_code, sub_class);
        bridge->ScanForDevices();
        AddBridge(bridge);
        func = bridge;
    }
    else
        func = new TPciFunction(this, function, vendor_device, class_code, sub_class);
    
    return func;
}

/*##########################################################################
#
#   Name       : TPciDevice::ScanForFunctions
#
#   Purpose....: Scan for functions
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::ScanForFunctions()
{
    int i;
    int val;
    char typ;

    val = ReadConfigDword(0, PCI_vendorID);
    if (val == 0xFFFFFFFF)
    {
        for (i = 1; i < 8; i++)
        {
            val = ReadConfigDword(i, PCI_vendorID);
            if (val != 0xFFFFFFFF)
                FFuncArr[i] = AddFunction(i, val);
        }
    }
    else
    {
        FFuncArr[0] = AddFunction(0, val);

        typ = ReadConfigByte(0, PCI_header_type);
        if (typ & 0x80)
        {
            for (i = 1; i < 8; i++)
            {
                val = ReadConfigDword(i, PCI_vendorID);
                if (val != 0xFFFFFFFF)
                    FFuncArr[i] = AddFunction(i, val);
            }
        }
    }
}

/*##########################################################################
#
#   Name       : TPciDevice::GetSegment
#
#   Purpose....: Get PCI segment
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciDevice::GetSegment()
{
    return FParent->GetBridgeSegment();
}

/*##########################################################################
#
#   Name       : TPciDevice::GetBus
#
#   Purpose....: Get PCI bus
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciDevice::GetBus()
{
    return FParent->GetBridgeBus();
}

/*##########################################################################
#
#   Name       : TPciDevice::GetDevice
#
#   Purpose....: Get PCI device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciDevice::GetDevice()
{
    return FDevice;
}

/*##########################################################################
#
#   Name       : TPciDevice::GetFunction
#
#   Purpose....: Get PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciFunction *TPciDevice::GetFunction(int function)
{
    if (function >= 0 && function < 8)
        return FFuncArr[function];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciDevice::ReadConfigByte
#
#   Purpose....: Read config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char TPciDevice::ReadConfigByte(int func, char reg)
{
    return FParent->ReadConfigByte(this, func, reg);
}

/*##########################################################################
#
#   Name       : TPciDevice::ReadConfigWord
#
#   Purpose....: Read config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short TPciDevice::ReadConfigWord(int func, char reg)
{
    return FParent->ReadConfigWord(this, func, reg);
}

/*##########################################################################
#
#   Name       : TPciDevice::ReadConfigDword
#
#   Purpose....: Read config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciDevice::ReadConfigDword(int func, char reg)
{
    return FParent->ReadConfigDword(this, func, reg);
}

/*##########################################################################
#
#   Name       : TPciDevice::WriteConfigByte
#
#   Purpose....: Write config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::WriteConfigByte(int func, char reg, char val)
{
    FParent->WriteConfigByte(this, func, reg, val);
}

/*##########################################################################
#
#   Name       : TPciDevice::WriteConfigWord
#
#   Purpose....: Write config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::WriteConfigWord(int func, char reg, short val)
{
    FParent->WriteConfigWord(this, func, reg, val);
}

/*##########################################################################
#
#   Name       : TPciDevice::WriteConfigDword
#
#   Purpose....: Write config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::WriteConfigDword(int func, char reg, int val)
{
    FParent->WriteConfigDword(this, func, reg, val);
}
