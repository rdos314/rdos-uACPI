/*#######################################################################
# RDOS operating system
# Copyright (C) 1988-2025, Leif Ekblad
#
# MIT License
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
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
