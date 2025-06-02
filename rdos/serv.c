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
