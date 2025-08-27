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
# cpu.cpp
# ACPI processor
#
########################################################################*/

#include "cpu.h"
#include "thrstat.h"

/*##########################################################################
#
#   Name       : TAcpiProcessor::TAcpiProcessor
#
#   Purpose....: Constructor for TAcpiProcessor
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiProcessor::TAcpiProcessor(TAcpiObject *parent)
  : TAcpiObject(parent)
{
}

/*##########################################################################
#
#   Name       : TAcpiProcessor::~TAcpiProcessor
#
#   Purpose....: Destructor for TAcpiProcessor
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TAcpiProcessor::~TAcpiProcessor()
{
}

/*##########################################################################
#
#   Name       : TAcpiProcessor::IsProcessor
#
#   Purpose....: Is processor?
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TAcpiProcessor::IsProcessor()
{
    return true;
}

/*##########################################################################
#
#   Name       : TCore::TCore
#
#   Purpose....: Constructor for TCore
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TCore::TCore()
{
    FProc = 0;
    Init();
}

/*##########################################################################
#
#   Name       : TCore::TCore
#
#   Purpose....: Constructor for TCore
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TCore::TCore(TAcpiProcessor *proc)
{
    FProc = proc;
    Init();
}

/*##########################################################################
#
#   Name       : TCore::~TCore
#
#   Purpose....: Destructor for TCore
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TCore::~TCore()
{
}

/*##########################################################################
#
#   Name       : TCore::Init
#
#   Purpose....: Init 
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TCore::Init()
{
    FStarted = false;

    FNullThread = 0;
    FThreadSize = 0;
    FThreadCount = 0;
    FThreadArr = 0;

    FLoadTics = 0;
    FIdleTics = 0;

    FLoadCount = 0;
    FLoadStart = 0;
}

/*##########################################################################
#
#   Name       : TCore::GrowThreadArr
#
#   Purpose....: Grow thread array
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TCore::GrowThreadArr()
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
#   Name       : TCore::AddThread
#
#   Purpose....: Add new thread
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TCore::AddThread(TThreadState *thread)
{
    int i;
    bool found = false;

    if (thread->GetPrio())
    {
        FStarted = true;

        if (FThreadCount == FThreadSize)
            GrowThreadArr();

        for (i = FThreadCount; i < FThreadSize && !found; i++)
        {
            if (FThreadArr[i] == 0)
            {
                FThreadArr[i] = thread;
                found = true;
            }
        }

        for (i = 0; i < FThreadCount && !found; i++)
        {
            if (FThreadArr[i] == 0)
            {
                FThreadArr[i] = thread;
                found = true;
            }
        }

        if (found)
            FThreadCount++;
    }
    else
        FNullThread = thread;
}

/*##########################################################################
#
#   Name       : TCore::RemoveThread
#
#   Purpose....: Remove thread
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TCore::RemoveThread(TThreadState *thread)
{
    int i;
    bool found = false;

    for (i = 0; i < FThreadSize && !found; i++)
    {
        if (FThreadArr[i] == thread)
        {
            FThreadArr[i] = 0;
            found = true;
        }
    }

    if (found)
        FThreadCount--;
}

/*##########################################################################
#
#   Name       : TCore::IsStarted
#
#   Purpose....: Check if started
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TCore::IsStarted()
{
    return FStarted;
}

/*##########################################################################
#
#   Name       : TCore::Update
#
#   Purpose....: Update core stats
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
bool TCore::Update()
{
    TThreadState *state;
    int load = 0;
    int idle = 0;
    int i;

    if (FNullThread)
        idle = FNullThread->GetUsedTics();
    
    for (i = 0; i < FThreadSize; i++)
    {
        state = FThreadArr[i];
        if (state)
            load += state->GetUsedTics();
    }

    if (FLoadCount == MAX_LOAD_COUNT)
    {
        FIdleArr[FLoadStart] = idle;
        FLoadArr[FLoadStart] = load;
        FLoadStart = (FLoadStart + 1) % MAX_LOAD_COUNT;
    }
    else
    {
        FIdleArr[FLoadCount] = idle;
        FLoadArr[FLoadCount] = load;
        FLoadCount++;
    }

    FLoadTics = load;
    FIdleTics = idle;

    if (load + idle > 1193046 / LOADS_PER_SEC / 2)
    {
        FStarted = true;
        return true;
    }
    else
    {
        FStarted = false;
        return false;
    }
}

/*##########################################################################
#
#   Name       : TCore::GetOptThread
#
#   Purpose....: Get thread with optimal load to move
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TThreadState *TCore::GetOptThread(double opt_load)
{
    int i;
    TThreadState *state;
    bool found = false;
    double load;
    double diff;
    double best_diff = opt_load / 2;
    int best_id;

    for (i = 0; i < FThreadSize; i++)
    {
        state = FThreadArr[i];

        if (state)
        {
            load = state->GetSecLoad();
            if (load > opt_load)
                diff = load - opt_load;
            else
                diff = opt_load - load;

            if (diff < best_diff)
            {
                found = true;
                best_diff = diff;
                best_id = i;
            }
        }
    }

    if (found)
        return FThreadArr[best_id];
    else
        return 0;
}

/*##########################################################################
#
#   Name       : TCore::GetThreadLoad
#
#   Purpose....: Get current thread load
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
double TCore::GetThreadLoad()
{
    return (double)FLoadTics / 1193046.0 * (double)LOADS_PER_SEC * 100.0;
}

/*##########################################################################
#
#   Name       : TCore::GetIdleLoad
#
#   Purpose....: Get current idle load
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
double TCore::GetIdleLoad()
{
    return (double)FIdleTics / 1193046.0 * (double)LOADS_PER_SEC * 100.0;
}

/*##########################################################################
#
#   Name       : TCore::GetIntLoad
#
#   Purpose....: Get current interrupt load
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
double TCore::GetIntLoad()
{
    double load = (double)(1193046 / LOADS_PER_SEC - FLoadTics - FIdleTics) / 1193046.0 * (double)LOADS_PER_SEC * 100.0;

    if (load > 0.0)
        return load;
    else
        return 0.0;
}

/*##########################################################################
#
#   Name       : TCore::GetSecLoad
#
#   Purpose....: Get load last second
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
double TCore::GetSecLoad()
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
#   Name       : TCore::GetMinLoad
#
#   Purpose....: Get load last minute
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
double TCore::GetMinLoad()
{
    int sum = 0;
    int count = FLoadCount;
    int i;

    for (i = 0; i < count; i++)
        sum += FLoadArr[i];

    return (double)sum / 1193046.0 * (double)LOADS_PER_SEC / 60.0 * 100.0;        
}
