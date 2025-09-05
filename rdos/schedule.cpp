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
# schedule.cpp
# Scheduler class
#
########################################################################*/

#include <stdio.h>
#include "rdos.h"
#include "acpi.h"
#include "schedule.h"
#include "cpu.h"
#include "pcifunc.h"

/*##########################################################################
#
#   Name       : ThreadStartup
#
#   Purpose....: Startup procedure for thread
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void ThreadStartup(void *ptr)
{
    ((TScheduler *)ptr)->Execute();
}

/*##########################################################################
#
#   Name       : TScheduler::TScheduler
#
#   Purpose....: Constructor for scheduler
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TScheduler::TScheduler()
{
    int i;

    FCoreCount = 0;
    FCoreSize = ServUacpiGetCoreCount();
    FCoreArr = new TCore*[FCoreCount];

    for (i = 0; i < FCoreSize; i++)
        FCoreArr[i] = 0;

    for (i = 0; i < 256; i++)
        FIrqArr[i] = 0;

    FThreadCount = 0;
    FThreadSize = 0;
    FThreadArr = 0;
}

/*##########################################################################
#
#   Name       : TScheduler::~TScheduler
#
#   Purpose....: Destructor for scheduler
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TScheduler::~TScheduler()
{
    int i;

    for (i = 0; i < 256; i++)
        if (FIrqArr[i])
            delete FIrqArr[i];
}

/*##########################################################################
#
#   Name       : TScheduler::Start
#
#   Purpose....: Start scheduler
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::Start()
{
    RdosCreatePrioThread(ThreadStartup, 10, "Scheduler", this, 0x4000);
}

/*##########################################################################
#
#   Name       : TScheduler::AddCore
#
#   Purpose....: Add core
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::AddCore(TAcpiProcessor *proc)
{
    if (FCoreSize > FCoreCount)
    {
        printf("Added core %d\r\n", FCoreCount);
        FCoreArr[FCoreCount] = new TCore(proc);
        FCoreCount++;
    }
}

/*##########################################################################
#
#   Name       : TScheduler::GetCoreCount
#
#   Purpose....: Get core count
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TScheduler::GetCoreCount()
{
    return FCoreCount;
}

/*##########################################################################
#
#   Name       : TScheduler::GrowThreadArr
#
#   Purpose....: Grow thread array
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::GrowThreadArr()
{
    int i;
    int Size = 2 * (FThreadSize + 1);
    TThreadState **NewArr;

    NewArr = new TThreadState*[Size];

    for (i = 0; i < FThreadSize; i++)
        NewArr[i] = FThreadArr[i];

    for (i = FThreadSize; i < Size; i++)
        NewArr[i] = 0;

    if (FThreadArr)
        delete FThreadArr;

    FThreadArr = NewArr;
    FThreadSize = Size;
}

/*##########################################################################
#
#   Name       : TScheduler::FindThread
#
#   Purpose....: Find thread
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TThreadState *TScheduler::FindThread(short int id)
{
    int i;
    TThreadState *state;    

    for (i = 0; i < FThreadSize; i++)
    {
        state = FThreadArr[i];
        if (state && state->GetId() == id)
            return state;
    }
    return 0;
}

/*##########################################################################
#
#   Name       : TScheduler::AddThread
#
#   Purpose....: Add new thread
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TThreadState *TScheduler::AddThread(int handle)
{
    int index = handle >> 16;
    TThreadState *state;
    short int coreid;
    TCore *core;

    while (index >= FThreadSize)
        GrowThreadArr();

    if (FThreadArr[index])
        printf("Already has entry: %d\r\n", index);

    state = new TThreadState(handle, this);

    FThreadArr[index] = state;
    FThreadCount++;

    coreid = state->GetCore();

    if (coreid >= 0 && coreid < FCoreCount)
    {
        core = FCoreArr[coreid];
        if (core)
            core->AddThread(state);
        else
            printf("core not found\r\n");
    }

    return state;
}

/*##########################################################################
#
#   Name       : TScheduler::RemoveThread
#
#   Purpose....: Remove thread
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::RemoveThread(int handle)
{
    int index = handle >> 16;
    TThreadState *state = FThreadArr[index];
    short int coreid;
    TCore *core;

    FThreadArr[index] = 0;

    if (state->GetPos() != index)
        printf("Bad delete index: %d\r\n", index);

    coreid = state->GetCore();

    if (coreid >= 0 && coreid < FCoreCount)
    {
        core = FCoreArr[coreid];
        if (core)
            core->RemoveThread(state);
    }

    delete state;
    FThreadCount--;
}

/*##########################################################################
#
#   Name       : TScheduler::Moved
#
#   Purpose....: Thread is moved
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::Moved(TThreadState *thread, short int to)
{
    short int from = thread->GetCore();
    TCore *core;

    core = FCoreArr[from];
    if (core)
        core->RemoveThread(thread);

    core = FCoreArr[to];
    if (core)
        core->AddThread(thread);

    printf("Thread %s moved from core %d to core %d\r\n", thread->GetName(), from, to);

}

/*##########################################################################
#
#   Name       : TScheduler::AddIrq
#
#   Purpose....: Add IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::AddIrq(TPciFunction *pci, int irq)
{
    if (!FIrqArr[irq])
        FIrqArr[irq] = new TIrq();

    FIrqArr[irq]->SetPciFunction(pci);
}

/*##########################################################################
#
#   Name       : TScheduler::AddServer
#
#   Purpose....: Add server for IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::AddServer(int irq, TThreadState *thread)
{
    bool changed;

    if (!FIrqArr[irq])
        FIrqArr[irq] = new TIrq();

    changed = FIrqArr[irq]->AddServer(thread);

    if (changed)
    {
        thread->SetIrq((unsigned char)irq);
        printf("Added server %d <%s> for IRQ %02hX\r\n", thread->GetId(), thread->GetName(), irq);
    }
}

/*##########################################################################
#
#   Name       : TScheduler::DeleteServer
#
#   Purpose....: Delete server for IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::DeleteServer(TThreadState *thread)
{
    int i;
    bool changed = false;

    for (i = 0; i < 256; i++)
        if (FIrqArr[i])
            changed |= FIrqArr[i]->DeleteServer(thread);
}

/*##########################################################################
#
#   Name       : TScheduler::StartCore
#
#   Purpose....: Start a new core
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::StartCore()
{
    int i;
    TCore *core;

    for (i = 0; i < FCoreCount; i++)
    {
        core = FCoreArr[i];
        if (core)
        {
            if (!core->IsStarted()) 
            {
                printf("Core %d, started\r\n", i);
                ServUacpiStartCore(i);
                break;
            }
        }
    }
}

/*##########################################################################
#
#   Name       : TScheduler::MoveIrq
#
#   Purpose....: Move IRQ to new core
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::MoveIrq(int nr, int core)
{
    TIrq *irq = FIrqArr[nr];
    TPciFunction *func = irq->GetPciFunction();

    if (func)
        func->MoveIrq((unsigned char)nr, core);
    else
        printf("Move IRQ %02hX to core %d\r\n", nr, core);

}

/*##########################################################################
#
#   Name       : TScheduler::MoveToCore
#
#   Purpose....: Move thread to new core
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::MoveToCore(TThreadState *state, int core)
{
    TIrq *irq;
    TThreadState *thread;
    int i;

    for (i = 0; i < 256; i++)
    {
        irq = FIrqArr[i];

        if (irq)
        {
            thread = irq->GetServer();
            if (thread == state)
                MoveIrq(i, core);
        }
    }

    state->MoveToCore(core);
}

/*##########################################################################
#
#   Name       : TScheduler::UpdateThreads
#
#   Purpose....: Update threads
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::UpdateThreads()
{
    int i;
    TThreadState *state;

    for (i = 0; i < FThreadSize; i++)
    {
        state = FThreadArr[i];
        if (state)
            state->Update();
    }
}

/*##########################################################################
#
#   Name       : TScheduler::UpdateCores
#
#   Purpose....: Update cores
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::UpdateCores()
{
    int i;
    TCore *core;
    TThreadState *state;
    double load;
    double load_1 = 0.0;
    double min_load = 100.0;
    double max_load = 0.0;
    double load_sum = 0.0;
    double opt_load;
    int max_core;
    int min_core;
    int threads;

    char str[100];

    FActiveCores = 0;

    for (i = 0; i < FCoreCount; i++)
    {
        core = FCoreArr[i];
        if (core)
        {
            if (core->Update())
            {
                FActiveCores++;
                load = core->GetThreadLoad();
                threads = core->GetThreadCount();
                load_sum += load;

                if (load > max_load)
                {
                    max_load = load;
                    max_core = i;
                }

                if (i == 1)
                    load_1 = load;
                else
                {
                    if (load < min_load)
                    {
                        min_load = load;
                        min_core = i;
                    }

                }
//                printf("Core %d, Load: %3.1Lf%%, Threads: %d\r\n", i, load, threads);
            }
        }
    }

    if (FActiveCores)
    {
        load = load_sum / (double)FActiveCores;

        if (load > 30.0)
            StartCore();
        else
        {
            opt_load = (max_load - min_load) / 2.0;
            if (opt_load > 1.0)
            {
                core = FCoreArr[max_core];
                state = core->GetOptThread(opt_load);
                if (state)
                    if (max_core == 1)
                        if (state->GetPrio() > 2)
                            state = 0;

                if (state)
                {
                    if (state->GetPrio() > 2)
                        MoveToCore(state, min_core);
                    else
                    {
                        if (load_1 < min_load)
                            MoveToCore(state, 1);
                        else
                            MoveToCore(state, min_core);
                    }
                }
            }
        }

//        printf("Active: %d, Load: %3.1Lf%%, Min: %d: %3.1Lf%%, Max: %d: %3.1Lf%%\r\n", FActiveCores, load, min_core, min_load, max_core, max_load);
    }
}

/*##########################################################################
#
#   Name       : TScheduler::MoveToScheduleCore
#
#   Purpose....: Move to schedule core
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::MoveToScheduleCore()
{
    int MyId;

    if (GetCoreCount() >= 2)
    {
        MyId = ServUacpiGetThread();
        ServUacpiSetThreadCore(MyId, 1);
    }
}

/*##########################################################################
#
#   Name       : TScheduler::Execute
#
#   Purpose....: Execute
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TScheduler::Execute()
{
    TCore *core;
    unsigned long msb;
    unsigned long lsb;
    unsigned long prev;

    core = FCoreArr[1];
    if (core)
    {
        printf("Core 1, started\r\n");
        ServUacpiStartCore(1);
    }

    MoveToScheduleCore();

    RdosGetSysTime(&msb, &lsb);

    for (;;)
    {
        prev = lsb;
        lsb += 1193046 / LOADS_PER_SEC;        

        if (lsb < prev)
            msb++;
 
        RdosWaitUntil(msb, lsb);

//        TaskSection.Enter();

        UpdateThreads();
        UpdateCores();


//        TaskSection.Leave();
    }
}
