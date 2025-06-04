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
# pciseg.cpp
# PCI segment
#
########################################################################*/

#ifndef _PCI_SEGMENT_H
#define _PCI_SEGMENT_H

#include <uacpi/tables.h>

class TPciBridge;

class TPciSegment
{
public:
    TPciSegment(int segment);
    TPciSegment(struct acpi_mcfg_allocation *mfcg);
    virtual ~TPciSegment();

    int GetSegment();
    void Add(TPciBridge *bridge, int bus);
    TPciBridge *Add(int bus);
    TPciBridge *Get(int bus);

    bool HasDev(TPciBridge *bridge, int device);

    int CalcOffs(int bus, int dev, int func, int reg);
    int GetHandle(int bus, int dev, int func);

    char ReadConfigByte(int offs);
    short ReadConfigWord(int offs);
    int ReadConfigDword(int offs);
    void WriteConfigByte(int offs, char val);
    void WriteConfigWord(int offs, short val);
    void WriteConfigDword(int offs, int val);

    char ReadConfigByte(int handle, int reg);
    short ReadConfigWord(int handle, int reg);
    int ReadConfigDword(int handle, int reg);
    void WriteConfigByte(int handle, int reg, char val);
    void WriteConfigWord(int handle, int reg, short val);
    void WriteConfigDword(int handle, int reg, int val);

protected:
    void Init();

    int FSegment;
    int FStartBus;
    int FBusCount;
    char *FMem;
    int FIo;
    TPciBridge **FBusArr;
};

#endif
