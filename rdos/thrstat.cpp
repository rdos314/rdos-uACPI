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
TThreadState::TThreadState(int handle)
{
    FHandle = handle;

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
    printf("Deleted %d.%d <%s>\r\n", FHandle & 0x7FFF, FHandle >> 16, FName);
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

    FNewCore = false;
    FNewIrq = false;
    FUsedTics = 0;

    printf("Added %d.%d <%s>\r\n", FHandle & 0x7FFF, FHandle >> 16, FName);

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

    if (ServUacpiGetThreadState(FHandle, &state))
    {
        if (FCore != state.Core)
        {
            FCore = state.Core;
            FNewCore = true;
            changed = true;
        }
        else
            FNewCore = false;

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
    }

    return changed;
}

/*##########################################################################
#
#   Name       : ThreadState::HasNewCore
#
#   Purpose....: Check for new core
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TThreadState::HasNewCore()
{
    return FNewCore;
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

