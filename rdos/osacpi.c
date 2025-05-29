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
# osacpi.cpp
# OS services
#
########################################################################*/

#include <malloc.h>
#include <stdio.h>
#include <uacpi/kernel_api.h>
#include "rdos.h"
#include "acpi.h"

struct io_map
{
    int base;
    int size;
};

struct event_t
{
    int wait;
    int handle;
};

int lock_spinlock(int *spinlock);
#pragma aux lock_spinlock = \
    "mov eax,[esi]" \
    "or eax,eax" \
    "jnz done" \
    "inc eax" \
    "xchg eax,[esi]" \
    "done: " \
    parm [ esi ] \
    value [ eax ]

char in_byte(int port);
#pragma aux in_byte = \
    "in al,dx" \
    parm [ edx ] \
    value [ al ]

short in_word(int port);
#pragma aux in_word = \
    "in ax,dx" \
    parm [ edx ] \
    value [ ax ]

int in_dword(int port);
#pragma aux in_dword = \
    "in eax,dx" \
    parm [ edx ] \
    value [ eax ]

void out_byte(int port, char val);
#pragma aux out_byte = \
    "out dx,al" \
    parm [ edx ] [ al ]

void out_word(int port, short val);
#pragma aux out_word = \
    "out dx,ax" \
    parm [ edx ] [ ax ]

void out_dword(int port, int val);
#pragma aux out_dword = \
    "out dx,eax" \
    parm [ edx ] [ eax ]

static long long TimeBase = 0;

/*##########################################################################
#
#   Name       : read_pci_byte
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static char read_pci_byte(int ads)
{
    char val;

    out_dword(0xCF8, ads & 0xFFFFFFFC);
    val = in_byte(0xCFC + (ads & 3));

    return val;
}

/*##########################################################################
#
#   Name       : read_pci_word
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static short read_pci_word(int ads)
{
    short val;

    out_dword(0xCF8, ads & 0xFFFFFFFC);
    val = in_word(0xCFC + (ads & 2));

    return val;
}

/*##########################################################################
#
#   Name       : read_pci_dword
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static int read_pci_dword(int ads)
{
    int val;

    out_dword(0xCF8, ads & 0xFFFFFFFC);
    val = in_dword(0xCFC);

    return val;
}

/*##########################################################################
#
#   Name       : write_pci_byte
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void write_pci_byte(int ads, char val)
{
    out_dword(0xCF8, ads & 0xFFFFFFFC);
    out_byte(0xCFC + (ads & 3), val);
}

/*##########################################################################
#
#   Name       : write_pci_word
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void write_pci_word(int ads, short val)
{
    out_dword(0xCF8, ads & 0xFFFFFFFC);
    out_word(0xCFC + (ads & 2), val);
}

/*##########################################################################
#
#   Name       : write_pci_dword
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
static void write_pci_dword(int ads, int val)
{
    out_dword(0xCF8, ads & 0xFFFFFFFC);
    out_dword(0xCFC, val);
}

/*##########################################################################
#
#   Name       : get rdsp
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address)
{
    long long phys = ServUacpiGetAcpi();

    if (phys)
    {
        *out_rsdp_address = phys;
        return UACPI_STATUS_OK;
    }
    else
        return UACPI_STATUS_NOT_FOUND;

}

/*##########################################################################
#
#   Name       : map
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len)
{
    return ServUacpiMap(addr, len);
}

/*##########################################################################
#
#   Name       : unmap
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_unmap(void *addr, uacpi_size len)
{
    ServUacpiUnmap(addr, len);
}

/*##########################################################################
#
#   Name       : log
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_log(enum uacpi_log_level lvl, const uacpi_char *text)
{
    const char *lstr;

    switch (lvl)
    {
        case UACPI_LOG_DEBUG:
            lstr = "DEBUG";
            break;

        case UACPI_LOG_TRACE:
            lstr = "TRACE";
            break;

        case UACPI_LOG_INFO:
            lstr = "INFO";
            break;

        case UACPI_LOG_WARN:
            lstr = "WARN";
            break;

        case UACPI_LOG_ERROR:
            lstr = "ERROR";
            break;

        default:
            lstr = "";
            break;
    }
    printf("[%s] %s", lstr, text);
}

/*##########################################################################
#
#   Name       : pci device open
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_pci_device_open(uacpi_pci_address address, uacpi_handle *out_handle)
{
    int ads;

    if (address.segment == 0)
    {
        ads = 0x80000000;
        ads |= (address.bus & 0xFF) << 16;
        ads |= (address.device & 0x1F) << 11;
        ads |= (address.function & 0x7) << 8;
        *out_handle = (uacpi_handle)ads;
        return UACPI_STATUS_OK;
    }
    else
    {
        printf("PCI segment not supported %d\n", address.segment);
        return UACPI_STATUS_NOT_FOUND;
    }
}

/*##########################################################################
#
#   Name       : pci device close
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_pci_device_close(uacpi_handle handle)
{
}

/*##########################################################################
#
#   Name       : pci read8
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_pci_read8(uacpi_handle device, uacpi_size offset, uacpi_u8 *value)
{
    int dev = (int)device;

    if (offset < 0x100)
    {
        *value = ServUacpiReadPciByte(dev + (offset & 0xFF));
        return UACPI_STATUS_OK;
    }
    else
    {
        printf("PCI offset error %d\n", offset);
        return UACPI_STATUS_NOT_FOUND;
    }
}

/*##########################################################################
#
#   Name       : pci read16
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_pci_read16(uacpi_handle device, uacpi_size offset, uacpi_u16 *value)
{
    int dev = (int)device;

    if (offset < 0x100)
    {
        *value = ServUacpiReadPciWord(dev + (offset & 0xFF));
        return UACPI_STATUS_OK;
    }
    else
    {
         printf("PCI offset error %d\n", offset);
         return UACPI_STATUS_NOT_FOUND;
    }
}

/*##########################################################################
#
#   Name       : pci read32
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_pci_read32(uacpi_handle device, uacpi_size offset, uacpi_u32 *value)
{
    int dev = (int)device;

    if (offset < 0x100)
    {
        *value = ServUacpiReadPciDword(dev + (offset & 0xFF));
        return UACPI_STATUS_OK;
    }
    else
    {
        printf("PCI offset error %d\n", offset);
        return UACPI_STATUS_NOT_FOUND;
    }
}

/*##########################################################################
#
#   Name       : pci write8
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_pci_write8(uacpi_handle device, uacpi_size offset, uacpi_u8 value)
{
    int dev = (int)device;

    if (offset < 0x100)
    {
        ServUacpiWritePciByte(dev + (offset & 0xFF), value);
        return UACPI_STATUS_OK;
    }
    else
    {
        printf("PCI offset error %d\n", offset);
        return UACPI_STATUS_NOT_FOUND;
    }
}

/*##########################################################################
#
#   Name       : pci write16
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_pci_write16(uacpi_handle device, uacpi_size offset, uacpi_u16 value)
{
    int dev = (int)device;

    if (offset < 0x100)
    {
        ServUacpiWritePciWord(dev + (offset & 0xFF), value);
        return UACPI_STATUS_OK;
    }
    else
    {
        printf("PCI offset error %d\n", offset);
        return UACPI_STATUS_NOT_FOUND;
    }
}

/*##########################################################################
#
#   Name       : pci write32
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_pci_write32(uacpi_handle device, uacpi_size offset, uacpi_u32 value)
{
    int dev = (int)device;

    if (offset < 0x100)
    {
        ServUacpiWritePciDword(dev + (offset & 0xFF), value);
        return UACPI_STATUS_OK;
    }
    else
    {
        printf("PCI offset error %d\n", offset);
        return UACPI_STATUS_NOT_FOUND;
    }
}

/*##########################################################################
#
#   Name       : io map
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_io_map(uacpi_io_addr base, uacpi_size len, uacpi_handle *out_handle)
{
    struct io_map *map = (struct io_map *)malloc(sizeof(struct io_map));

    map->base = base;
    map->size = len;
    ServUacpiEnableIo(base, len);

    *out_handle = map;
    return UACPI_STATUS_OK;
}

/*##########################################################################
#
#   Name       : io unmap
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_io_unmap(uacpi_handle handle)
{
    struct io_map *map = (struct io_map *)handle;

    free(map);
}

/*##########################################################################
#
#   Name       : io read 8
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_io_read8(uacpi_handle handle, uacpi_size offset, uacpi_u8 *out_value)
{
    struct io_map *map = (struct io_map *)handle;

    *out_value = in_byte(map->base + offset);
    return UACPI_STATUS_OK;
}

/*##########################################################################
#
#   Name       : io read 16
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_io_read16(uacpi_handle handle, uacpi_size offset, uacpi_u16 *out_value)
{
    struct io_map *map = (struct io_map *)handle;

    *out_value = in_word(map->base + offset);
    return UACPI_STATUS_OK;
}

/*##########################################################################
#
#   Name       : io read 32
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_io_read32(uacpi_handle handle, uacpi_size offset, uacpi_u32 *out_value)
{
    struct io_map *map = (struct io_map *)handle;

    *out_value = in_dword(map->base + offset);
    return UACPI_STATUS_OK;
}

/*##########################################################################
#
#   Name       : io write 8
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_io_write8(uacpi_handle handle, uacpi_size offset, uacpi_u8 in_value)
{
    struct io_map *map = (struct io_map *)handle;

    out_byte(map->base + offset, in_value);
    return UACPI_STATUS_OK;
}

/*##########################################################################
#
#   Name       : io write 16
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_io_write16(uacpi_handle handle, uacpi_size offset, uacpi_u16 in_value)
{
    struct io_map *map = (struct io_map *)handle;

    out_word(map->base + offset, in_value);
    return UACPI_STATUS_OK;
}

/*##########################################################################
#
#   Name       : io write 32
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_io_write32(uacpi_handle handle, uacpi_size offset, uacpi_u32 in_value)
{
    struct io_map *map = (struct io_map *)handle;

    out_dword(map->base + offset, in_value);
    return UACPI_STATUS_OK;
}

/*##########################################################################
#
#   Name       : alloc
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void *uacpi_kernel_alloc(uacpi_size size)
{
    return malloc(size);
}

/*##########################################################################
#
#   Name       : free
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_free(void *mem)
{
    free(mem);
}

/*##########################################################################
#
#   Name       : get time since boot
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void)
{
    long long diff;

    if (!TimeBase)
        TimeBase = RdosGetLongSysTime();

    diff = RdosGetLongSysTime() - TimeBase;
    diff = diff * 1000000 / 1193;

    return diff;
}

/*##########################################################################
#
#   Name       : stall
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_stall(uacpi_u8 usec)
{
    RdosWaitMicro(usec);
}

/*##########################################################################
#
#   Name       : sleep
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_sleep(uacpi_u64 msec)
{
    RdosWaitMilli(msec);
}

/*##########################################################################
#
#   Name       : create mutex
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_handle uacpi_kernel_create_mutex(void)
{
    struct RdosFutex *futex = (struct RdosFutex *)malloc(sizeof(struct RdosFutex));

    RdosInitFutex(futex, "UACPI");

    return (uacpi_handle)futex;
}

/*##########################################################################
#
#   Name       : free mutex
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_free_mutex(uacpi_handle handle)
{
    struct RdosFutex *futex = (struct RdosFutex *)handle;

    RdosResetFutex(futex);
    free(futex);
}

/*##########################################################################
#
#   Name       : create event
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_handle uacpi_kernel_create_event(void)
{
    struct event_t *ev = (struct event_t *)malloc(sizeof(struct event_t));

    ev->wait = RdosCreateWait();
    ev->handle = RdosCreateSignal();
    RdosAddWaitForSignal(ev->wait, ev->handle, 1);

    return (uacpi_handle)ev;
}

/*##########################################################################
#
#   Name       : free event
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_free_event(uacpi_handle handle)
{
    struct event_t *ev = (struct event_t *)handle;

    RdosFreeSignal(ev->handle);
    RdosCloseWait(ev->wait);
}

/*##########################################################################
#
#   Name       : get thread id
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_thread_id uacpi_kernel_get_thread_id(void)
{
    return (uacpi_thread_id)RdosGetThreadHandle();
}

/*##########################################################################
#
#   Name       : acquire mutex
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_acquire_mutex(uacpi_handle handle, uacpi_u16 timeout)
{
    struct RdosFutex *futex = (struct RdosFutex *)handle;

    RdosEnterFutex(futex);
    return UACPI_STATUS_OK;
}

/*##########################################################################
#
#   Name       : release mutex
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_release_mutex(uacpi_handle handle)
{
    struct RdosFutex *futex = (struct RdosFutex *)handle;

    RdosLeaveFutex(futex);
}

/*##########################################################################
#
#   Name       : wait for event
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle handle, uacpi_u16 timeout)
{
    printf("wait event\n");
    return false;
}

/*##########################################################################
#
#   Name       : signal event
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_signal_event(uacpi_handle handle)
{
    printf("signal event\n");
}

/*##########################################################################
#
#   Name       : reset event
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_reset_event(uacpi_handle handle)
{
    printf("reset event\n");
}

/*##########################################################################
#
#   Name       : handle firmware req
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_handle_firmware_request(uacpi_firmware_request *req)
{
    printf("fw req\n");
    return 0;
}

/*##########################################################################
#
#   Name       : install interrupt handler
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_install_interrupt_handler(uacpi_u32 irq, uacpi_interrupt_handler handler, uacpi_handle ctx, uacpi_handle *out_irq_handle)
{
    printf("install IRQ handler\n");
    return 0;
}

/*##########################################################################
#
#   Name       : uninstall interrupt handler
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_uninstall_interrupt_handler(uacpi_interrupt_handler handler, uacpi_handle irq_handle)
{
    printf("uninstall IRQ handler\n");
    return 0;
}

/*##########################################################################
#
#   Name       : create spinlock
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_handle uacpi_kernel_create_spinlock(void)
{
    int *lock = (int *)malloc(sizeof(int));

    *lock = 0;
    return (uacpi_handle)lock;
}

/*##########################################################################
#
#   Name       : free spinlock
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_free_spinlock(uacpi_handle handle)
{
    int *lock = (int *)handle;

    free(lock);
}

/*##########################################################################
#
#   Name       : lock spinlock
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle handle)
{
    int *lock = (int *)handle;
    bool done = false;
    int val;

    while (!done)
    {
        val = lock_spinlock(lock);
        if (val)
            RdosWaitMilli(10);
        else
            done = true;
    }

    return 0;
}

/*##########################################################################
#
#   Name       : unlock spinlock
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void uacpi_kernel_unlock_spinlock(uacpi_handle handle, uacpi_cpu_flags flags)
{
    int *lock = (int *)handle;
    *lock = 0;
}

/*##########################################################################
#
#   Name       : schedule work
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_schedule_work(uacpi_work_type type, uacpi_work_handler handler, uacpi_handle ctx)
{
    printf("schedule work\n");
    return 0;
}

/*##########################################################################
#
#   Name       : wait for work
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
uacpi_status uacpi_kernel_wait_for_work_completion(void)
{
    printf("wait for work\n");
    return 0;
}

/*##########################################################################
#
#   Name       : InitPci
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void InitPci()
{
    ServUacpiEnableIo(0xCF8, 4);
    ServUacpiEnableIo(0xCFC, 4);
}

/*##########################################################################
#
#   Name       : ReadPciByte
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
char ReadPciByte(unsigned char bus, char device, char function, char reg)
{
    int ads;

    ads = 0x80000000;
    ads |= bus << 16;
    ads |= (device & 0x1F) << 11;
    ads |= (function & 0x7) << 8;
    ads |= reg;

    return read_pci_byte(ads);
}

/*##########################################################################
#
#   Name       : ReadPciWord
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
short ReadPciWord(unsigned char bus, char device, char function, char reg)
{
    int ads;

    ads = 0x80000000;
    ads |= bus << 16;
    ads |= (device & 0x1F) << 11;
    ads |= (function & 0x7) << 8;
    ads |= reg;

    return read_pci_word(ads);
}

/*##########################################################################
#
#   Name       : ReadPciDword
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
int ReadPciDword(unsigned char bus, char device, char function, char reg)
{
    int ads;

    ads = 0x80000000;
    ads |= bus << 16;
    ads |= (device & 0x1F) << 11;
    ads |= (function & 0x7) << 8;
    ads |= reg;

    return read_pci_dword(ads);
}

/*##########################################################################
#
#   Name       : WritePciByte
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void WritePciByte(unsigned char bus, char device, char function, char reg, char val)
{
    int ads;

    ads = 0x80000000;
    ads |= bus << 16;
    ads |= (device & 0x1F) << 11;
    ads |= (function & 0x7) << 8;
    ads |= reg;
    write_pci_byte(ads, val);
}

/*##########################################################################
#
#   Name       : WritePciWord
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void WritePciWord(unsigned char bus, char device, char function, char reg, short val)
{
    int ads;

    ads = 0x80000000;
    ads |= bus << 16;
    ads |= (device & 0x1F) << 11;
    ads |= (function & 0x7) << 8;
    ads |= reg;
    write_pci_word(ads, val);
}

/*##########################################################################
#
#   Name       : WritePciDword
#
#   Purpose....:
#
#   In params..: *
#   Out params.: *
#   Returns....: *
#
##########################################################################*/
void WritePciDword(unsigned char bus, char device, char function, char reg, int val)
{
    int ads;

    ads = 0x80000000;
    ads |= bus << 16;
    ads |= (device & 0x1F) << 11;
    ads |= (function & 0x7) << 8;
    ads |= reg;
    write_pci_dword(ads, val);
}
