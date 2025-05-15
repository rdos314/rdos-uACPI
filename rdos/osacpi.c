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
#include "serv.h"

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
	long long phys = ServUacpiGetRdsp();
	
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return 0;
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
	return (void *)RdosGetThreadHandle();
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
	return 0;
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
	_asm int 3
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
	return 0;
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
	return 0;
}
