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
# thrstat.h
# Thread state
#
########################################################################*/

#ifndef _THREAD_STAT_H
#define _THREAD_STAT_H

#define LOADS_PER_SEC       4
#define MAX_LOAD_COUNT      (LOADS_PER_SEC * 60)

class TScheduler;

class TThreadState
{
public:
    TThreadState(int handle, TScheduler *scheduler);
    virtual ~TThreadState();

    int GetPos();
    short int GetId();
    const char *GetName();

    void MoveToCore(int core);
    void SetIrq(unsigned char irq);

    void Update();
    short int GetCore();
    short int GetPrio();
    unsigned char GetIrq();
    int GetUsedTics();
    double GetLoad();
    double GetSecLoad();
    double GetMinLoad();

protected:
    void Init();

    short int FCore;
    short int FPrio;
    unsigned char FIrq;
    long long FTics;
    int FUsedTics;

    int FLoadCount;
    int FLoadStart;
    int FLoadArr[MAX_LOAD_COUNT];

    bool FHasIrq;

    int FHandle;
    int FProcId;
    TScheduler *FScheduler;

    char FName[40];
};

#endif
