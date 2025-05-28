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
# pcibrg.cpp
# PCI bridge
#
########################################################################*/

#ifndef _PCI_BRIDGE_H
#define _PCI_BRIDGE_H

#include <uacpi/resources.h>
#include "pcifunc.h"

class TPciBridge : public TPciFunction
{
public:
    TPciBridge(TPciFunction *parent, int bus);
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
    bool Check(uacpi_namespace_node *node, uacpi_namespace_node_info *info);

    char ReadConfigByte(TPciDevice *dev, int func, char reg);
    short ReadConfigWord(TPciDevice *dev, int func, char reg);
    int ReadConfigDword(TPciDevice *dev, int func, char reg);
    void WriteConfigByte(TPciDevice *dev, int func, char reg, char val);
    void WriteConfigWord(TPciDevice *dev, int func, char reg, short val);
    void WriteConfigDword(TPciDevice *dev, int func, char reg, int val);

protected:
    void ParseIrqRouting(uacpi_pci_routing_table_entry *entry);
    void SetupIrqRouting(uacpi_namespace_node *node);

    TPciBridge *FBridgeArr[256];	
    TPciDevice *FDevArr[32];
    int FSeg;
    int FBus;
    int FIo;

private:
    void Init();

};

#endif
