;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; RDOS operating system
; Copyright (C) 1988-2011, Leif Ekblad
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version. The only exception to this rule
; is for commercial usage in embedded systems. For information on
; usage in commercial embedded systems, contact embedded@rdos.net
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
;
; The author of this program may be contacted at leif@rdos.net
;
; devmsg.inc
; uACPI msg definitions
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; IN  AH    Class
; IN  AL    Sub class
; IN  BX    Start handle
; OUT BX    Handle
FIND_PCI_CLASS_CMD         = 0

; IN  AH    Class
; IN  AL    Sub class
; IN  DL    Protocol
; IN  BX    Start handle
; OUT BX    Handle
FIND_PCI_PROTOCOL_CMD      = 1

; IN  CX    Device
; IN  DX    Vendor
; IN  BX    Start handle
; OUT BX    Handle
FIND_PCI_DEVICE_CMD        = 2

; IN  DH    Segment
; IN  DL    Bus
; IN  AH    Device
; IN  BX    Function
; OUT BX    Handle
GET_PCI_HANDLE_CMD         = 3


; IN  BX    Handle
; OUT DH    Segment
; OUT DL    Bus
; OUT AH    Device
; OUT AL    Function
GET_PCI_PARAM_CMD          = 4

; IN  DH    Segment
; IN  DL    Bus
; OUT DL    Bus
; OUT AH    Device
; OUT AL    Function
GET_PCI_BUS_CMD            = 5

; IN  BX    Handle
; IN  AX    Index
; OUT AL    IRQ
GET_PCI_IRQ_CMD            = 6

; IN  BX    Handle
; IN  AL    Capability
; OUT AX    Register
GET_PCI_CAP_CMD            = 7

; IN  BX    Handle
;           Buffer
GET_PCI_NAME_CMD           = 8

; IN  BX    Handle
; IN  CX    Register
; IN  DX    Issuer selector
; OUT AL    Value
READ_PCI_CONFIG_BYTE_CMD   = 9

; IN  BX    Handle
; IN  CX    Register
; IN  DX    Issuer selector
; OUT AX    Value
READ_PCI_CONFIG_WORD_CMD   = 10

; IN  BX    Handle
; IN  CX    Register
; IN  DX    Issuer selector
; OUT EAX   Value
READ_PCI_CONFIG_DWORD_CMD  = 11

; IN  AL    Value
; IN  BX    Handle
; IN  CX    Register
; IN  DX    Issuer selector
WRITE_PCI_CONFIG_BYTE_CMD  = 12

; IN  AX    Value
; IN  BX    Handle
; IN  CX    Register
; IN  DX    Issuer selector
WRITE_PCI_CONFIG_WORD_CMD  = 13

; IN  EAX   Value
; IN  BX    Handle
; IN  CX    Register
; IN  DX    Issuer selector
WRITE_PCI_CONFIG_DWORD_CMD = 14

; IN  BX     Handle
; IN  DX     Issuer selector
; IN  ES:EDI Name
LOCK_PCI_CMD               = 15

; IN  BX     Handle
; IN  DX     Issuer selector
UNLOCK_PCI_CMD             = 16

; IN  BX     Handle
; OUT AL     Vectors
GET_PCI_MSI_CMD            = 17

; IN  BX     Handle
; OUT AL     Vectors
GET_PCI_MSIX_CMD           = 18

; IN  AH     Priority
; IN  BX     Handle
; IN  DX     Issuer selector
; IN  SI     Core ID
; OUT AL     Vector
; OUT CL     Mode, 0 = failed, 1 = IRQ, 2 = MSI
SETUP_PCI_IRQ_CMD          = 19

; IN  AH     Priority
; IN  BX     Handle
; IN  DX     Issuer selector
; IN  SI     Core ID
; IN  CX     Requested vectors
; OUT CX     Allocated vectors
SETUP_PCI_MSI_CMD          = 20

; IN  BX     Handle
; IN  DX     Issuer selector
ENABLE_PCI_MSI_CMD         = 21

; IN  BX     Handle
IS_PCI_LOCKED_CMD          = 22

; IN  AL      Bar #
; IN  BX      Handle
; OUT EDX:EAX Physical address
GET_PCI_BAR_PHYS_CMD       = 23

; IN  AL      Bar #
; IN  BX      Handle
; OUT DX      IO port
GET_PCI_BAR_IO_CMD         = 24

RESET_CMD                  = 25
