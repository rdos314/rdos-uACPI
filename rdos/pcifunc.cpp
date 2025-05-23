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
	FDevice = 0;
	FFunction = 0;
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
TPciFunction::TPciFunction(TPciDevice *device, int function)
{
	FDevice = device;
	FFunction = function;
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
	FDevice = device;
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
	if (FDevice)
		return FDevice->GetSegment();
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
	if (FDevice)
		return FDevice->GetBus();
	else
		return 0;
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
int TPciFunction::GetDevice()
{
	if (FDevice)
		return FDevice->GetDevice();
	else
		return 0;
}

/*##########################################################################
#
#   Name       : TPciFunction::GetFunction
#
#   Purpose....: Get PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciFunction::GetFunction()
{
	return FFunction;
}

/*##########################################################################
#
#   Name       : TPciFunction::Check
#
#   Purpose....: Check if this device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciFunction::Check(uacpi_namespace_node *node, uacpi_namespace_node_info *info)
{
	int device = (info->adr >> 16) & 0xFFFF;
	int function = info->adr & 0xFFFF;
	
	if (FDevice)
		if (device == FDevice->GetDevice() && function == FFunction)
			return true;
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
char TPciFunction::ReadConfigByte(char reg)
{
	if (FDevice)
		return FDevice->ReadConfigByte(FFunction, reg);
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
	if (FDevice)
		return FDevice->ReadConfigWord(FFunction, reg);
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
	if (FDevice)
		return FDevice->ReadConfigDword(FFunction, reg);
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
	if (FDevice)
		FDevice->WriteConfigByte(FFunction, reg, val);
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
	if (FDevice)
		FDevice->WriteConfigWord(FFunction, reg, val);
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
	if (FDevice)
		FDevice->WriteConfigDword(FFunction, reg, val);
}
