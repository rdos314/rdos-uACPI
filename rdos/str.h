/*#######################################################################
# RDOS operating system
# Copyright (C) 1988-2003, Leif Ekblad
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
# string.h
# String class
#
########################################################################*/

#ifndef _STRING_H
#define _STRING_H

#include <stdarg.h>
#include "shareobj.h"

class TString : public TShareObject
{
friend class TStringListNode;
public:
    TString();
    TString(const TString &source);
    TString(const char *str);
    TString(const char *str, int size);
    virtual ~TString();

    const TString &operator=(const TString &src);
    const TString &operator=(const char *str);
    const TString &operator+=(const char *str);
    const TString &operator+=(char ch);
    const TString &operator+=(const TString& str);
    int operator==(const TString &dest) const;
    int operator!=(const TString &dest) const;
    int operator>(const TString &dest) const;
    int operator>=(const TString &dest) const;
    int operator<(const TString &dest) const;
    int operator<=(const TString &dest) const;
    char operator[](int n) const;

    int printf(const char *fmt, ...);
    int prtf(const char *frm, va_list args);

    void Reset();
    const char *GetData() const;
    int GetSize() const;

    const char *Find(char ch) const;
    const char *Find(const char *str) const;
    void Upper();
    void Lower();
    void RemoveCrLf();
    void Append(char ch);
    void Append(const char *str);
    void Append(const char *str, int size);
    void Replace(const char *src, const char *dest);

    void ConcatCopy(const char *str1, int len1, const char *str2, int len2);

    virtual int Compare(const TShareObject &str) const;

protected:
    virtual char Upper(char ch);
    virtual char Lower(char ch);

    void Pad(int count, const char *str);

#ifndef __RDOS__
        int Number(long num, int base, int size, int precision, int type);
#endif

    void ReplaceOne(char *ptr, const char *src, const char *dest);
    void AllocCopy(TString& dest, int CopyLen, int CopyIndex, int ExtraLen) const;
    void ConcatInPlace(const char *str, int size);
};

TString operator+(const TString& str1, const TString& str2);
TString operator+(const TString& str, const char *cstr);
TString operator+(const char *cstr, const TString& str);

#endif
