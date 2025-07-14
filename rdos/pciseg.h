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
# pciseg.h
# PCI segment
#
########################################################################*/

#ifndef _PCI_SEGMENT_H
#define _PCI_SEGMENT_H

#include <uacpi/tables.h>

class TPciBridge;

class TPciSegment
{
public:
    TPciSegment(int segment);
    TPciSegment(struct acpi_mcfg_allocation *mfcg);
    virtual ~TPciSegment();

    int GetSegment();
    void Add(TPciBridge *bridge, int bus);
    TPciBridge *Add(int bus);
    TPciBridge *Get(int bus);

    bool HasDev(TPciBridge *bridge, int device);

    int CalcOffs(int bus, int dev, int func, int reg);
    int GetHandle(int bus, int dev, int func);

    char ReadConfigByte(int offs);
    short ReadConfigWord(int offs);
    int ReadConfigDword(int offs);
    void WriteConfigByte(int offs, char val);
    void WriteConfigWord(int offs, short val);
    void WriteConfigDword(int offs, int val);

    char ReadConfigByte(int handle, int reg);
    short ReadConfigWord(int handle, int reg);
    int ReadConfigDword(int handle, int reg);
    void WriteConfigByte(int handle, int reg, char val);
    void WriteConfigWord(int handle, int reg, short val);
    void WriteConfigDword(int handle, int reg, int val);

protected:
    void Init();

    int FSegment;
    int FStartBus;
    int FBusCount;
    char *FMem;
    int FIo;
    TPciBridge **FBusArr;
};

#endif
