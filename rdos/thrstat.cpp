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
# thrstat.cpp
# Thread state class
#
########################################################################*/

#include <stdio.h>
#include "rdos.h"
#include "acpi.h"
#include "thrstat.h"

/*##########################################################################
#
#   Name       : TThreadState::TThreadState
#
#   Purpose....: Constructor for Thread state
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TThreadState::TThreadState(int pos, short int id)
{
    FPos = pos;
    FId = id;

    Init();
}

/*##########################################################################
#
#   Name       : TThreadState::~TThreadState
#
#   Purpose....: Destructor for Thread state
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TThreadState::~TThreadState()
{
}

/*##########################################################################
#
#   Name       : TThreadState::Init
#
#   Purpose....: Init
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TThreadState::Init()
{
    struct TCurrThreadState state;

    if (ServUacpiGetThreadState(FId, &state))
    {
        printf("Thread: %d, Core: %d, Prio: %d\r\n", FId, state.Core, state.Prio);
    }
}

/*##########################################################################
#
#   Name       : ThreadState::GetPos
#
#   Purpose....: Get position
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int TThreadState::GetPos()
{
    return FPos;
}

/*##########################################################################
#
#   Name       : ThreadState::GetId
#
#   Purpose....: Get ID
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short int TThreadState::GetId()
{
    return FId;
}
