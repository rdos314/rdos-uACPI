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
# cpu.cpp
# ACPI processor
#
########################################################################*/

#include "cpu.h"

int TAcpiProcessor::FProcessorCount = 0;
int TAcpiProcessor::FProcessorSize = 0;
TAcpiProcessor **TAcpiProcessor::FProcessorArr = 0;

/*##########################################################################
#
#   Name       : TAcpiProcessor::TAcpiProcessor
#
#   Purpose....: Constructor for TAcpiProcessor
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiProcessor::TAcpiProcessor(TAcpiObject *parent)
  : TAcpiObject(parent)
{
    Add(this);
}

/*##########################################################################
#
#   Name       : TAcpiProcessor::~TAcpiProcessor
#
#   Purpose....: Destructor for TAcpiProcessor
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiProcessor::~TAcpiProcessor()
{
}

/*##########################################################################
#
#   Name       : TAcpiProcessor::IsProcessor
#
#   Purpose....: Is processor?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TAcpiProcessor::IsProcessor()
{
    return true;
}

/*##########################################################################
#
#   Name       : TAcpiProcessor::Count
#
#   Purpose....: Get processor count
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TAcpiProcessor::Count()
{
    return FProcessorCount;
}

/*##########################################################################
#
#   Name       : TAcpiProcessor::Get
#
#   Purpose....: Get processor #
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiProcessor *TAcpiProcessor::Get(int index)
{
    if (index >= 0 && index < FProcessorCount)
        return FProcessorArr[index];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TAcpiProcessor::Add
#
#   Purpose....: Add processor
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TAcpiProcessor::Add(TAcpiProcessor *proc)
{
    TAcpiProcessor **arr;
    int size;
    int i;

    if (FProcessorSize == FProcessorCount)
    {
        if (FProcessorSize)
        {
            size = 2 * FProcessorSize;
            arr = new TAcpiProcessor *[size];

            for (i = 0; i < FProcessorSize; i++)
                arr[i] = FProcessorArr[i];

            delete FProcessorArr;
            FProcessorArr = arr;
            FProcessorSize = size;
        }
        else
        {
            FProcessorSize = 4;
            FProcessorArr = new TAcpiProcessor *[FProcessorSize];
        }
    }
    FProcessorArr[FProcessorCount] = proc;
    FProcessorCount++;
}
