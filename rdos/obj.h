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
# obj.h
# ACPI object
#
########################################################################*/

#ifndef _OBJ_H
#define _OBJ_H

#include <uacpi/uacpi.h>
#include <uacpi/utilities.h>
#include "str.h"

class TAcpiObject
{
public:
    TAcpiObject();
    TAcpiObject(TAcpiObject *parent);
    virtual ~TAcpiObject();

    virtual bool IsDevice();
    virtual bool IsPciFunction();
    virtual bool IsPciBridge();
    virtual bool IsProcessor();
    virtual void Setup(uacpi_namespace_node *node, uacpi_namespace_node_info *info);
    virtual void Update();
    virtual TAcpiObject *FindPciFunction(int device, int function);

    static int Count();
    static TAcpiObject *Get(int index);

    TAcpiObject *GetAcpiParent();
    void SetAcpiParent(TAcpiObject *parent);
    void AddObject(TAcpiObject *obj);
    int EvalInt(int def);
    const char *GetName();
    TAcpiObject *Find(const char *name);

protected:
    void Add(TAcpiObject *obj);

    static int FObjCount;
    static int FObjSize;
    static TAcpiObject **FObjArr;

    TAcpiObject *FParent;
    uacpi_namespace_node *FNode;
    uacpi_namespace_node_info *FInfo;

    char FName[5];

    int FSize;
    int FCount;
    TAcpiObject **FArr;
};

#endif

