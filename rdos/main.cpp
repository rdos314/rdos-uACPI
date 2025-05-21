/*#######################################################################
# RDOS operating system
# Copyright (C) 1988-2012, Leif Ekblad
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
# acpi.cpp
# Main acpi server
#
########################################################################*/

#include <stdio.h>
#include <uacpi/uacpi.h>
#include <uacpi/event.h>
#include <uacpi/utilities.h>
#include "rdos.h"

extern "C" 
{

void InitPci();
char ReadPciByte(unsigned char bus, char device, char function, char reg);
short ReadPciWord(unsigned char bus, char device, char function, char reg);
int ReadPciDword(unsigned char bus, char device, char function, char reg);
void WritePciByte(unsigned char bus, char device, char function, char reg, char val);
void WritePciWord(unsigned char bus, char device, char function, char reg, short val);
void WritePciDword(unsigned char bus, char device, char function, char reg, int val);

};

/*##########################################################################
#
#   Name       : AddAscend
#
#   Purpose....: Add ascend
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_iteration_decision AddAscend(void *ctx, uacpi_namespace_node *node, uacpi_u32 node_depth)
{
    uacpi_namespace_node_info *info;
    uacpi_status ret;

    ret = uacpi_get_namespace_node_info(node, &info);
    if (uacpi_unlikely_error(ret)) 
	{
        const char *path = uacpi_namespace_node_generate_absolute_path(node);
        printf("unable to retrieve node %s information: %s",
                  path, uacpi_status_to_string(ret));
        uacpi_free_absolute_path(path);
        return UACPI_ITERATION_DECISION_CONTINUE;
    }

    if (info->flags & UACPI_NS_NODE_INFO_HAS_HID) {
        // Match the HID against every existing acpi_driver pnp id list
    }

    uacpi_free_namespace_node_info(info);
    return UACPI_ITERATION_DECISION_CONTINUE;
}

/*##########################################################################
#
#   Name       : AddDescend
#
#   Purpose....: Add descend
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_iteration_decision AddDescend(void *ctx, uacpi_namespace_node *node, uacpi_u32 node_depth)
{
    uacpi_namespace_node_info *info;
    uacpi_status ret;

    ret = uacpi_get_namespace_node_info(node, &info);
    if (uacpi_unlikely_error(ret)) 
	{
        const char *path = uacpi_namespace_node_generate_absolute_path(node);
        printf("unable to retrieve node %s information: %s",
                  path, uacpi_status_to_string(ret));
        uacpi_free_absolute_path(path);
        return UACPI_ITERATION_DECISION_CONTINUE;
    }

    if (info->flags & UACPI_NS_NODE_INFO_HAS_HID) {
        // Match the HID against every existing acpi_driver pnp id list
    }

    uacpi_free_namespace_node_info(info);
    return UACPI_ITERATION_DECISION_CONTINUE;
}

/*##########################################################################
#
#   Name       : InitAcpi
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool InitAcpi()
{
	uacpi_status ret;
	
	ret = uacpi_initialize(0);
	if (uacpi_unlikely_error(ret))
	{
		printf("uacpi_initialize error: %s\n", uacpi_status_to_string(ret));
		return false;
	}
	
	ret = uacpi_namespace_load();
	if (uacpi_unlikely_error(ret))
	{
		printf("uacpi_namespace_load error: %s\n", uacpi_status_to_string(ret));
		return false;
	}

	ret = uacpi_namespace_initialize();
	if (uacpi_unlikely_error(ret))
	{
		printf("uacpi_namespace_initialize error: %s\n", uacpi_status_to_string(ret));
		return false;
	}
	ret = uacpi_finalize_gpe_initialization();
	if (uacpi_unlikely_error(ret))
	{
		printf("uacpi_finalize_gpe_initialization error: %s\n", uacpi_status_to_string(ret));
		return false;
	}

	uacpi_namespace_for_each_child(uacpi_namespace_root(), AddDescend, AddAscend, UACPI_OBJECT_ANY_BIT, UACPI_MAX_DEPTH_ANY, UACPI_NULL);
	
	return true;
}

/*##########################################################################
#
#   Name       : main
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int main(int argc, char **argv)
{
	bool start = false;
	
	while (!start)
		RdosWaitMilli(50);

    InitPci();
    InitAcpi();
	
	for (;;)
		RdosWaitMilli(250);
}
