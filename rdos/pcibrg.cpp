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
#include "pcibrg.h"

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
#   Name       : TPciBridge::Update
#
#   Purpose....: Update
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::Update()
{
	uacpi_status ret;
	uacpi_resources *resources;
	uacpi_resource *resource;
	unsigned long long val;

	FBus = EvalObjectInt("_BBN", 0);

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
			
				case UACPI_RESOURCE_TYPE_ADDRESS16:
					if (resource->address16.common.type == UACPI_RANGE_BUS)
						FBus = resource->address16.minimum;
					break;
					
				case UACPI_RESOURCE_TYPE_ADDRESS32:
					if (resource->address32.common.type == UACPI_RANGE_BUS)
						FBus = resource->address32.minimum;
					break;
				
				default:
					break;
			}
			resource = UACPI_NEXT_RESOURCE(resource);
		}
	}	
	uacpi_free_resources(resources);
}
