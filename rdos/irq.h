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
# irq.h
# IRQ class
#
########################################################################*/

#ifndef _IRQ_H
#define _IRQ_H

#define MAX_IRQ_SERVERS   4

class TThreadState;
class TPciFunction;

class TIrq
{
public:
    TIrq();
    virtual ~TIrq();

    int GetCore();
    int GetServers();
    TThreadState *GetServer();

    TPciFunction *GetPciFunction();
    void SetPciFunction(TPciFunction *func);

    bool AddServer(TThreadState *thread);
    bool DeleteServer(TThreadState *thread);

protected:
    bool FDisabled;
    TPciFunction *FFunc;
    int FCore;    
    int FServerCount;
    TThreadState *FServerArr[MAX_IRQ_SERVERS];     
};

#endif
