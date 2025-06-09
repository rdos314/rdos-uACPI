/*#######################################################################
# RDOS operating system
# Copyright (C) 1988-2012, Leif Ekblad
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
# handler.c
# Server handler
#
########################################################################*/

int FindClass(int start, unsigned char class_code, unsigned char sub_class);
int FindClassProtocol(int start, unsigned char class_code, unsigned char sub_class, unsigned char protocol);
int FindDevice(int start, short int vendor, short int device);
int GetHandle(unsigned char segment, unsigned char bus, unsigned device, unsigned function);
int GetParam(int handle);
int GetBus(unsigned char segment, unsigned char bus);
unsigned char GetIrq(int handle, int index);
unsigned char GetMsi(int handle);
unsigned char GetMsiX(int handle);
int SetupIrq(int issuer, int handle, int core, int prio);
int SetupMsi(int issuer, int handle, int core, int prio, int vectors);
short int GetCap(int handle, unsigned char cap);
int GetPciName(int handle, char *buf, int maxsize);
char ReadPciConfigByte(int issuer, int handle, int reg);
short int ReadPciConfigWord(int issuer, int handle, int reg);
int ReadPciConfigDword(int issuer, int handle, int reg);
void WritePciConfigByte(int issuer, int handle, int reg, char val);
void WritePciConfigWord(int issuer, int handle, int reg, short int val);
void WritePciConfigDword(int issuer, int handle, int reg, int val);
int LockPci(int issuer, int handle, char *name);
int UnlockPci(int issuer, int handle);

/*##########################################################################
#
#   Name       : LowFindClass
#
#   Purpose....: Find class
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowFindClass "*" parm routine [ebx] [eax] value [eax]
int LowFindClass(int start, int class_subclass)
{
    unsigned char subclass = class_subclass & 0xFF;
    unsigned char class = (class_subclass >> 8) & 0xFF;

    return FindClass(start, class, subclass);
}

/*##########################################################################
#
#   Name       : FindClassInterface
#
#   Purpose....: Find class & interface
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowFindClassProtocol "*" parm routine [ebx] [eax] [edx] value [eax]
int LowFindClassProtocol(int start, int class_subclass, int interface)
{
    unsigned char subclass = class_subclass & 0xFF;
    unsigned char class = (class_subclass >> 8) & 0xFF;

    return FindClassProtocol(start, class, subclass, (unsigned char)interface);
}

/*##########################################################################
#
#   Name       : FindDevice
#
#   Purpose....: Find device
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowFindDevice "*" parm routine [ebx] [edx] [ecx] value [eax]
int LowFindDevice(int start, int vendor, int device)
{
    return FindDevice(start, (short int)vendor, (short int)device);
}

/*##########################################################################
#
#   Name       : GetHandle
#
#   Purpose....: Get handle
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowGetHandle "*" parm routine [edx] [ecx] value [eax]
int LowGetHandle(int seg_bus, int dev_func)
{
    unsigned char bus = seg_bus & 0xFF;
    unsigned char seg = (seg_bus >> 8) & 0xFF;
    unsigned char func = dev_func & 0xFF;
    unsigned char dev = (dev_func >> 8) & 0xFF;

    return GetHandle(seg, bus, dev, func);
}

/*##########################################################################
#
#   Name       : GetParam
#
#   Purpose....: Get param
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowGetParam "*" parm routine [ebx] value [eax]
int LowGetParam(int start)
{
    return GetParam(start);
}

/*##########################################################################
#
#   Name       : GetBus
#
#   Purpose....: Get bus
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowGetBus "*" parm routine [edx] value [eax]
int LowGetBus(int seg_bus)
{
    unsigned char bus = seg_bus & 0xFF;
    unsigned char seg = (seg_bus >> 8) & 0xFF;

    return GetBus(seg, bus);
}

/*##########################################################################
#
#   Name       : GetIrq
#
#   Purpose....: Get IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowGetIrq "*" parm routine [ebx] [edi] value [al]
unsigned char LowGetIrq(int handle, int index)
{
    return GetIrq(handle, index);
}

/*##########################################################################
#
#   Name       : GetMsi
#
#   Purpose....: Get MSI
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowGetMsi "*" parm routine [ebx] value [al]
unsigned char LowGetMsi(int handle)
{
    return GetMsi(handle);
}

/*##########################################################################
#
#   Name       : GetMsiX
#
#   Purpose....: Get MSI-X
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowGetMsiX "*" parm routine [ebx] value [al]
unsigned char LowGetMsiX(int handle)
{
    return GetMsiX(handle);
}

/*##########################################################################
#
#   Name       : SetupIrq
#
#   Purpose....: Setup IRQ
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowSetupIrq "*" parm routine [edx] [ebx] [esi] [edi] value [eax]
int LowSetupIrq(int issuer, int handle, int core, int prio)
{
    return SetupIrq(issuer, handle, core, prio);
}

/*##########################################################################
#
#   Name       : SetupMsi
#
#   Purpose....: Setup MSI
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowSetupMsi "*" parm routine [edx] [ebx] [esi] [edi] [ecx] value [eax]
int LowSetupMsi(int issuer, int handle, int core, int prio, int vectors)
{
    return SetupMsi(issuer, handle, core, prio, vectors);
}

/*##########################################################################
#
#   Name       : GetCap
#
#   Purpose....: Get capability
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowGetCap "*" parm routine [ebx] [edi] value [ax]
short int LowGetCap(int handle, int cap)
{
    return GetCap(handle, (unsigned char)cap);
}

/*##########################################################################
#
#   Name       : GetPciName
#
#   Purpose....: Get PCI funtion ACPI name
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowGetPciName "*" parm routine [ebx] [edi] [ecx] value [eax]
int LowGetPciName(int handle, char *buf, int maxsize)
{
    return GetPciName(handle, buf, maxsize);
}

/*##########################################################################
#
#   Name       : ReadPciConfigByte
#
#   Purpose....: Read PCI config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowReadConfigByte "*" parm routine [edx] [ebx] [ecx] value [al]
char LowReadConfigByte(int issuer, int handle, int reg)
{
    return ReadPciConfigByte(issuer, handle, reg);
}

/*##########################################################################
#
#   Name       : ReadPciConfigWord
#
#   Purpose....: Read PCI config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowReadConfigWord "*" parm routine [edx] [ebx] [ecx] value [ax]
short int LowReadConfigWord(int issuer, int handle, int reg)
{
    return ReadPciConfigWord(issuer, handle, reg);
}

/*##########################################################################
#
#   Name       : ReadPciConfigDword
#
#   Purpose....: Read PCI config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowReadConfigDword "*" parm routine [edx] [ebx] [ecx] value [eax]
int LowReadConfigDword(int issuer, int handle, int reg)
{
    return ReadPciConfigDword(issuer, handle, reg);
}

/*##########################################################################
#
#   Name       : WritePciConfigByte
#
#   Purpose....: Write PCI config byte
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowWriteConfigByte "*" parm routine [edx] [ebx] [ecx] [edi]
void LowWriteConfigByte(int issuer, int handle, int reg, int val)
{
    WritePciConfigByte(issuer, handle, reg, (char)val);
}

/*##########################################################################
#
#   Name       : WritePciConfigWord
#
#   Purpose....: Write PCI config word
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowWriteConfigWord "*" parm routine [edx] [ebx] [ecx] [edi]
void LowWriteConfigWord(int issuer, int handle, int reg, int val)
{
    WritePciConfigWord(issuer, handle, reg, (short int)val);
}

/*##########################################################################
#
#   Name       : WritePciConfigDword
#
#   Purpose....: Write PCI config dword
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowWriteConfigDword "*" parm routine [edx] [ebx] [ecx] [edi]
void LowWriteConfigDword(int issuer, int handle, int reg, int val)
{
    WritePciConfigDword(issuer, handle, reg, val);
}

/*##########################################################################
#
#   Name       : LockPci
#
#   Purpose....: Lock PCI
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowLockPci "*" parm routine [edx] [ebx] [edi] value [eax]
int LowLockPci(int issuer, int handle, const char *name)
{
    return LockPci(issuer, handle, name);
}

/*##########################################################################
#
#   Name       : UnlockPci
#
#   Purpose....: Unlock PCI
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
#pragma aux LowUnlockPci "*" parm routine [edx] [ebx] value [eax]
int LowUnlockPci(int issuer, int handle)
{
    return UnlockPci(issuer, handle);
}
