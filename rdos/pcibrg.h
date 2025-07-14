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
# pcibrg.cpp
# PCI bridge
#
########################################################################*/

#ifndef _PCI_BRIDGE_H
#define _PCI_BRIDGE_H

#include <uacpi/resources.h>
#include "pcifunc.h"
#include "pciseg.h"

class TPciBridge : public TPciFunction
{
public:
    TPciBridge(TPciSegment *seg, int bus);
    TPciBridge(TPciBridge *bridge, int bus, TPciDevice *device, int function, int vendor_device, unsigned char class_code, unsigned char sub_class);
    virtual ~TPciBridge();

    void AddBridge(TPciBridge *bridge);
    void ScanForDevices();

    virtual bool IsPciBridge();
    virtual void Setup(uacpi_namespace_node *node, uacpi_namespace_node_info *info);
    virtual TAcpiObject *FindPciFunction(int device, int function);

    int GetBridgeSegment();
    int GetBridgeBus();
    TPciBridge *GetBridge(int bus);

    char ReadConfigByte(TPciDevice *dev, int func, int reg);
    short ReadConfigWord(TPciDevice *dev, int func, int reg);
    int ReadConfigDword(TPciDevice *dev, int func, int reg);
    void WriteConfigByte(TPciDevice *dev, int func, int reg, char val);
    void WriteConfigWord(TPciDevice *dev, int func, int reg, short val);
    void WriteConfigDword(TPciDevice *dev, int func, int reg, int val);

protected:
    void ParseIrqRouting(uacpi_pci_routing_table_entry *entry);
    void SetupIrqRouting(uacpi_namespace_node *node);

    TPciSegment *FSeg;
    TPciBridge *FBridgeArr[256];
    TPciDevice *FDevArr[32];
    int FBus;

private:
    void Init();

};

#endif
