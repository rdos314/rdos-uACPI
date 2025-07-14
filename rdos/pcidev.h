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
# pcidev.cpp
# PCI device
#
########################################################################*/

#ifndef _PCI_DEV_H
#define _PCI_DEV_H

#include "dev.h"
#include "pcibrg.h"

class TPciIrqRoute
{
public:
    TPciIrqRoute(uacpi_pci_routing_table_entry *entry);
    ~TPciIrqRoute();

    int Irq;
    bool Edge;
    int Level;

protected:
    void SetupIrq(uacpi_resource_irq *irq);
    void SetupExtIrq(uacpi_resource_extended_irq *irq);
};

class TPciFunction;

class TPciDevice
{
public:
    TPciDevice(TPciBridge *parent, int device);
    virtual ~TPciDevice();

    int GetSegment();
    int GetBus();
    int GetDevice();
    TPciIrqRoute *GetIrq(int pin);
    void ScanForFunctions();
    void AddBridge(TPciBridge *bridge);
    void AddIrq(uacpi_pci_routing_table_entry *entry);
    TPciFunction *GetFunction(int function);
    TAcpiObject *Find(int device, int function);

    char ReadConfigByte(int func, int reg);
    short ReadConfigWord(int func, int reg);
    int ReadConfigDword(int func, int reg);
    void WriteConfigByte(int func, int reg, char val);
    void WriteConfigWord(int func, int reg, short val);
    void WriteConfigDword(int func, int reg, int val);

    void PowerOn();

protected:
    TPciFunction *AddFunction(int function, int vendor_device);

    TPciIrqRoute *FIrqArr[4];
    TPciFunction *FFuncArr[8];
    TPciBridge *FParent;
    int FDevice;
};

#endif
