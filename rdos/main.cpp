/*#######################################################################
# RDOS operating system
# Copyright (C) 1988-2012, Leif Ekblad
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
# acpi.cpp
# Main acpi server
#
########################################################################*/

#include <stdio.h>
#include <memory.h>
#include <uacpi/uacpi.h>
#include <uacpi/event.h>
#include <uacpi/utilities.h>
#include <uacpi/resources.h>
#include "acpi.h"
#include "rdos.h"
#include "dev.h"
#include "pcidev.h"
#include "pcibrg.h"
#include "cpu.h"

extern "C"
{

void InitPci();

};

static TAcpiObject *ObjArr[256] = {0};
static TPciBridge *PciRootArr[256] = {0};

/*##########################################################################
#
#   Name       : IsPciRoot
#
#   Purpose....: Check if device is PCI root
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool IsPciRoot(uacpi_id_string *hid)
{
    if (hid->size)
    {
        if (!strcmp(hid->value, "PNP0A03"))
            return true;

        if (!strcmp(hid->value, "PNP0A08"))
            return true;
    }
    return false;
}

/*##########################################################################
#
#   Name       : AddPciRoot
#
#   Purpose....: Add pci root
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiDevice *AddPciRoot(TAcpiObject *parent, uacpi_namespace_node *node, uacpi_namespace_node_info *info)
{
    int seg;
    TPciBridge *bridge = new TPciBridge(0, 0);

    bridge->Setup(node, info);
    seg = bridge->GetBridgeSegment();
    if (PciRootArr[seg])
    {
        printf("Multiple identical PCI segments %d\r\n", seg);
        delete bridge;
        bridge = 0;
    }
    else
        PciRootArr[seg] = bridge;

    return bridge;
}

/*##########################################################################
#
#   Name       : PrintDeviceInfo
#
#   Purpose....: Print device info
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void PrintDeviceInfo(uacpi_namespace_node *node, uacpi_namespace_node_info *info)
{
    uacpi_status ret;
    uacpi_resources *resources;
    uacpi_resource *resource;
    unsigned int start;
    unsigned int end;
    int i;

    ret = uacpi_get_current_resources(node, &resources);
    if (ret == UACPI_STATUS_OK)
    {
        const char *path = uacpi_namespace_node_generate_absolute_path(node);
        printf("Device: %s ", path);

        resource = resources->entries;
        while (resource->type != UACPI_RESOURCE_TYPE_END_TAG)
        {
            switch (resource->type)
            {
                case UACPI_RESOURCE_TYPE_IRQ:
                    printf("IRQ [");
                    for (i = 0; i < resource->irq.num_irqs; i++)
                    {
                        if (i)
                            printf(" ");
                        printf("%d", resource->irq.irqs[i]);
                    }
                    printf("] ");
                    break;

                case UACPI_RESOURCE_TYPE_EXTENDED_IRQ:
                    printf("ExtIRQ ");
                    break;

                case UACPI_RESOURCE_TYPE_DMA:
                    printf("DMA ");
                    break;

                case UACPI_RESOURCE_TYPE_FIXED_DMA:
                    printf("ExtDMA ");
                    break;

                case UACPI_RESOURCE_TYPE_IO:
                    printf("IO [%04hX] ", resource->io.minimum);
                    break;

                case UACPI_RESOURCE_TYPE_FIXED_IO:
                    printf("FixedIO ");
                    break;

                case UACPI_RESOURCE_TYPE_ADDRESS16:
                    printf("Ads16 ");
                    break;

                case UACPI_RESOURCE_TYPE_ADDRESS32:
                    printf("Ads32 ");
                    break;

                case UACPI_RESOURCE_TYPE_ADDRESS64:
                    printf("Ads64 ");
                    break;

                case UACPI_RESOURCE_TYPE_ADDRESS64_EXTENDED:
                    printf("ExtAds64 ");
                    break;

                case UACPI_RESOURCE_TYPE_MEMORY24:
                    printf("Mem24 ");
                    break;

                case UACPI_RESOURCE_TYPE_MEMORY32:
                    printf("Mem32 ");
                    break;

                case UACPI_RESOURCE_TYPE_FIXED_MEMORY32:
                    start = resource->fixed_memory32.address;
                    end = start + resource->fixed_memory32.length - 1;
                    printf("Mem [%08lX-%08lX] ", start, end);
                    break;

                case UACPI_RESOURCE_TYPE_START_DEPENDENT:
                    printf("StartDep ");
                    break;

                case UACPI_RESOURCE_TYPE_END_DEPENDENT:
                    printf("EndDep ");
                    break;

                case UACPI_RESOURCE_TYPE_VENDOR_SMALL:
                    printf("VendSmall ");
                    break;

                case UACPI_RESOURCE_TYPE_VENDOR_LARGE:
                    printf("VendLarge ");
                    break;

                case UACPI_RESOURCE_TYPE_GENERIC_REGISTER:
                    printf("GenReg ");
                    break;

                case UACPI_RESOURCE_TYPE_SERIAL_I2C_CONNECTION:
                    printf("I2C ");
                    break;

                case UACPI_RESOURCE_TYPE_SERIAL_SPI_CONNECTION:
                    printf("SPI ");
                    break;

                case UACPI_RESOURCE_TYPE_SERIAL_UART_CONNECTION:
                    printf("UART ");
                    break;

                case UACPI_RESOURCE_TYPE_SERIAL_CSI2_CONNECTION:
                    printf("CSI2 ");
                    break;

                case UACPI_RESOURCE_TYPE_PIN_FUNCTION:
                    printf("PinFunc ");
                    break;

                case UACPI_RESOURCE_TYPE_PIN_CONFIGURATION:
                    printf("PinConfig ");
                    break;

                case UACPI_RESOURCE_TYPE_PIN_GROUP:
                    printf("PinGroup ");
                    break;

                case UACPI_RESOURCE_TYPE_PIN_GROUP_FUNCTION:
                    printf("PinGroupFunc ");
                    break;

                case UACPI_RESOURCE_TYPE_PIN_GROUP_CONFIGURATION:
                    printf("PinGroupConfig ");
                    break;

                case UACPI_RESOURCE_TYPE_CLOCK_INPUT:
                    printf("ClkInput ");
                    break;
            }
            resource = UACPI_NEXT_RESOURCE(resource);
        }
        printf("\n");
    }
}

/*##########################################################################
#
#   Name       : FindPciDevice
#
#   Purpose....: Find PCI device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiDevice *FindPciDevice(TAcpiObject *parent, uacpi_namespace_node *node, uacpi_namespace_node_info *info)
{
    TAcpiObject *obj;
    TPciFunction *func = (TPciFunction *)parent;
    int device = (info->adr >> 16) & 0xFFFF;
    int function = info->adr & 0xFFFF;

    obj = func->FindPciFunction(device, function);
    if (obj)
    {
        if (!obj->GetAcpiParent())
        {
            obj->SetAcpiParent(parent);
            if (obj->IsPciFunction())
                return (TAcpiDevice *)obj;
        }
    }
    return 0;
}

/*##########################################################################
#
#   Name       : AddProcessor
#
#   Purpose....: Add processor
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiProcessor *AddProcessor(TAcpiObject *parent, uacpi_namespace_node *node, uacpi_namespace_node_info *info)
{
    TAcpiProcessor *proc = new TAcpiProcessor(parent);

    proc->Setup(node, info);
    return proc;
}

/*##########################################################################
#
#   Name       : AddObj
#
#   Purpose....: Add obj
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_iteration_decision AddObj(void *ctx, uacpi_namespace_node *node, uacpi_u32 node_depth)
{
    uacpi_namespace_node_info *info;
    uacpi_status ret;
    TAcpiObject *obj;
    TAcpiDevice *dev;
    TAcpiProcessor *proc;
    char name[5];

    ret = uacpi_get_namespace_node_info(node, &info);
    if (uacpi_unlikely_error(ret))
    {
        const char *path = uacpi_namespace_node_generate_absolute_path(node);
        printf("unable to retrieve node %s information: %s",
                path, uacpi_status_to_string(ret));
        uacpi_free_absolute_path(path);
        return UACPI_ITERATION_DECISION_CONTINUE;
    }

    switch (info->type)
    {
        case UACPI_OBJECT_DEVICE:
            obj = ObjArr[node_depth - 1];
            if (IsPciRoot(&info->hid))
                dev = AddPciRoot(obj, node, info);
            else if (obj)
            {
                if (obj->IsPciFunction())
                    dev = FindPciDevice(obj, node, info);
                else
                    dev = new TAcpiDevice(obj);

                if (dev)
                    dev->Setup(node, info);
            }
            else
                dev = 0;

            if (!dev)
                uacpi_free_namespace_node_info(info);

            ObjArr[node_depth] = dev;
            break;

        case UACPI_OBJECT_PROCESSOR:
            proc = new TAcpiProcessor(ObjArr[node_depth - 1]);
            proc->Setup(node, info);
            ObjArr[node_depth] = proc;
            break;

        default:
            if (ObjArr[node_depth - 1])
            {
                obj = new TAcpiObject(ObjArr[node_depth - 1]);
                obj->Setup(node, info);
                ObjArr[node_depth - 1]->AddObject(obj);
            }
            else
                uacpi_free_namespace_node_info(info);

            ObjArr[node_depth] = 0;
            break;
    }
    return UACPI_ITERATION_DECISION_CONTINUE;
}

/*##########################################################################
#
#   Name       : UpdateObj
#
#   Purpose....: Update obj
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_iteration_decision UpdateObj(void *ctx, uacpi_namespace_node *node, uacpi_u32 node_depth)
{
    TAcpiObject *obj = ObjArr[node_depth];

    if (obj)
        obj->Update();

    ObjArr[node_depth] = 0;

    return UACPI_ITERATION_DECISION_CONTINUE;
}

/*##########################################################################
#
#   Name       : InitAcpi
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool InitAcpi()
{
    uacpi_status ret;

    ret = uacpi_initialize(0);
    if (uacpi_unlikely_error(ret))
    {
        printf("uacpi_initialize error: %s\n", uacpi_status_to_string(ret));
        return false;
    }

    ret = uacpi_namespace_load();
    if (uacpi_unlikely_error(ret))
    {
        printf("uacpi_namespace_load error: %s\n", uacpi_status_to_string(ret));
        return false;
    }

    ret = uacpi_set_interrupt_model(UACPI_INTERRUPT_MODEL_IOAPIC);
    if (uacpi_unlikely_error(ret))
    {
        printf("uacpi_set_interrupt_model: %s\n", uacpi_status_to_string(ret));
        return false;
    }

    ret = uacpi_namespace_initialize();
    if (uacpi_unlikely_error(ret))
    {
        printf("uacpi_namespace_initialize error: %s\n", uacpi_status_to_string(ret));
        return false;
    }

    uacpi_namespace_for_each_child(uacpi_namespace_root(), AddObj, UpdateObj, UACPI_OBJECT_ANY_BIT, UACPI_MAX_DEPTH_ANY, UACPI_NULL);

    ServUacpiStartPci();

    ret = uacpi_finalize_gpe_initialization();
    if (uacpi_unlikely_error(ret))
    {
        printf("uacpi_finalize_gpe_initialization error: %s\n", uacpi_status_to_string(ret));
        return false;
    }

    return true;
}

/*##########################################################################
#
#   Name       : main
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int main(int argc, char **argv)
{
    bool start = false;

    InitPci();
    InitAcpi();

    printf("%d processor cores\r\n", TAcpiProcessor::Count());
    printf("%d devices\r\n", TAcpiDevice::Count());
    printf("%d objects\r\n", TAcpiObject::Count());
    printf("%d PCI functions\r\n", TPciFunction::Count());

    while (!start)
        RdosWaitMilli(50);

    for (;;)
        RdosWaitMilli(250);
}
