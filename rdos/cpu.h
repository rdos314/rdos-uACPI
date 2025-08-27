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
# cpu.h
# ACPI processor
#
########################################################################*/

#ifndef _CPU_H
#define _CPU_H

#include "obj.h"
#include "thrstat.h"

class TAcpiProcessor : public TAcpiObject
{
public:
    TAcpiProcessor(TAcpiObject *parent);
    ~TAcpiProcessor();

    virtual bool IsProcessor();

};

class TCore
{
public:
    TCore();
    TCore(TAcpiProcessor *proc);
    ~TCore();

    void AddThread(TThreadState *thread);
    void RemoveThread(TThreadState *thread);

    void Update();

    double GetThreadLoad();
    double GetIdleLoad();
    double GetIntLoad();

    double GetSecLoad();
    double GetMinLoad();

protected:
    void Init();
    void GrowThreadArr();

    TAcpiProcessor *FProc;

    TThreadState *FNullThread;

    int FThreadSize;
    int FThreadCount;
    TThreadState **FThreadArr;

    int FLoadTics;
    int FIdleTics;

    int FLoadCount;
    int FLoadStart;
    int FLoadArr[MAX_LOAD_COUNT];
    int FIdleArr[MAX_LOAD_COUNT];
};

#endif
