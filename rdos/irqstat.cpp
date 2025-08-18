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
# irqstat.cpp
# IRQ state class
#
########################################################################*/

#include <stdio.h>
#include "rdos.h"
#include "acpi.h"
#include "thrstat.h"
#include "irqstat.h"

/*##########################################################################
#
#   Name       : TIrqState::TIrqState
#
#   Purpose....: Constructor for IRQ state
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TIrqState::TIrqState(int irq)
{
    FCore = 0;
    FIrq = irq;
    FServerCount = 0;
}

/*##########################################################################
#
#   Name       : TIrqState::~TIrqState
#
#   Purpose....: Destructor for IRQ state
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TIrqState::~TIrqState()
{
}

/*##########################################################################
#
#   Name       : TIrqState::GetIrq
#
#   Purpose....: Get IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TIrqState::GetIrq()
{
    return FIrq;
}

/*##########################################################################
#
#   Name       : TIrqState::GetCore
#
#   Purpose....: Get core
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TIrqState::GetCore()
{
    return FCore;
}

/*##########################################################################
#
#   Name       : TIrqState::GetServers
#
#   Purpose....: Get servers
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TIrqState::GetServers()
{
    return FServerCount;
}

/*##########################################################################
#
#   Name       : TIrqState::GetServer
#
#   Purpose....: Get server
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TThreadState *TIrqState::GetServer()
{
    if (FServerCount == 1)
        return FServerArr[0];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TIrqState::AddServer
#
#   Purpose....: Add server
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TIrqState::AddServer(TThreadState *thread)
{
    int i;
    bool found = false;

    for (i = 0; i < FServerCount && !found; i++)
        if (FServerArr[i] == thread)
            found = true;

    if (!found && FServerCount < 4)
    {
        printf("Added server %d <%s> for IRQ %d\r\n", thread->GetId(), thread->GetName(), FIrq);

        FServerArr[FServerCount] = thread;
        FServerCount++;
    }
}

/*##########################################################################
#
#   Name       : TIrqState::DeleteServer
#
#   Purpose....: Delete server
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TIrqState::DeleteServer(TThreadState *thread)
{
    if (FServerCount == 1 && FServerArr[0] == thread)
    {
        printf("Deleted server %d <%s> for IRQ %d\r\n", thread->GetId(), thread->GetName(), FIrq);
        FServerCount = 0;
    }
}
