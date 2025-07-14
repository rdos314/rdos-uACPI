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
