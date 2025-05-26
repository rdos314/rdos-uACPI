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
# pci.h
# PCI header
#
########################################################################*/

#ifndef _PCI_H
#define _PCI_H

#define PCI_vendorID        0
#define PCI_deviceID        2
#define PCI_command_reg     4
#define PCI_status_reg      6
#define PCI_revisionID      8
#define PCI_progIF          9
#define PCI_subclass        10
#define PCI_classcode       11
#define PCI_cacheline_size  12
#define PCI_latency         13
#define PCI_header_type     14
#define PCI_BIST            15
#define PCI_interrupt_line  60
#define PCI_interrupt_pin   61

#endif
