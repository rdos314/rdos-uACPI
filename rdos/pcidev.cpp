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

#include <stdio.h>
#include "pci.h"
#include "pcidev.h"
#include "pcibrg.h"

/*##########################################################################
#
#   Name       : TPciIrqRoute::TPciIrqRoute
#
#   Purpose....: Constructor for TPciIrqRoute
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciIrqRoute::TPciIrqRoute(uacpi_pci_routing_table_entry *entry)
{
    uacpi_status ret;
    uacpi_resources *resources;
    uacpi_resource *resource;

    Irq = entry->index;
    Edge = false;
    Level = 0;

    if (entry->source)
    {
        ret = uacpi_get_current_resources(entry->source, &resources);
        if (ret == UACPI_STATUS_OK)
        {
            resource = resources->entries;
            switch (resource->type)
            {
                case UACPI_RESOURCE_TYPE_IRQ:
                    SetupIrq(&resource->irq);
                    break;

                case UACPI_RESOURCE_TYPE_EXTENDED_IRQ:
                    SetupExtIrq(&resource->extended_irq);
                    break;

                default:
                    printf("Unexpected IRQ resource\r\n");
                    break;
            }
            uacpi_free_resources(resources);
        }
        else
            printf("Couldn't get IRQ resources\r\n");
    }
}

/*##########################################################################
#
#   Name       : TPciIrqRoute::~TPciIrqRoute
#
#   Purpose....: Destructor for TPciIrqRoute
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciIrqRoute::~TPciIrqRoute()
{
}

/*##########################################################################
#
#   Name       : TPciIrqRoute::SetupIrq
#
#   Purpose....: Setup using standard IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciIrqRoute::SetupIrq(uacpi_resource_irq *irq)
{
    if (irq->num_irqs >= 1)
    {
        Irq = irq->irqs[0];

        if (irq->triggering == UACPI_TRIGGERING_EDGE)
            Edge = true;

        if (irq->polarity == UACPI_POLARITY_ACTIVE_HIGH)
            Level = 1;
    }
}

/*##########################################################################
#
#   Name       : TPciIrqRoute::SetupExtIrq
#
#   Purpose....: Setup using extended IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciIrqRoute::SetupExtIrq(uacpi_resource_extended_irq *irq)
{
    if (irq->num_irqs < 1)
        printf("No IRQs\r\n");
    else
    {
        Irq = irq->irqs[0];

        if (irq->triggering == UACPI_TRIGGERING_EDGE)
            Edge = true;

        if (irq->polarity == UACPI_POLARITY_ACTIVE_HIGH)
            Level = 1;
    }
}

/*##########################################################################
#
#   Name       : TPciDevice::TPciDevice
#
#   Purpose....: Constructor for TPciDevice
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciDevice::TPciDevice(TPciBridge *parent, int device)
{
    int i;

    FParent = parent;
    FDevice = device;

    for (i = 0; i < 4; i++)
        FIrqArr[i] = 0;

    for (i = 0; i < 8; i++)
        FFuncArr[i] = 0;
}

/*##########################################################################
#
#   Name       : TPciDevice::~TPciDevice
#
#   Purpose....: Destructor for TPciDevice
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciDevice::~TPciDevice()
{
    int i;

    for (i = 0; i < 4; i++)
        if (FIrqArr[i])
            delete FIrqArr[i];

    for (i = 0; i < 8; i++)
        if (FFuncArr[i])
            delete FFuncArr[i];
}

/*##########################################################################
#
#   Name       : TPciDevice::Find
#
#   Purpose....: Find PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiObject *TPciDevice::Find(int device, int function)
{
    if (FParent)
        return FParent->FindPciFunction(device, function);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciDevice::AddBridge
#
#   Purpose....: Add bridge
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::AddBridge(TPciBridge *bridge)
{
    FParent->AddBridge(bridge);
}

/*##########################################################################
#
#   Name       : TPciDevice::AddIrq
#
#   Purpose....: Add IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::AddIrq(uacpi_pci_routing_table_entry *entry)
{
    int pin = entry->pin;

    if (pin >= 0 && pin < 4)
        if (!FIrqArr[pin])
            FIrqArr[pin] = new TPciIrqRoute(entry);
}

/*##########################################################################
#
#   Name       : TPciDevice::AddFunction
#
#   Purpose....: Add function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciFunction *TPciDevice::AddFunction(int function, int vendor_device)
{
    TPciFunction *func;
    TPciBridge *bridge;
    int bus;
    unsigned char class_code = (unsigned char)ReadConfigByte(function, PCI_classcode);
    unsigned char sub_class = (unsigned char)ReadConfigByte(function, PCI_subclass);

    if (class_code == 6 && sub_class == 4)
    {
        bus = (unsigned char)ReadConfigByte(function, 26);
        bridge = new TPciBridge(FParent, bus, this, function, vendor_device, class_code, sub_class);
        bridge->ScanForDevices();
        AddBridge(bridge);
        func = bridge;
    }
    else
        func = new TPciFunction(this, function, vendor_device, class_code, sub_class);

    return func;
}

/*##########################################################################
#
#   Name       : TPciDevice::ScanForFunctions
#
#   Purpose....: Scan for functions
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::ScanForFunctions()
{
    int i;
    int val;
    char typ;

    val = ReadConfigDword(0, PCI_vendorID);
    if (val == 0xFFFFFFFF)
    {
        for (i = 1; i < 8; i++)
        {
            val = ReadConfigDword(i, PCI_vendorID);
            if (val != 0xFFFFFFFF)
                FFuncArr[i] = AddFunction(i, val);
        }
    }
    else
    {
        FFuncArr[0] = AddFunction(0, val);

        typ = ReadConfigByte(0, PCI_header_type);
        if (typ & 0x80)
        {
            for (i = 1; i < 8; i++)
            {
                val = ReadConfigDword(i, PCI_vendorID);
                if (val != 0xFFFFFFFF)
                    FFuncArr[i] = AddFunction(i, val);
            }
        }
    }
}

/*##########################################################################
#
#   Name       : TPciDevice::GetSegment
#
#   Purpose....: Get PCI segment
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciDevice::GetSegment()
{
    return FParent->GetBridgeSegment();
}

/*##########################################################################
#
#   Name       : TPciDevice::GetBus
#
#   Purpose....: Get PCI bus
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciDevice::GetBus()
{
    return FParent->GetBridgeBus();
}

/*##########################################################################
#
#   Name       : TPciDevice::GetDevice
#
#   Purpose....: Get PCI device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciDevice::GetDevice()
{
    return FDevice;
}

/*##########################################################################
#
#   Name       : TPciDevice::GetFunction
#
#   Purpose....: Get PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciFunction *TPciDevice::GetFunction(int function)
{
    if (function >= 0 && function < 8)
        return FFuncArr[function];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciDevice::GetIrq
#
#   Purpose....: Get IRQ for pin
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciIrqRoute *TPciDevice::GetIrq(int pin)
{
    if (pin > 0 && pin <= 4)
        return FIrqArr[pin - 1];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TPciDevice::ReadConfigByte
#
#   Purpose....: Read config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char TPciDevice::ReadConfigByte(int func, int reg)
{
    return FParent->ReadConfigByte(this, func, reg);
}

/*##########################################################################
#
#   Name       : TPciDevice::ReadConfigWord
#
#   Purpose....: Read config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short TPciDevice::ReadConfigWord(int func, int reg)
{
    return FParent->ReadConfigWord(this, func, reg);
}

/*##########################################################################
#
#   Name       : TPciDevice::ReadConfigDword
#
#   Purpose....: Read config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TPciDevice::ReadConfigDword(int func, int reg)
{
    return FParent->ReadConfigDword(this, func, reg);
}

/*##########################################################################
#
#   Name       : TPciDevice::WriteConfigByte
#
#   Purpose....: Write config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::WriteConfigByte(int func, int reg, char val)
{
    FParent->WriteConfigByte(this, func, reg, val);
}

/*##########################################################################
#
#   Name       : TPciDevice::WriteConfigWord
#
#   Purpose....: Write config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::WriteConfigWord(int func, int reg, short val)
{
    FParent->WriteConfigWord(this, func, reg, val);
}

/*##########################################################################
#
#   Name       : TPciDevice::WriteConfigDword
#
#   Purpose....: Write config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::WriteConfigDword(int func, int reg, int val)
{
    FParent->WriteConfigDword(this, func, reg, val);
}

/*##########################################################################
#
#   Name       : TPciDevice::PowerOn
#
#   Purpose....: Set power to D0
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TPciDevice::PowerOn()
{
    FParent->PowerOn();
}
