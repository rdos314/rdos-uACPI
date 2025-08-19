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

    while (index >= FThreadSize)
        GrowThreadArr();

    if (FThreadArr[index])
        printf("Already has entry: %d\r\n", index);

    FThreadArr[index] = new TThreadState(handle, this);
    FThreadCount++;

    return FThreadArr[index];
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

    FThreadArr[index] = 0;

    if (state->GetPos() != index)
        printf("Bad delete index: %d\r\n", index);

    delete state;
    FThreadCount--;
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
        printf("Added server %d <%s> for IRQ %02hX\r\n", thread->GetId(), thread->GetName(), irq);
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
    int i;
    TThreadState *state;

    for (;;)
    {
        RdosWaitMilli(250);

//        TaskSection.Enter();

        for (i = 0; i < FThreadSize; i++)
        {
            state = FThreadArr[i];
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

//        TaskSection.Leave();
    }
}
