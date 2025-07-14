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
# PCI bridge device
#
########################################################################*/

#include <stdio.h>
#include <uacpi/resources.h>
#include "acpi.h"
#include "pcibrg.h"
#include "pcidev.h"

/*##########################################################################
#
#   Name       : TPciBridge::TPciBridge
#
#   Purpose....: Constructor for TPciBridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciBridge::TPciBridge(TPciSegment *seg, int bus)
{
    FSeg = seg;
    FBus = bus;

    Init();
}

/*##########################################################################
#
#   Name       : TPciBridge::TPciBridge
#
#   Purpose....: Constructor for TPciBridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciBridge::TPciBridge(TPciBridge *bridge, int bus, TPciDevice *device, int function, int vendor_device, unsigned char class_code, unsigned char sub_class)
 : TPciFunction(device, function, vendor_device, class_code, sub_class)
{
    FSeg = bridge->FSeg;
    FBus = bus;

    Init();
    FSeg->Add(this, bus);
}

/*##########################################################################
#
#   Name       : TPciBridge::~TPciBridge
#
#   Purpose....: Destructor for TPciBridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciBridge::~TPciBridge()
{
    int i;

    for (i = 0; i < 32; i++)
        if (FDevArr[i])
            delete FDevArr[i];

    for (i = 0; i < 256; i++)
        if (FBridgeArr[i])
            delete FBridgeArr[i];
}

/*##########################################################################
#
#   Name       : TPciBridge::Init
#
#   Purpose....: Init
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::Init()
{
    int i;

    for (i = 0; i < 256; i++)
        FBridgeArr[i] = 0;

    for (i = 0; i < 32; i++)
        FDevArr[i] = 0;
}

/*##########################################################################
#
#   Name       : TPciBridge::IsPciBridge
#
#   Purpose....: Is PCI bridge?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TPciBridge::IsPciBridge()
{
    return true;
}

/*##########################################################################
#
#   Name       : TPciBridge::GetBridgeBus
#
#   Purpose....: Get my PCI bus
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciBridge::GetBridgeBus()
{
    return FBus;
}

/*##########################################################################
#
#   Name       : TPciBridge::GetBridgeSegment
#
#   Purpose....: Get my PCI segment
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciBridge::GetBridgeSegment()
{
    return FSeg->GetSegment();
}

/*##########################################################################
#
#   Name       : TPciBridge::GetBridge
#
#   Purpose....: Get bridge child
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciBridge *TPciBridge::GetBridge(int bus)
{
    if (bus >= 0 && bus < 256)
        return FBridgeArr[bus];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciBridge::FindPciFunction
#
#   Purpose....: Find PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiObject *TPciBridge::FindPciFunction(int device, int function)
{
    TPciDevice *dev = 0;

    if (device >= 0 && device < 32)
        dev = FDevArr[device];

    if (dev)
        return dev->GetFunction(function);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciBridge::AddBridge
#
#   Purpose....: Add bridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::AddBridge(TPciBridge *bridge)
{
    int bus = bridge->GetBridgeBus();

    if (FBridgeArr[bus])
        printf("Duplicate PCI bus %d\r\n", bus);
    else
        FBridgeArr[bus] = bridge;

    if (FPciDevice)
        FPciDevice->AddBridge(bridge);
}

/*##########################################################################
#
#   Name       : TPciBridge::ScanForDevices
#
#   Purpose....: Scan for PCI devices
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::ScanForDevices()
{
    int dev;
    bool found;
    TPciDevice *pdev;

    for (dev = 0; dev < 32; dev++)
    {
        found = FSeg->HasDev(this, dev);

        if (found)
        {
            pdev = new TPciDevice(this, dev);
            pdev->ScanForFunctions();
            FDevArr[dev] = pdev;
        }
    }
}

/*##########################################################################
#
#   Name       : TPciBridge::ParseIrqRouting
#
#   Purpose....: Parse IRQ routing table
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::ParseIrqRouting(uacpi_pci_routing_table_entry *entry)
{
    int dev = (entry->address >> 16) & 0xFFFF;

    if (dev >= 0 && dev < 32)
    {
        if (FDevArr[dev])
            FDevArr[dev]->AddIrq(entry);
    }
}

/*##########################################################################
#
#   Name       : TPciBridge::SetupIrqRouting
#
#   Purpose....: Setup IRQ routing table
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::SetupIrqRouting(uacpi_namespace_node *node)
{
    int i;
    uacpi_pci_routing_table *table;
    uacpi_status ret = uacpi_get_pci_routing_table(node, &table);

    if (ret == UACPI_STATUS_OK)
    {
        for (i = 0; i < table->num_entries; i++)
            ParseIrqRouting(&table->entries[i]);
        uacpi_free_pci_routing_table(table);
    }
}

/*##########################################################################
#
#   Name       : TPciBridge::Setup
#
#   Purpose....: Setup
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::Setup(uacpi_namespace_node *node, uacpi_namespace_node_info *info)
{
    TAcpiObject::Setup(node, info);
    SetupIrqRouting(node);
}

/*##########################################################################
#
#   Name       : TPciBridge::ReadConfigByte
#
#   Purpose....: Read config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char TPciBridge::ReadConfigByte(TPciDevice *dev, int func, int reg)
{
    int offs = FSeg->CalcOffs(FBus, dev->GetDevice(), func, reg);
    return FSeg->ReadConfigByte(offs);
}

/*##########################################################################
#
#   Name       : TPciBridge::ReadConfigWord
#
#   Purpose....: Read config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short TPciBridge::ReadConfigWord(TPciDevice *dev, int func, int reg)
{
    int offs = FSeg->CalcOffs(FBus, dev->GetDevice(), func, reg);
    return FSeg->ReadConfigWord(offs);
}

/*##########################################################################
#
#   Name       : TPciBridge::ReadConfigDword
#
#   Purpose....: Read config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciBridge::ReadConfigDword(TPciDevice *dev, int func, int reg)
{
    int offs = FSeg->CalcOffs(FBus, dev->GetDevice(), func, reg);
    return FSeg->ReadConfigDword(offs);
}

/*##########################################################################
#
#   Name       : TPciBridge::WriteConfigByte
#
#   Purpose....: Write config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::WriteConfigByte(TPciDevice *dev, int func, int reg, char val)
{
    int offs = FSeg->CalcOffs(FBus, dev->GetDevice(), func, reg);
    FSeg->WriteConfigByte(offs, val);
}

/*##########################################################################
#
#   Name       : TPciBridge::WriteConfigWord
#
#   Purpose....: Write config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::WriteConfigWord(TPciDevice *dev, int func, int reg, short val)
{
    int offs = FSeg->CalcOffs(FBus, dev->GetDevice(), func, reg);
    FSeg->WriteConfigWord(offs, val);
}

/*##########################################################################
#
#   Name       : TPciBridge::WriteConfigDword
#
#   Purpose....: Write config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciBridge::WriteConfigDword(TPciDevice *dev, int func, int reg, int val)
{
    int offs = FSeg->CalcOffs(FBus, dev->GetDevice(), func, reg);
    FSeg->WriteConfigDword(offs, val);
}
