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
#include <uacpi/tables.h>
#include "acpi.h"
#include "rdos.h"
#include "dev.h"
#include "pcidev.h"
#include "pcibrg.h"
#include "pciseg.h"
#include "cpu.h"

extern "C"
{

int OpenPci(int seg, int bus, int dev, int func);
char ReadPci8(int segment, int handle, int reg);
short ReadPci16(int segment, int handle, int reg);
int ReadPci32(int segment, int handle, int reg);
void WritePci8(int segment, int handle, int reg, char val);
void WritePci16(int segment, int handle, int reg, short val);
void WritePci32(int segment, int handle, int reg, int val);

int FindClass(int start, int pci_class, int pci_sub_class);
int FindClassInterface(int start, int pci_class, int pci_sub_class, int pci_interface);

extern int WaitForMsg();
#pragma aux WaitForMsg value [eax]

};

static TAcpiObject *ObjArr[256] = {0};
static TPciSegment *PciSegArr[256] = {0};

/*##########################################################################
#
#   Name       : OpenPsi
#
#   Purpose....: OpenPci
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int OpenPci(int index, int bus, int dev, int func)
{
    TPciSegment *seg;

    if (index < 0 || index > 255)
        return -1;

    seg = PciSegArr[index];

    if (!seg && index == 0)
    {
        seg = new TPciSegment(0);
        PciSegArr[index] = seg;
    }

    if (seg)
        return seg->GetHandle(bus, dev, func);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : ReadPci8
#
#   Purpose....: Read pci byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char ReadPci8(int index, int handle, int reg)
{
    TPciSegment *seg = PciSegArr[index];

    if (seg)
        return seg->ReadConfigByte(handle, reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : ReadPci16
#
#   Purpose....: Read pci word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short ReadPci16(int index, int handle, int reg)
{
    TPciSegment *seg = PciSegArr[index];

    if (seg)
        return seg->ReadConfigWord(handle, reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : ReadPci32
#
#   Purpose....: Read pci dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int ReadPci32(int index, int handle, int reg)
{
    TPciSegment *seg = PciSegArr[index];

    if (seg)
        return seg->ReadConfigDword(handle, reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : WritePci8
#
#   Purpose....: Write pci byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void WritePci8(int index, int handle, int reg, char val)
{
    TPciSegment *seg = PciSegArr[index];

    if (seg)
        seg->WriteConfigByte(handle, reg, val);
}

/*##########################################################################
#
#   Name       : WritePci16
#
#   Purpose....: Write pci word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void WritePci16(int index, int handle, int reg, short val)
{
    TPciSegment *seg = PciSegArr[index];

    if (seg)
        seg->WriteConfigWord(handle, reg, val);
}

/*##########################################################################
#
#   Name       : WritePci32
#
#   Purpose....: Write pci dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void WritePci32(int index, int handle, int reg, int val)
{
    TPciSegment *seg = PciSegArr[index];

    if (seg)
        seg->WriteConfigDword(handle, reg, val);
}

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
#   Name       : ProcessEcam
#
#   Purpose....: Process ecam
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void ProcessEcam()
{
    int i;
    int mcfg_size;
    uacpi_table mcfg_table;
    uacpi_status status = uacpi_table_find_by_signature(ACPI_MCFG_SIGNATURE, &mcfg_table);
    struct acpi_mcfg *mcfg = (struct acpi_mcfg *)mcfg_table.ptr;
    TPciSegment *seg;
    int index;

    if (status == UACPI_STATUS_OK)
    {
        mcfg_size = (mcfg->hdr.length - offsetof(struct acpi_mcfg, entries)) / sizeof(struct acpi_mcfg_allocation);
        for (i = 0; i < mcfg_size; i++)
        {
            seg = new TPciSegment(mcfg->entries + i);
            index = seg->GetSegment();
            if (index >= 0 && index < 256)
                PciSegArr[index] = seg;
        }
    }
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
    int index = 0;
    int bus = 0;
    unsigned long long val;
    TPciSegment *seg;
    TPciBridge *bridge = 0;
    uacpi_status ret;

    ret = uacpi_eval_simple_integer(node, "_SEG", &val);
    if (ret == UACPI_STATUS_OK)
        index = (int)val;

    ret = uacpi_eval_simple_integer(node, "_BBN", &val);
    if (ret == UACPI_STATUS_OK)
        bus = (int)val;

    seg = PciSegArr[index];

    if (!seg && index == 0)
    {
        seg = new TPciSegment(0);
        PciSegArr[index] = seg;
    }

    if (seg)
    {
        bridge = new TPciBridge(seg, bus);
        bridge->Setup(node, info);
    }
    else
        printf("Segment not defined %d\r\n", seg);

    return bridge;
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

    ProcessEcam();

    ret = uacpi_namespace_load();
    if (uacpi_unlikely_error(ret))
    {
        printf("uacpi_namespace_load error: %s\n", uacpi_status_to_string(ret));
        return false;
    }

    if (ServUacpiHasApic())
        ret = uacpi_set_interrupt_model(UACPI_INTERRUPT_MODEL_IOAPIC);
    else
        ret = uacpi_set_interrupt_model(UACPI_INTERRUPT_MODEL_PIC);
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
#   Name       : FindClass
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int FindClass(int start, int pci_class, int pci_sub_class)
{
    return 0;
}

/*##########################################################################
#
#   Name       : FindClassInterface
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int FindClassInterface(int start, int pci_class, int pci_sub_class, int pci_interface)
{
    return 0;
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
//    bool start = false;

//    ServUacpiStartPci();

//    while (!start)
//        RdosWaitMilli(50);

    InitAcpi();

    printf("%d processor cores\r\n", TAcpiProcessor::Count());
    printf("%d devices\r\n", TAcpiDevice::Count());
    printf("%d objects\r\n", TAcpiObject::Count());
    printf("%d PCI functions\r\n", TPciFunction::Count());

    for (;;)
        WaitForMsg();
}
