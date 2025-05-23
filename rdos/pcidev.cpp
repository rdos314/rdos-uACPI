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
TPciDevice::TPciDevice(TAcpiObject *parent, int device, int function)
 : TAcpiDevice(parent)
{
	if (parent && parent->IsPciBridge())
		FParent = (TPciBridge *)parent;
	else
		FParent = 0;
	
	FDevice = device;
	FFunction = function;
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
#   Name       : TPciDevice::IsPciDevice
#
#   Purpose....: Is PCI device?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciDevice::IsPciDevice()
{
	return true;
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
int TPciDevice::GetFunction()
{
	return FFunction;
}

/*##########################################################################
#
#   Name       : TPciDevice::Check
#
#   Purpose....: Check if this device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciDevice::Check(uacpi_namespace_node *node, uacpi_namespace_node_info *info)
{
	int device = (info->adr >> 16) & 0xFFFF;
	int function = info->adr & 0xFFFF;
	
	if (device == FDevice && function == FFunction)
		return true;
	else
		return false;
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
char TPciDevice::ReadConfigByte(char reg)
{
	if (FParent)
		return FParent->ReadConfigByte(this, reg);
	else
		return -1;
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
short TPciDevice::ReadConfigWord(char reg)
{
	if (FParent)
		return FParent->ReadConfigWord(this, reg);
	else
		return -1;
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
int TPciDevice::ReadConfigDword(char reg)
{
	if (FParent)
		return FParent->ReadConfigDword(this, reg);
	else
		return -1;
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
void TPciDevice::WriteConfigByte(char reg, char val)
{
	if (FParent)
		FParent->WriteConfigByte(this, reg, val);
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
void TPciDevice::WriteConfigWord(char reg, short val)
{
	if (FParent)
		FParent->WriteConfigWord(this, reg, val);
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
void TPciDevice::WriteConfigDword(char reg, int val)
{
	if (FParent)
		FParent->WriteConfigDword(this, reg, val);
}
