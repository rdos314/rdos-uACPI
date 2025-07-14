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
# obj.cpp
# ACPI object
#
########################################################################*/

#include <memory.h>
#include "obj.h"

int TAcpiObject::FObjCount = 0;
int TAcpiObject::FObjSize = 0;
TAcpiObject **TAcpiObject::FObjArr = 0;

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
TAcpiObject::TAcpiObject()
{
    FParent = 0;
    FNode = 0;
    FInfo = 0;
    FArr = 0;
    FCount = 0;
    FSize = 0;

    FName[0] = 0;

    Add(this);
}

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

    Add(this);
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
#   Name       : TAcpiObject::IsPciFunction
#
#   Purpose....: Is PCI function?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TAcpiObject::IsPciFunction()
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
#   Name       : TAcpiObject::Count
#
#   Purpose....: Get object count
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TAcpiObject::Count()
{
    return FObjCount;
}

/*##########################################################################
#
#   Name       : TAcpiObject::Get
#
#   Purpose....: Get object #
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiObject *TAcpiObject::Get(int index)
{
    if (index >= 0 && index < FObjCount)
        return FObjArr[index];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TAcpiObject::Add
#
#   Purpose....: Add object
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TAcpiObject::Add(TAcpiObject *obj)
{
    TAcpiObject **arr;
    int size;
    int i;

    if (FObjSize == FObjCount)
    {
        if (FObjSize)
        {
            size = 2 * FObjSize;
            arr = new TAcpiObject *[size];

            for (i = 0; i < FObjSize; i++)
                arr[i] = FObjArr[i];

            delete FObjArr;
            FObjArr = arr;
            FObjSize = size;
        }
        else
        {
            FObjSize = 4;
            FObjArr = new TAcpiObject *[FObjSize];
        }
    }
    FObjArr[FObjCount] = obj;
    FObjCount++;
}

/*##########################################################################
#
#   Name       : TAcpiObject::FindPciFunction
#
#   Purpose....: Find PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiObject *TAcpiObject::FindPciFunction(int device, int function)
{
    return 0;
}

/*##########################################################################
#
#   Name       : TAcpiObject::GetAcpiParent
#
#   Purpose....: Get ACPI parent
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiObject *TAcpiObject::GetAcpiParent()
{
    return FParent;
}

/*##########################################################################
#
#   Name       : TAcpiObject::SetAcpiParent
#
#   Purpose....: Set ACPI parent
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TAcpiObject::SetAcpiParent(TAcpiObject *parent)
{
    FParent = parent;
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
