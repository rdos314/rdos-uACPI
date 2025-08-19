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
# schedule.h
# Scheduler class
#
########################################################################*/

#ifndef _SCHED_H
#define _SCHED_H

#include "thrstat.h"
#include "irq.h"

class TScheduler
{
friend class TThreadState;
friend class TIrq;
public:
    TScheduler();
    virtual ~TScheduler();

    TThreadState *FindThread(short int id);
    TThreadState *AddThread(int handle);
    void RemoveThread(int handle);

    void Start();
    void Execute();

protected:
    void GrowThreadArr();

    void AddServer(int irq, TThreadState *thread);
    void DeleteServer(TThreadState *thread);

    TIrq *FIrqArr[256];

    int FThreadSize;
    int FThreadCount;
    TThreadState **FThreadArr;
};

#endif
