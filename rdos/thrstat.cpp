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
# thrstat.cpp
# Thread state class
#
########################################################################*/

#include <stdio.h>
#include "rdos.h"
#include "acpi.h"
#include "thrstat.h"
#include "schedule.h"

/*##########################################################################
#
#   Name       : TThreadState::TThreadState
#
#   Purpose....: Constructor for Thread state
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TThreadState::TThreadState(int handle, TScheduler *scheduler)
{
    FHandle = handle;
    FScheduler = scheduler;

    Init();
}

/*##########################################################################
#
#   Name       : TThreadState::~TThreadState
#
#   Purpose....: Destructor for Thread state
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TThreadState::~TThreadState()
{
    FScheduler->DeleteServer(this);

//    printf("Deleted %d.%d <%s>\r\n", FHandle & 0x7FFF, FHandle >> 16, FName);
}

/*##########################################################################
#
#   Name       : TThreadState::Init
#
#   Purpose....: Init
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TThreadState::Init()
{
    struct TCurrThreadState state;

    ServUacpiGetThreadName(FHandle, FName);

    FProcId = ServUacpiGetThreadProcess(FHandle);

    FHasIrq = false;
    FNewIrq = false;
    FUsedTics = 0;
    FLoadCount = 0;
    FLoadStart = 0;

//    printf("Added %d.%d PID: %d <%s>\r\n", FHandle & 0x7FFF, FHandle >> 16, FProcId, FName);

    if (ServUacpiGetThreadState(FHandle, &state))
    {
        FCore = state.Core;
        FPrio = state.Prio;
        FIrq = state.Irq;
        FTics = state.Tics;
    }
    else
    {
        FCore = 0;
        FPrio = -1;
        FIrq = 0;
        FTics = 0;
    }
}

/*##########################################################################
#
#   Name       : ThreadState::GetPos
#
#   Purpose....: Get position
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TThreadState::GetPos()
{
    return FHandle >> 16;
}

/*##########################################################################
#
#   Name       : ThreadState::GetId
#
#   Purpose....: Get Id
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short int TThreadState::GetId()
{
    return FHandle & 0x7FFF;
}

/*##########################################################################
#
#   Name       : ThreadState::GetName
#
#   Purpose....: Get name
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
const char *TThreadState::GetName()
{
    return FName;
}

/*##########################################################################
#
#   Name       : ThreadState::MoveToCore
#
#   Purpose....: Move to core
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TThreadState::MoveToCore(int core)
{
    ServUacpiSetThreadCore(FHandle, core);
}

/*##########################################################################
#
#   Name       : ThreadState::Update
#
#   Purpose....: Update
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TThreadState::Update()
{
    bool changed = false;
    struct TCurrThreadState state;
    int IrqArr[8];
    int val;
    int i;
    int j;
    int mask;
    int irq;

    if (ServUacpiGetThreadState(FHandle, &state))
    {
        if (FCore != state.Core)
        {
            FScheduler->Moved(this, state.Core);
            FCore = state.Core;
        }

        FPrio = state.Prio;

        if (FIrq != state.Irq)
        {
            FIrq = state.Irq;
            FNewIrq = true;
            changed = true;
        }
        else
            FNewIrq = false;

        FUsedTics = (int)(state.Tics - FTics);       
        FTics = state.Tics;

        if (FLoadCount == MAX_LOAD_COUNT)
        {
            FLoadArr[FLoadStart] = FUsedTics;
            FLoadStart = (FLoadStart + 1) % MAX_LOAD_COUNT;
        }
        else
        {
            FLoadArr[FLoadCount] = FUsedTics;
            FLoadCount++;
        }

        if (GetPrio() > 2 && (state.Flags & STATE_FLAG_IRQ))
        {
            ServUacpiGetThreadIrqArr(FHandle, IrqArr);
            for (i = 0; i < 8; i++)
            {
                val = IrqArr[i];
                if (val)
                {
                    mask = 1;
                    for (j = 0; j < 32; j++)
                    {
                        if (val & mask)
                        {
                            irq = 32 * i + j;
                            FScheduler->AddServer(irq, this);
                        }
                        mask = mask << 1;
                    }
                }
            }
        }
    }

    return changed;
}

/*##########################################################################
#
#   Name       : ThreadState::HasNewIrq
#
#   Purpose....: Check for new IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TThreadState::HasNewIrq()
{
    return FNewIrq;
}

/*##########################################################################
#
#   Name       : ThreadState::GetCore
#
#   Purpose....: Get core
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short int TThreadState::GetCore()
{
    return FCore;
}

/*##########################################################################
#
#   Name       : ThreadState::GetPrio
#
#   Purpose....: Get prio
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short int TThreadState::GetPrio()
{
    return FPrio;
}

/*##########################################################################
#
#   Name       : ThreadState::GetIrq
#
#   Purpose....: Get IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
unsigned char TThreadState::GetIrq()
{
    return FIrq;
}

/*##########################################################################
#
#   Name       : ThreadState::GetUsedTics
#
#   Purpose....: Get tics since last call to update
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TThreadState::GetUsedTics()
{
    return FUsedTics;
}

/*##########################################################################
#
#   Name       : ThreadState::GetLoad
#
#   Purpose....: Get current load
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
double TThreadState::GetLoad()
{
    return (double)FUsedTics / 1193046.0 * (double)LOADS_PER_SEC * 100.0;
}

/*##########################################################################
#
#   Name       : ThreadState::GetSecLoad
#
#   Purpose....: Get load last second
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
double TThreadState::GetSecLoad()
{
    int sum = 0;
    int count = FLoadCount;
    int ind;
    int i;

    if (count == MAX_LOAD_COUNT)
    {
        ind = FLoadStart - LOADS_PER_SEC;
        if (ind < 0)
            ind += MAX_LOAD_COUNT;

        for (i = 0; i < LOADS_PER_SEC; i++)
        {
            sum += FLoadArr[ind];
            ind = (ind + 1) % MAX_LOAD_COUNT;
        }
    }
    else
    {
        if (count > LOADS_PER_SEC)
            count = LOADS_PER_SEC;

        for (i = 0; i < count; i++)
            sum += FLoadArr[i];

    }
    return (double)sum / 1193046.0 * (double)LOADS_PER_SEC * 100.0;        
}

/*##########################################################################
#
#   Name       : ThreadState::GetMinLoad
#
#   Purpose....: Get load last minute
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
double TThreadState::GetMinLoad()
{
    int sum = 0;
    int count = FLoadCount;
    int i;

    for (i = 0; i < count; i++)
        sum += FLoadArr[i];

    return (double)sum / 1193046.0 * (double)LOADS_PER_SEC / 60.0 * 100.0;        
}
