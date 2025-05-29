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
# dev.cpp
# ACPI device
#
########################################################################*/

#include "dev.h"

int TAcpiDevice::FDeviceCount = 0;
int TAcpiDevice::FDeviceSize = 0;
TAcpiDevice **TAcpiDevice::FDeviceArr = 0;

/*##########################################################################
#
#   Name       : TAcpiDevice::TAcpiDevice
#
#   Purpose....: Constructor for TAcpiDevice
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiDevice::TAcpiDevice(TAcpiObject *parent)
  : TAcpiObject(parent)
{
    Add(this);
}

/*##########################################################################
#
#   Name       : TAcpiDevice::TAcpiDevice
#
#   Purpose....: Constructor for TAcpiDevice
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiDevice::TAcpiDevice()
{
    Add(this);
}

/*##########################################################################
#
#   Name       : TAcpiDevice::~TAcpiDevice
#
#   Purpose....: Destructor for TAcpiDevice
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiDevice::~TAcpiDevice()
{
}

/*##########################################################################
#
#   Name       : TAcpiDevice::IsDevice
#
#   Purpose....: Is device?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TAcpiDevice::IsDevice()
{
    return true;
}

/*##########################################################################
#
#   Name       : TAcpiDevice::Count
#
#   Purpose....: Get device count
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TAcpiDevice::Count()
{
    return FDeviceCount;
}

/*##########################################################################
#
#   Name       : TAcpiDevice::Get
#
#   Purpose....: Get device #
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiDevice *TAcpiDevice::Get(int index)
{
    if (index >= 0 && index < FDeviceCount)
        return FDeviceArr[index];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TAcpiDevice::Add
#
#   Purpose....: Add device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TAcpiDevice::Add(TAcpiDevice *dev)
{
    TAcpiDevice **arr;
    int size;
    int i;

    if (FDeviceSize == FDeviceCount)
    {
        if (FDeviceSize)
        {
            size = 2 * FDeviceSize;
            arr = new TAcpiDevice *[size];

            for (i = 0; i < FDeviceSize; i++)
                arr[i] = FDeviceArr[i];

            delete FDeviceArr;
            FDeviceArr = arr;
            FDeviceSize = size;
        }
        else
        {
            FDeviceSize = 4;
            FDeviceArr = new TAcpiDevice *[FDeviceSize];
        }
    }
    FDeviceArr[FDeviceCount] = dev;
    FDeviceCount++;
}

/*##########################################################################
#
#   Name       : TAcpiDevice::IsDevice
#
#   Purpose....: Is device?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TAcpiDevice::EvalObjectInt(const char *name, int def)
{
    TAcpiObject *obj = Find(name);

    if (obj)
        return obj->EvalInt(def);
    else
        return def;
}
