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
# fs.cpp
# Fs base class
#
########################################################################*/

#include <stdio.h>
#include <string.h>
#include "acpi.h"
#include "rdos.h"
#include "task.h"

#define REQ_CREATE_THREAD       1
#define REQ_TERMINATE_THREAD    2

/*##########################################################################
#
#   Name       : ThreadStartup
#
#   Purpose....: Startup procedure for thread
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void ThreadStartup(void *ptr)
{
    ((TTaskHandler *)ptr)->Execute();
}

/*##########################################################################
#
#   Name       : TTaskHandler::TTaskHandler
#
#   Purpose....: Task handler contructor
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TTaskHandler::TTaskHandler()
{
}

/*##########################################################################
#
#   Name       : TTaskHandler::~TTaskHandler
#
#   Purpose....: Task handler destructor
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
TTaskHandler::~TTaskHandler()
{
}

/*##########################################################################
#
#   Name       : TTaskHandler::Start
#
#   Purpose....: Start server
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TTaskHandler::Start()
{
    RdosCreateThread(ThreadStartup, "Task Handler", this, 0x2000);
}

/*##########################################################################
#
#   Name       : TTaskHandler::HandleQueue
#
#   Purpose....: Handle queue entry
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TTaskHandler::HandleQueue(struct TTaskQueueEntry *entry)
{
    switch (entry->Op)
    {
        case REQ_CREATE_THREAD:
            break;

        case REQ_TERMINATE_THREAD:
            break;

    }
}

/*##########################################################################
#
#   Name       : TTaskHandler::Execute
#
#   Purpose....: Execute
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void TTaskHandler::Execute()
{
    int index;
    struct TTaskQueueEntry *entry;

    FQueueArr = (struct TTaskQueueEntry *)ServUacpiGetTaskQueue();

    index = 0;

    for (;;)
    {
        if (FQueueArr[index].Op)
        {
            entry = &FQueueArr[index];
            HandleQueue(entry);
            entry->Op = 0;
            index = (index + 1) % 256;
        }
        else
            ServUacpiWaitTaskQueue(index);
    }
}
