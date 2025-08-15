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
#include <uacpi/sleep.h>
#include "acpi.h"
#include "rdos.h"
#include "dev.h"
#include "pcidev.h"
#include "pcibrg.h"
#include "pciseg.h"
#include "cpu.h"
#include "thrstat.h"
#include "section.h"

#define REQ_CREATE_THREAD       1
#define REQ_TERMINATE_THREAD    2
#define REQ_CREATE_PROCESS      3
#define REQ_TERMINATE_PROCESS   4
#define REQ_CREATE_PROGRAM      5
#define REQ_TERMINATE_PROGRAM   6
#define REQ_LOAD_MODULE         7
#define REQ_UNLOAD_MODULE       8

struct TTaskQueueEntry
{
    short int Op;
    short int Spare;
    int Handle;
};

extern "C"
{

int OpenPci(int seg, int bus, int dev, int func);
char ReadPci8(int segment, int handle, int reg);
short ReadPci16(int segment, int handle, int reg);
int ReadPci32(int segment, int handle, int reg);
void WritePci8(int segment, int handle, int reg, char val);
void WritePci16(int segment, int handle, int reg, short val);
void WritePci32(int segment, int handle, int reg, int val);

int FindClass(int start, unsigned char class_code, unsigned char sub_class);
int FindClassProtocol(int start, unsigned char class_code, unsigned char sub_class, unsigned char protocol);
int FindDevice(int start, short int vendor, short int device);
int GetHandle(unsigned char segment, unsigned char bus, unsigned device, unsigned function);
int GetParam(int handle);
int GetBus(unsigned char segment, unsigned char bus);
unsigned char GetIrq(int handle, int index);
unsigned char GetMsi(int handle);
unsigned char GetMsiX(int handle);
long long GetBarPhys(int handle, unsigned char bar);
short int GetBarIo(int handle, unsigned char bar);
int SetupIrq(int issuer, int handle, int core, int prio);
int ReqMsi(int issuer, int handle, int core, int prio, int vectors);
int SetupMsi(int issuer, int handle, int entry, int core, int prio);
void EnableMsi(int issuer, int handle, int entry);
short int GetCap(int handle, unsigned char cap);
int GetPciName(int handle, char *buf, int maxsize);

int LockPci(int issuer, int handle, const char *name);
int UnlockPci(int issuer, int handle);
int IsPciLocked(int handle);

int EvalIntArr(int handle, char *name, int *arr, int entries);

char ReadPciConfigByte(int issuer, int handle, int reg);
short int ReadPciConfigWord(int issuer, int handle, int reg);
int ReadPciConfigDword(int issuer, int handle, int reg);
void WritePciConfigByte(int issuer, int handle, int reg, char val);
void WritePciConfigWord(int issuer, int handle, int reg, short int val);
void WritePciConfigDword(int issuer, int handle, int reg, int val);

extern int WaitForMsg();
#pragma aux WaitForMsg value [eax]

};

static TAcpiObject *ObjArr[256] = {0};
static TPciSegment *PciSegArr[256] = {0};
static struct TTaskQueueEntry *TaskQueueArr;

static TSection TaskSection("Task.Sect");
static int ThreadSize = 0;
static int ThreadCount = 0;
static TThreadState **ThreadArr = 0;

/*##########################################################################
#
#   Name       : FindThread
#
#   Purpose....: Find thread
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static TThreadState *FindThread(short int id)
{
    int i;
    TThreadState *state;    

    for (i = 0; i < ThreadSize; i++)
    {
        state = ThreadArr[i];
        if (state && state->GetId() == id)
            return state;
    }
    return 0;
}

/*##########################################################################
#
#   Name       : GrowThreadArr
#
#   Purpose....: Grow thread array
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void GrowThreadArr()
{
    int i;
    int Size = 2 * (ThreadSize + 1);
    TThreadState **NewArr;

    NewArr = new TThreadState*[Size];

    for (i = 0; i < ThreadSize; i++)
        NewArr[i] = ThreadArr[i];

    for (i = ThreadSize; i < Size; i++)
        NewArr[i] = 0;

    if (ThreadArr)
        delete ThreadArr;

    ThreadArr = NewArr;
    ThreadSize = Size;
}

/*##########################################################################
#
#   Name       : AddThread
#
#   Purpose....: Add new thread
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void AddThread(int handle)
{
    int index = handle >> 16;

    while (index >= ThreadSize)
        GrowThreadArr();

    if (ThreadArr[index])
        printf("Already has entry: %d\r\n", index);

    ThreadArr[index] = new TThreadState(handle);
    ThreadCount++;
}

/*##########################################################################
#
#   Name       : RemoveThread
#
#   Purpose....: Remove thread
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void RemoveThread(int handle)
{
    int index = handle >> 16;
    TThreadState *state = ThreadArr[index];

    ThreadArr[index] = 0;

    if (state->GetPos() != index)
        printf("Bad delete index: %d\r\n", index);

    delete state;
    ThreadCount--;
}

/*##########################################################################
#
#   Name       : AddProcess
#
#   Purpose....: Add new process
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void AddProcess(int handle)
{
    int index = handle >> 16;
    int id = handle & 0x7FFF;

    printf("Add Process %d.%d\r\n", id, index);
}

/*##########################################################################
#
#   Name       : RemoveProcess
#
#   Purpose....: Remove process
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void RemoveProcess(int handle)
{
    int index = handle >> 16;
    int id = handle & 0x7FFF;

    printf("Remove Process %d.%d\r\n", id, index);
}

/*##########################################################################
#
#   Name       : AddProgram
#
#   Purpose....: Add new program
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void AddProgram(int handle)
{
    int index = handle >> 16;
    int id = handle & 0x7FFF;

    printf("Add Program %d.%d\r\n", id, index);
}

/*##########################################################################
#
#   Name       : RemoveProgram
#
#   Purpose....: Remove program
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void RemoveProgram(int handle)
{
    int index = handle >> 16;
    int id = handle & 0x7FFF;

    printf("Remove Program %d.%d\r\n", id, index);
}

/*##########################################################################
#
#   Name       : LoadModule
#
#   Purpose....: Load new module
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void LoadModule(int handle)
{
    int index = handle >> 16;
    int id = handle & 0x7FFF;

    printf("Load Module %d.%d\r\n", id, index);
}

/*##########################################################################
#
#   Name       : UnloadModule
#
#   Purpose....: Unload module
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void UnloadModule(int handle)
{
    int index = handle >> 16;
    int id = handle & 0x7FFF;

    printf("Unload Module %d.%d\r\n", id, index);
}

/*##########################################################################
#
#   Name       : HandleTaskQueue
#
#   Purpose....: Handle task queue entry
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void HandleTaskQueue(struct TTaskQueueEntry *entry)
{
    switch (entry->Op)
    {
        case REQ_CREATE_THREAD:
            AddThread(entry->Handle);
            break;

        case REQ_TERMINATE_THREAD:
            RemoveThread(entry->Handle);
            break;

        case REQ_CREATE_PROCESS:
            AddProcess(entry->Handle);
            break;

        case REQ_TERMINATE_PROCESS:
            RemoveProcess(entry->Handle);
            break;

        case REQ_CREATE_PROGRAM:
            AddProgram(entry->Handle);
            break;

        case REQ_TERMINATE_PROGRAM:
            RemoveProgram(entry->Handle);
            break;

        case REQ_LOAD_MODULE:
            LoadModule(entry->Handle);
            break;

        case REQ_UNLOAD_MODULE:
            UnloadModule(entry->Handle);
            break;
    }
}

/*##########################################################################
#
#   Name       : TaskHandler
#
#   Purpose....: Task handler
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void TaskHandler(void *ptr)
{
    int index;
    struct TTaskQueueEntry *entry;

    TaskQueueArr = (struct TTaskQueueEntry *)ServUacpiGetTaskQueue();

    index = 0;

    for (;;)
    {
        if (TaskQueueArr[index].Op)
        {
            entry = &TaskQueueArr[index];
            HandleTaskQueue(entry);
            entry->Op = 0;
            index = (index + 1) % 1024;
        }
        else
            ServUacpiWaitTaskQueue(index);
    }
}

/*##########################################################################
#
#   Name       : Schedule
#
#   Purpose....: Scheduler
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void TaskScheduler(void *ptr)
{
    int i;
    TThreadState *state;

    for (;;)
    {
        RdosWaitMilli(250);

        TaskSection.Enter();

        for (i = 0; i < ThreadSize; i++)
        {
            state = ThreadArr[i];
            if (state)
            {
                if (state->Update())
                {
                    if (state->HasNewCore())
                        printf("Thread %d move to core %d\r\n", state->GetId(), state->GetCore());

                    if (state->HasNewIrq())
                        printf("Thread %d assigned to new IRQ %d\r\n", state->GetId(), state->GetIrq());
                }
            }
        }

        TaskSection.Leave();
    }
}

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
        bridge = seg->Add(bus);
        bridge->ScanForDevices();
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
    char *st = new char[0x1000];

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
int FindClass(int start, unsigned char class_code, unsigned char sub_class)
{
    return TPciFunction::FindClass(start, class_code, sub_class);
}

/*##########################################################################
#
#   Name       : FindClassProtocol
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int FindClassProtocol(int start, unsigned char class_code, unsigned char sub_class, unsigned char protocol)
{
    return TPciFunction::FindClassProtocol(start, class_code, sub_class, protocol);
}

/*##########################################################################
#
#   Name       : FindDevice
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int FindDevice(int start, short int vendor, short int device)
{
    return TPciFunction::FindDevice(start, vendor, device);
}

/*##########################################################################
#
#   Name       : GetHandle
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int GetHandle(unsigned char segment, unsigned char bus, unsigned device, unsigned function)
{
    return TPciFunction::GetHandle(segment, bus, device, function);
}

/*##########################################################################
#
#   Name       : GetParam
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int GetParam(int handle)
{
    return TPciFunction::GetParam(handle);
}

/*##########################################################################
#
#   Name       : GetBus
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int GetBus(unsigned char segment, unsigned char bus)
{
    TPciSegment *seg = PciSegArr[segment];
    TPciBridge *func = 0;
    int val;

    if (seg)
        func = seg->Get(bus);

    if (func)
    {
        val = func->GetSegment();
        val = val << 8;

        val |= func->GetBus();
        val = val << 8;

        val |= func->GetPciDevice();
        val = val << 8;

        val |= func->GetPciFunction();

        return val;
    }
    else
        return -1;
}

/*##########################################################################
#
#   Name       : GetIrq
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned char GetIrq(int handle, int index)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->GetIrq(index);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : GetMsi
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned char GetMsi(int handle)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->GetMsi();
    else
        return 0;
}

/*##########################################################################
#
#   Name       : GetMsiX
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned char GetMsiX(int handle)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->GetMsiX();
    else
        return 0;
}

/*##########################################################################
#
#   Name       : GetBarPhys
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
long long GetBarPhys(int handle, unsigned char bar)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->GetBarPhys(bar);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : GetBarIo
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short int GetBarIo(int handle, unsigned char bar)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->GetBarIo(bar);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : SetupIrq
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int SetupIrq(int issuer, int handle, int core, int prio)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        if (!func->IsAllowed(issuer))
            func = 0;

    if (func)
        return func->SetupIrq(core, prio);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : ReqMsi
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int ReqMsi(int issuer, int handle, int core, int prio, int vectors)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        if (!func->IsAllowed(issuer))
            func = 0;

    if (func)
        return func->ReqMsi(core, prio, vectors);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : SetupMsi
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int SetupMsi(int issuer, int handle, int entry, int core, int prio)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        if (!func->IsAllowed(issuer))
            func = 0;

    if (func)
        return func->SetupMsi(entry, core, prio);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : EnableMsi
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void EnableMsi(int issuer, int handle, int entry)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        if (!func->IsAllowed(issuer))
            func = 0;

    if (func)
        func->EnableMsi(entry);
}

/*##########################################################################
#
#   Name       : GetCap
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short int GetCap(int handle, unsigned char cap)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->GetCap(cap);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : GetPciName
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int GetPciName(int handle, char *buf, int maxsize)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->GetPciName(buf, maxsize);
    else
        return 0;
}

/*##########################################################################
#
#   Name       : ReadPciConfigByte
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char ReadPciConfigByte(int issuer, int handle, int reg)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->ReadConfigByte(reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : ReadPciConfigWord
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short int ReadPciConfigWord(int issuer, int handle, int reg)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->ReadConfigWord(reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : ReadPciConfigDword
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int ReadPciConfigDword(int issuer, int handle, int reg)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->ReadConfigDword(reg);
    else
        return -1;
}

/*##########################################################################
#
#   Name       : WritePciConfigByte
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void WritePciConfigByte(int issuer, int handle, int reg, char val)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        if (!func->IsAllowed(issuer))
            func = 0;

    if (func)
        func->WriteConfigByte(reg, val);
}

/*##########################################################################
#
#   Name       : WritePciConfigWord
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void WritePciConfigWord(int issuer, int handle, int reg, short int val)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        if (!func->IsAllowed(issuer))
            func = 0;

    if (func)
        func->WriteConfigWord(reg, val);
}

/*##########################################################################
#
#   Name       : WritePciConfigDword
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void WritePciConfigDword(int issuer, int handle, int reg, int val)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        if (!func->IsAllowed(issuer))
            func = 0;

    if (func)
        func->WriteConfigDword(reg, val);
}

/*##########################################################################
#
#   Name       : LockPci
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int LockPci(int issuer, int handle, const char *name)
{
    return TPciFunction::LockPci(issuer, handle, name);
}

/*##########################################################################
#
#   Name       : UnlockPci
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int UnlockPci(int issuer, int handle)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        if (!func->IsAllowed(issuer))
            func = 0;

    if (func)
    {
        func->UnlockPci();
        return true;
    }
    else
        return false;
}

/*##########################################################################
#
#   Name       : IsPciLocked
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int IsPciLocked(int handle)
{
    TPciFunction *func = TPciFunction::GetFunction(handle);

    if (func)
        return func->IsPciLocked();
    else
        return false;
}

/*##########################################################################
#
#   Name       : EvalIntArr
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int EvalIntArr(int handle, char *name, int *darr, int maxentries)
{
    int *arr = new int[maxentries];
    int count;
    int i;

    count = TPciFunction::EvalIntArr(handle, name, arr, maxentries);

    for (i = 0; i < count; i++)
        darr[i] = arr[i];

    delete arr;

    return count;
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

    RdosCreatePrioThread(TaskHandler, 10, "Task Handler", 0, 0x2000);
    RdosCreateThread(TaskScheduler, "Schedule", 0, 0x2000);

    for (;;)
        WaitForMsg();
}
