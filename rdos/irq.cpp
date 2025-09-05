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
# irq.cpp
# IRQ class
#
########################################################################*/

#include <stdio.h>
#include "rdos.h"
#include "acpi.h"
#include "thrstat.h"
#include "irq.h"
#include "pcifunc.h"

/*##########################################################################
#
#   Name       : TIrq::TIrq
#
#   Purpose....: Constructor for IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TIrq::TIrq()
{
    FCore = 0;
    FServerCount = 0;
    FDisabled = false;
    FFunc = 0;
}

/*##########################################################################
#
#   Name       : TIrq::~TIrq
#
#   Purpose....: Destructor for IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TIrq::~TIrq()
{
}

/*##########################################################################
#
#   Name       : TIrq::GetCore
#
#   Purpose....: Get core
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TIrq::GetCore()
{
    return FCore;
}

/*##########################################################################
#
#   Name       : TIrq::GetServers
#
#   Purpose....: Get servers
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TIrq::GetServers()
{
    if (FDisabled)
        return -1;
    else
        return FServerCount;
}

/*##########################################################################
#
#   Name       : TIrq::GetServer
#
#   Purpose....: Get server
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TThreadState *TIrq::GetServer()
{
    if (!FDisabled && FServerCount == 1)
        return FServerArr[0];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TIrq::SetPciFunction
#
#   Purpose....: Set PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TIrq::SetPciFunction(TPciFunction *func)
{
    FFunc = func;
}

/*##########################################################################
#
#   Name       : TIrq::GetPciFunction
#
#   Purpose....: Get PCI function
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TPciFunction *TIrq::GetPciFunction()
{
    return FFunc;
}

/*##########################################################################
#
#   Name       : TIrq::AddServer
#
#   Purpose....: Add server
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TIrq::AddServer(TThreadState *thread)
{
    int i;
    bool found = false;

    if (!FDisabled)
    {
        for (i = 0; i < FServerCount && !found; i++)
            if (FServerArr[i] == thread)
                found = true;

        if (!found && FServerCount < 4)
        {
            if (FServerCount > 1)
                FDisabled = true;
            else
            {
                FServerArr[FServerCount] = thread;
                FServerCount++;
            }
            return true;
        }
    }
    return false;
}

/*##########################################################################
#
#   Name       : TIrq::DeleteServer
#
#   Purpose....: Delete server
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TIrq::DeleteServer(TThreadState *thread)
{
    if (FServerCount == 1 && FServerArr[0] == thread)
    {
        FServerCount = 0;
        return true;
    }
    return false;
}
