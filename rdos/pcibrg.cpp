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
# pcibrg.cpp
# PCI bridge device
#
########################################################################*/

#include <uacpi/resources.h>
#include "acpi.h"
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

char in_dword(int port);
#pragma aux in_dword = \
	"in eax,dx" \
    parm [ edx ] \
	value [ al ]

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
#   Name       : TPciBridge::TPciBridge
#
#   Purpose....: Constructor for TPciBridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciBridge::TPciBridge(TAcpiObject *parent, int bus, int device, int function)
 : TPciDevice(parent, device, function)
{
	FSeg = 0;
	FBus = bus;
	FIo = 0xCF8;
}

/*##########################################################################
#
#   Name       : TPciBridge::~TPciBridge
#
#   Purpose....: Destructor for TPciBridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciBridge::~TPciBridge()
{
}

/*##########################################################################
#
#   Name       : TPciBridge::IsPciBridge
#
#   Purpose....: Is PCI bridge?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciBridge::IsPciBridge()
{
	return true;
}

/*##########################################################################
#
#   Name       : TPciBridge::GetBridgeBus
#
#   Purpose....: Get my PCI bus
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciBridge::GetBridgeBus()
{
	return FBus;
}

/*##########################################################################
#
#   Name       : TPciBridge::GetBridgeSegment
#
#   Purpose....: Get my PCI segment
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciBridge::GetBridgeSegment()
{
	return FSeg;
}

/*##########################################################################
#
#   Name       : TPciBridge::Check
#
#   Purpose....: Check for my bridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciBridge::Check(uacpi_namespace_node *node, uacpi_namespace_node_info *info)
{
	uacpi_status ret;
	uacpi_resources *resources;
	uacpi_resource *resource;
	bool done = false;

	ret = uacpi_get_current_resources(node, &resources);
	if (ret == UACPI_STATUS_OK)
	{
		resource = resources->entries;
		while (!done && resource->type != UACPI_RESOURCE_TYPE_END_TAG)
		{
			switch (resource->type)
			{					
				case UACPI_RESOURCE_TYPE_ADDRESS16:
					if (resource->address16.common.type == UACPI_RANGE_BUS)
						if (resource->address16.minimum == FBus)
							done = true;
					break;
					
				case UACPI_RESOURCE_TYPE_ADDRESS32:
					if (resource->address32.common.type == UACPI_RANGE_BUS)
						if (resource->address32.minimum == FBus)
							done = true;
					break;
				
				default:
					break;
			}
			resource = UACPI_NEXT_RESOURCE(resource);
		}
	}	

	uacpi_free_resources(resources);
	return done;
}

/*##########################################################################
#
#   Name       : TPciBridge::Setup
#
#   Purpose....: Setup
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::Setup(uacpi_namespace_node *node, uacpi_namespace_node_info *info)
{
	uacpi_status ret;
	uacpi_resources *resources;
	uacpi_resource *resource;
	unsigned long long val;

    TAcpiObject::Setup(node, info);

	ret = uacpi_eval_simple_integer(node, "_BBN", &val);
	if (ret == UACPI_STATUS_OK)
		FBus = (int)val;
	
	ret = uacpi_get_current_resources(FNode, &resources);
	if (ret == UACPI_STATUS_OK)
	{
		resource = resources->entries;
		while (resource->type != UACPI_RESOURCE_TYPE_END_TAG)
		{
			switch (resource->type)
			{				
				case UACPI_RESOURCE_TYPE_IO:
					FIo = resource->io.minimum;
					break;
			
				default:
					break;
			}
			resource = UACPI_NEXT_RESOURCE(resource);
		}
	}	
	uacpi_free_resources(resources);

	ServUacpiEnableIo(FIo, 4);	
	ServUacpiEnableIo(FIo + 4, 4);	
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
char TPciBridge::ReadConfigByte(TPciDevice *dev, char reg)
{
	int ads;

	ads = 0x80000000;
	ads |= FBus << 16;
	ads |= (dev->GetDevice() & 0x1F) << 11;
	ads |= (dev->GetFunction() & 0x7) << 8;
	ads |= reg;
	out_dword(FIo, ads & 0xFFFFFFFC);
	return in_byte(FIo + 4 + (ads & 3));
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
short TPciBridge::ReadConfigWord(TPciDevice *dev, char reg)
{
	int ads;

	ads = 0x80000000;
	ads |= FBus << 16;
	ads |= (dev->GetDevice() & 0x1F) << 11;
	ads |= (dev->GetFunction() & 0x7) << 8;
	ads |= reg;
	out_dword(FIo, ads & 0xFFFFFFFC);
	return in_word(FIo + 4 + (ads & 3));
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
int TPciBridge::ReadConfigDword(TPciDevice *dev, char reg)
{
	int ads;

	ads = 0x80000000;
	ads |= FBus << 16;
	ads |= (dev->GetDevice() & 0x1F) << 11;
	ads |= (dev->GetFunction() & 0x7) << 8;
	ads |= reg;
	out_dword(FIo, ads & 0xFFFFFFFC);
	return in_dword(FIo + 4 + (ads & 3));
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
void TPciBridge::WriteConfigByte(TPciDevice *dev, char reg, char val)
{
	int ads;

	ads = 0x80000000;
	ads |= FBus << 16;
	ads |= (dev->GetDevice() & 0x1F) << 11;
	ads |= (dev->GetFunction() & 0x7) << 8;
	ads |= reg;
	out_dword(FIo, ads & 0xFFFFFFFC);
	out_byte(FIo + 4 + (ads & 3), val);
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
void TPciBridge::WriteConfigWord(TPciDevice *dev, char reg, short val)
{
	int ads;

	ads = 0x80000000;
	ads |= FBus << 16;
	ads |= (dev->GetDevice() & 0x1F) << 11;
	ads |= (dev->GetFunction() & 0x7) << 8;
	ads |= reg;
	out_dword(FIo, ads & 0xFFFFFFFC);
	out_word(FIo + 4 + (ads & 3), val);
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
void TPciBridge::WriteConfigDword(TPciDevice *dev, char reg, int val)
{
	int ads;

	ads = 0x80000000;
	ads |= FBus << 16;
	ads |= (dev->GetDevice() & 0x1F) << 11;
	ads |= (dev->GetFunction() & 0x7) << 8;
	ads |= reg;
	out_dword(FIo, ads & 0xFFFFFFFC);
	out_dword(FIo + 4 + (ads & 3), val);
}
