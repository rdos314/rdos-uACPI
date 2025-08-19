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

    for (i = 0; i < 256; i++)
        FIrqArr[i] = 0;
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
