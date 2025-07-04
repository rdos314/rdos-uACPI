/*#######################################################################
# RDOS operating system
# Copyright (C) 1988-2002, Leif Ekblad
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version. The only exception to this rule
# is for commercial usage in embedded systems. For information on
# usage in commercial embedded systems, contact embedded@rdos.net
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
# The author of this program may be contacted at leif@rdos.net
#
# pcifunc.cpp
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
