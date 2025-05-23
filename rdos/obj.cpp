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
# obj.cpp
# ACPI object
#
########################################################################*/

#include <memory.h>
#include "obj.h"

/*##########################################################################
#
#   Name       : TAcpiObject::TAcpiObject
#
#   Purpose....: Constructor for TAcpiObject
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiObject::TAcpiObject(TAcpiObject *parent)
{
	FParent = parent;
	FNode = 0;
	FInfo = 0;
	FArr = 0;
	FCount = 0;
	FSize = 0;
	
	FName[0] = 0;
}

/*##########################################################################
#
#   Name       : TAcpiObject::~TAcpiObject
#
#   Purpose....: Destructor for TAcpiObject
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiObject::~TAcpiObject()
{
	int i;

	if (FArr)
	{
		for (i = 0; i < FCount; i++)
			delete FArr[i];

		delete FArr;
	}
	
    uacpi_free_namespace_node_info(FInfo);
}

/*##########################################################################
#
#   Name       : TAcpiObject::IsDevice
#
#   Purpose....: Is device?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TAcpiObject::IsDevice()
{
	return false;
}

/*##########################################################################
#
#   Name       : TAcpiObject::IsPciDevice
#
#   Purpose....: Is PCI device?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TAcpiObject::IsPciDevice()
{
	return false;
}

/*##########################################################################
#
#   Name       : TAcpiObject::IsPciBridge
#
#   Purpose....: Is PCI bridge?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TAcpiObject::IsPciBridge()
{
	return false;
}

/*##########################################################################
#
#   Name       : TAcpiObject::IsProcessor
#
#   Purpose....: Is processor?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TAcpiObject::IsProcessor()
{
	return false;
}

/*##########################################################################
#
#   Name       : TAcpiObject::Setup
#
#   Purpose....: Setup from uACPI
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TAcpiObject::Setup(uacpi_namespace_node *node, uacpi_namespace_node_info *info)
{
	FNode = node;
	FInfo = info;
	
	memcpy(FName, info->name.text, 4);
	FName[4] = 0;
}

/*##########################################################################
#
#   Name       : TAcpiObject::AddObject
#
#   Purpose....: Add object
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TAcpiObject::AddObject(TAcpiObject *obj)
{
	TAcpiObject **arr;
	int size;
	int i;
	
	if (FSize == FCount)
	{
		if (FSize)
		{
			size = 2 * FSize;
			arr = new TAcpiObject *[size];
			
			for (i = 0; i < FSize; i++)
				arr[i] = FArr[i];
			
			delete FArr;
			FArr = arr;
			FSize = size;
		}
		else
		{
			FSize = 4;
			FArr = new TAcpiObject *[FSize];
		}
	}
	FArr[FCount] = obj;
	FCount++;
}

/*##########################################################################
#
#   Name       : TAcpiObject::Update
#
#   Purpose....: Update after all owned objects have been added
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TAcpiObject::Update()
{
}

/*##########################################################################
#
#   Name       : TAcpiObject::GetName
#
#   Purpose....: Get object name
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
const char *TAcpiObject::GetName()
{
	return FName;
}

/*##########################################################################
#
#   Name       : TAcpiObject::Find
#
#   Purpose....: Find child obj
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiObject *TAcpiObject::Find(const char *name)
{
	int i;
	TAcpiObject *obj;

	for (i = 0; i < FCount; i++)
	{
		obj = FArr[i];
		if (!strcmp(name, obj->FName))
			return obj;
	}
	return 0;
}

/*##########################################################################
#
#   Name       : TAcpiObject::EvalInt
#
#   Purpose....: Eval object int
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TAcpiObject::EvalInt(int def)
{
	uacpi_status ret;
	unsigned long long val;

	if (FParent)
	{
		ret = uacpi_eval_simple_integer(FParent->FNode, FName, &val);
		if (ret == UACPI_STATUS_OK)
			return (int)val;
	}
	return def;
}
