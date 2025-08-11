
#ifndef _RDOS_ACPI_SERV_H
#define _RDOS_ACPI_SERV_H

#pragma pack( __push, 1 )

#define RDOSAPI

#include <stdarg.h>
#include "rdacpi.h"

#pragma pack( __pop )


// API functions

#ifdef __cplusplus
extern "C" {
#endif


struct TCurrThreadState
{
    short int Core;
    short int Prio;
    unsigned char Irq;
    char Pad;
    long long Tics;
};


long long RDOSAPI ServUacpiGetAcpi();
void *RDOSAPI ServUacpiMap(long long phys, int size);
void RDOSAPI ServUacpiUnmap(void *base, int size);
void RDOSAPI ServUacpiEnableIo(int base, int size);
void RDOSAPI ServUacpiDisableIo(int base, int size);
void RDOSAPI ServUacpiStartPci();
int RDOSAPI ServUacpiHasApic();
unsigned char RDOSAPI ServUacpiAllocateInts(int count, unsigned char prio);
void RDOSAPI ServUacpiFreeInt(unsigned char irq);
int RDOSAPI ServUacpiGetMsiAddress(int core);
int RDOSAPI ServUacpiGetMsiData(unsigned char irq);

void *RDOSAPI ServUacpiGetTaskQueue();
void RDOSAPI ServUacpiWaitTaskQueue(int index);
int RDOSAPI ServUacpiGetThreadState(int id, struct TCurrThreadState *state);

#ifdef __cplusplus
}
#endif

#ifdef __WATCOMC__
#include "owacpi.h"
#endif

#endif
