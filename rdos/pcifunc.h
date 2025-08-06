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
# pcifunc.h
# PCI function
#
########################################################################*/

#ifndef _PCI_FUNC_H
#define _PCI_FUNC_H

#include "dev.h"

#define MAX_PCI_IRQS    32
#define MAX_PCI_BARS     6

class TPciDevice;
class TPciIrqRoute;

class TPciFunction : public TAcpiDevice
{
public:
    TPciFunction();
    TPciFunction(TPciDevice *dev, int function, int vendor_device, unsigned char class_code, unsigned char sub_class);
    virtual ~TPciFunction();

    virtual bool IsPciFunction();
    virtual TAcpiObject *FindPciFunction(int device, int function);

    static int Count();
    static TPciFunction *Get(int index);

    static int FindClass(int index, unsigned char class_code, unsigned char sub_class);
    static int FindClassProtocol(int index, unsigned char class_code, unsigned char sub_class, unsigned char protocol);
    static int FindDevice(int start, unsigned short vendor, unsigned short device);
    static int GetHandle(unsigned char segment, unsigned char bus, unsigned device, unsigned function);
    static int GetParam(int handle);
    static TPciFunction *GetFunction(int handle);
    static bool LockPci(int issuer, int handle, const char *name);

    static int EvalIntArr(int handle, char *name, int *arr, int maxentries);

    bool IsAllowed(int issuer);
    void SetDevice(TPciDevice *dev);
    int GetSegment();
    int GetBus();
    int GetPciDevice();
    int GetPciFunction();
    unsigned short GetVendor();
    unsigned short GetDevice();
    unsigned char GetClass();
    unsigned char GetSubClass();
    unsigned char GetProtocol();
    short int GetCap(unsigned char cap);
    int GetPciName(char *buf, int maxsize);
    unsigned char GetIrq(int index);
    unsigned char GetMsi();
    unsigned char GetMsiX();

    void UnlockPci();
    bool IsPciLocked();

    int SetupIrq(int core, int prio);
    int SetupMsi(int core, int prio, int vectors);
    void EnableMsi();

    char ReadConfigByte(int reg);
    short ReadConfigWord(int reg);
    int ReadConfigDword(int reg);
    void WriteConfigByte(int reg, char val);
    void WriteConfigWord(int reg, short val);
    void WriteConfigDword(int reg, int val);

    void PowerOn();
    long long GetBarPhys(unsigned char bar);
    short int GetBarIo(unsigned char bar);

protected:
    void Init();
    void Setup(int vendor_device, unsigned char class_code, unsigned char sub_class);
    void SetupBars();
    void Add(TPciFunction *func);
    TPciIrqRoute *GetIrq();

    void LockPci(int issuer, const char *name);

    static int FFuncCount;
    static int FFuncSize;
    static TPciFunction **FFuncArr;

    TPciDevice *FPciDevice;
    int FPciFunction;

    int FIssuer;
    char *FOwnerName;

    unsigned short FVendor;
    unsigned short FDevice;
    unsigned char FClass;
    unsigned char FSubClass;
    unsigned char FProtocol;

    unsigned char FMsiBase;
    int FMsiVectors;
    bool FUseMsi;

    unsigned char FMsiXBase;
    int FMsiXVectors;
    int *FMsiXVectorArr;
    bool FUseMsiX;

    int FIrqCount;
    unsigned char FIrqArr[MAX_PCI_IRQS];

    long long FBarPhysArr[MAX_PCI_BARS];
    short int FBarIoArr[MAX_PCI_BARS];
};

#endif
