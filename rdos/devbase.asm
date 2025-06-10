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
; devbase.ASM
; Basic dev server
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include acpi.def
include acpi.inc

.386p

dev_cmd_struc   STRUC

fc_op              DD ?
fc_handle          DD ?
fc_buf             DD ?,?
fc_size            DD ?
fc_eflags          DD ?
fc_eax             DD ?
fc_ebx             DD ?
fc_ecx             DD ?
fc_edx             DD ?
fc_esi             DD ?
fc_edi             DD ?

dev_cmd_struc   ENDS

;;;;;;;;; INTERNAL PROCEDURES ;;;;;;;;;;;

_TEXT   segment use32 word public 'CODE'

    assume  cs:_TEXT

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalFindClass
;
;       DESCRIPTION:    Find class
;
;       PARAMETERS:     AH      Class
;                       AL      Subclass
;                       BX      Start dev
;
;       RETURNS:        BX      Found dev or 0
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowFindClass:near

LocalFindClass Proc near
    push edi
    movzx ebx,bx
    call LowFindClass
    pop edi
;
    or eax,eax
    jz fcFail
;    
    mov [edi].fc_ebx,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret

fcFail:
    mov ebx,[edi].fc_handle
    ReplyDevCmd
    ret
LocalFindClass Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalFindClassProtocol
;
;       DESCRIPTION:    Find class & protocol
;
;       PARAMETERS:     AH      Class
;                       AL      Subclass
;                       DL      Protocol
;                       BX      Start dev
;
;       RETURNS:        BX      Found dev or 0
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowFindClassProtocol:near

LocalFindClassProtocol Proc near
    push edi
    movzx ebx,bx
    call LowFindClassProtocol
    pop edi
;
    or eax,eax
    jz fciFail
;    
    mov [edi].fc_ebx,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret

fciFail:
    mov ebx,[edi].fc_handle
    ReplyDevCmd
    ret
LocalFindClassProtocol Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalFindDevice
;
;       DESCRIPTION:    Find vendor & device
;
;       PARAMETERS:     CX      Device
;                       DX      Vendor
;                       BX      Start dev
;
;       RETURNS:        BX      Found dev or 0
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowFindDevice:near

LocalFindDevice Proc near
    push edi
    movzx ebx,bx
    movzx ecx,cx
    movzx edx,dx
    call LowFindDevice
    pop edi
;
    or eax,eax
    jz fcdFail
;    
    mov [edi].fc_ebx,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret

fcdFail:
    mov ebx,[edi].fc_handle
    ReplyDevCmd
    ret
LocalFindDevice Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalGetHandle
;
;       DESCRIPTION:    Get handle
;
;       PARAMETERS:     DH      Segment
;                       DL      Bus
;                       AH      Device
;                       AL      Function
;
;       RETURNS:        BX      Found dev or 0
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowGetHandle:near

LocalGetHandle Proc near
    push edi
    movzx edx,dx
    movzx ecx,ax
    call LowGetHandle
    pop edi
;
    or eax,eax
    jz ghFail
;    
    mov [edi].fc_ebx,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret

ghFail:
    mov ebx,[edi].fc_handle
    ReplyDevCmd
    ret
LocalGetHandle Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalGetParam
;
;       DESCRIPTION:    Get param
;
;       PARAMETERS:     BX      Handle
;
;       RETURNS:        DH      Segment
;                       DL      Bus
;                       AH      Device
;                       AL      Function
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowGetParam:near

LocalGetParam Proc near
    push edi
    movzx ebx,bx
    call LowGetParam
    pop edi
;    
    mov edx,eax
    shr edx,16
    mov [edi].fc_eax,eax
    mov [edi].fc_edx,edx
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalGetParam Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalGetBus
;
;       DESCRIPTION:    Get bus
;
;       PARAMETERS:     DH      Segment
;                       DL      Bus
;
;       RETURNS:        DH      Segment
;                       DL      Bus
;                       AH      Device
;                       AL      Function
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowGetBus:near

LocalGetBus Proc near
    push edi
    call LowGetBus
    pop edi
;    
    cmp eax,-1
    je gbFail
;    
    mov edx,eax
    shr edx,16
    mov [edi].fc_eax,eax
    mov [edi].fc_edx,edx
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret

gbFail:
    mov ebx,[edi].fc_handle
    ReplyDevCmd
    ret
LocalGetBus Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalGetIrq
;
;       DESCRIPTION:    Get IRQ
;
;       PARAMETERS:     BX      Handle
;                       AX      Index
;
;       RETURNS:        AL      IRQ
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowGetIrq:near

LocalGetIrq Proc near
    push edi
    movzx ebx,bx
    movzx edi,ax
    call LowGetIrq
    pop edi
;    
    mov [edi].fc_eax,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalGetIrq Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalGetMsi
;
;       DESCRIPTION:    Get MSI
;
;       PARAMETERS:     BX      Handle
;
;       RETURNS:        AL      Vectors
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowGetMsi:near

LocalGetMsi Proc near
    push edi
    movzx ebx,bx
    call LowGetMsi
    pop edi
;    
    mov [edi].fc_eax,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalGetMsi Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalGetMsiX
;
;       DESCRIPTION:    Get MSI-X
;
;       PARAMETERS:     BX      Handle
;
;       RETURNS:        AL      Vectors
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowGetMsiX:near

LocalGetMsiX Proc near
    push edi
    movzx ebx,bx
    call LowGetMsiX
    pop edi
;    
    mov [edi].fc_eax,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalGetMsiX Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalGetCap
;
;       DESCRIPTION:    Get capability
;
;       PARAMETERS:     BX      Handle
;                       AL      Capability
;
;       RETURNS:        AX      Register
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowGetCap:near

LocalGetCap Proc near
    push edi
    movzx ebx,bx
    mov edi,[edi].fc_eax
    call LowGetCap
    pop edi
;    
    mov [edi].fc_eax,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalGetCap Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalGetPciName
;
;       DESCRIPTION:    Get PCI device name
;
;       PARAMETERS:     BX      Handle
;                       EDI     Buffer
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowGetPciName:near

LocalGetPciName Proc near
    push esi
    push edi
    movzx ebx,bx
    add edi,SIZE dev_cmd_struc
    add edi,4
    mov ecx,1000h - SIZE dev_cmd_struc - 5
    call LowGetPciName
    pop edi
    pop esi
;
    or eax,eax
    jz gpnFail
;
    push edi
    add edi,SIZE dev_cmd_struc
    stosd
    pop edi
;
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevDataCmd
    ret

gpnFail:
    mov ebx,[edi].fc_handle
    ReplyDevCmd
    ret
LocalGetPciName Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalReadPciByte
;
;       DESCRIPTION:    Read PCI config byte
;
;       PARAMETERS:     DX      Issuer
;                       BX      Handle
;                       CX      Register
;
;       RETURNS:        AL      Value
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowReadConfigByte:near

LocalReadConfigByte Proc near
    push edi
    movzx ebx,bx
    movzx ecx,cx
    call LowReadConfigByte
    pop edi
;
    mov [edi].fc_eax,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalReadConfigByte Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalReadPciWord
;
;       DESCRIPTION:    Read PCI config word
;
;       PARAMETERS:     DX      Issuer
;                       BX      Handle
;                       CX      Register
;
;       RETURNS:        AX      Value
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowReadConfigWord:near

LocalReadConfigWord Proc near
    push edi
    movzx ebx,bx
    movzx ecx,cx
    call LowReadConfigWord
    pop edi
;
    mov [edi].fc_eax,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalReadConfigWord Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalReadPciDword
;
;       DESCRIPTION:    Read PCI config dword
;
;       PARAMETERS:     DX      Issuer
;                       BX      Handle
;                       CX      Register
;
;       RETURNS:        EAX      Value
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowReadConfigDword:near

LocalReadConfigDword Proc near
    push edi
    movzx ebx,bx
    movzx ecx,cx
    call LowReadConfigDword
    pop edi
;
    mov [edi].fc_eax,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalReadConfigDword Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalWritePciByte
;
;       DESCRIPTION:    Write PCI config byte
;
;       PARAMETERS:     DX      Issuer
;                       BX      Handle
;                       CX      Register
;                       AL      Value
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowWriteConfigByte:near

LocalWriteConfigByte Proc near
    push edi
    movzx ebx,bx
    movzx ecx,cx
    mov edi,[edi].fc_eax
    call LowWriteConfigByte
    pop edi
;
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalWriteConfigByte Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalWritePciWord
;
;       DESCRIPTION:    Write PCI config word
;
;       PARAMETERS:     DX      Issuer
;                       BX      Handle
;                       CX      Register
;                       AX      Value
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowWriteConfigWord:near

LocalWriteConfigWord Proc near
    push edi
    movzx ebx,bx
    movzx ecx,cx
    mov edi,[edi].fc_eax
    call LowWriteConfigWord
    pop edi
;
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalWriteConfigWord Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalWritePciDword
;
;       DESCRIPTION:    Write PCI config dword
;
;       PARAMETERS:     DX      Issuer
;                       BX      Handle
;                       CX      Register
;                       EAX     Value
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowWriteConfigDword:near

LocalWriteConfigDword Proc near
    push edi
    movzx ebx,bx
    movzx ecx,cx
    mov edi,[edi].fc_eax
    call LowWriteConfigDword
    pop edi
;
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalWriteConfigDword Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalLockPci
;
;       DESCRIPTION:    Local lock pci
;
;       PARAMETERS:     DX      Issuer
;                       BX      Handle
;                       Data    Name
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowLockPci:near

LocalLockPci Proc near
    push edi
    movzx ebx,bx
    add edi,SIZE dev_cmd_struc
    call LowLockPci
    pop edi
;    
    or eax,eax
    jz llFail
;
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret

llFail:
    mov ebx,[edi].fc_handle
    ReplyDevCmd
    ret
LocalLockPci Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalUnlockPci
;
;       DESCRIPTION:    Local unlock PCI
;
;       PARAMETERS:     DX      Issuer
;                       BX      Handle
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowUnlockPci:near

LocalUnlockPci Proc near
    push edi
    movzx ebx,bx
    call LowUnlockPci
    pop edi
;    
    or eax,eax
    jz luFail
;
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret

luFail:
    mov ebx,[edi].fc_handle
    ReplyDevCmd
    ret
LocalUnlockPci Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalSetupIrq
;
;       DESCRIPTION:    Local setup IRQ
;
;       PARAMETERS:     DX      Issuer
;                       AH      Priority
;                       BX      Handle
;                       SI      Core
;
;       RETURNS:        AL      Vector
;                       AH      Mode, 0 = fail, 1 = IRQ, 2 = MSI
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowSetupIrq:near

LocalSetupIrq Proc near
    push edi
    movzx ebx,bx
    movzx edx,dx
    movzx edi,ah
    movzx esi,si
    call LowSetupIrq
    pop edi
;
    mov [edi].fc_eax,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalSetupIrq Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalSetupMsi
;
;       DESCRIPTION:    Local setup MSI
;
;       PARAMETERS:     DX      Issuer
;                       AH      Priority
;                       BX      Handle
;                       CX      Requested vectors
;                       SI      Core
;
;       RETURNS:        CX      Allocated vectors
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowSetupMsi:near

LocalSetupMsi Proc near
    push edi
    movzx ebx,bx
    movzx edx,dx
    movzx edi,ah
    movzx ecx,cx
    movzx esi,si
    call LowSetupMsi
    pop edi
;
    mov [edi].fc_ecx,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalSetupMsi Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalEnableMsi
;
;       DESCRIPTION:    Local enable MSI
;
;       PARAMETERS:     DX      Issuer
;                       BX      Handle
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowEnableMsi:near

LocalEnableMsi Proc near
    push edi
    movzx ebx,bx
    movzx edx,dx
    call LowEnableMsi
    pop edi
;
    mov [edi].fc_ecx,eax
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret
LocalEnableMsi Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           LocalIsLocked
;
;       DESCRIPTION:    Local is locked
;
;       PARAMETERS:     BX      Handle
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    extern LowIsPciLocked:near

LocalIsLocked Proc near
    push edi
    movzx edx,dx
    call LowIsPciLocked
    pop edi
;    
    or eax,eax
    jz ilFail
;
    mov ebx,[edi].fc_handle
    and [edi].fc_eflags,NOT 1
    ReplyDevCmd
    ret

ilFail:
    mov ebx,[edi].fc_handle
    ReplyDevCmd
    ret
LocalIsLocked Endp

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       
;
;       NAME:           WaitForMsg
;
;       DESCRIPTION:    Wait for msg
;
;       PARAMETERS:     EBX     Handle
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    public WaitForMsg_


Unused   Proc near
    ret
Unused   Endp

msgtab:
m00 DD OFFSET LocalFindClass
m01 DD OFFSET LocalFindClassProtocol
m02 DD OFFSET LocalFindDevice
m03 DD OFFSET LocalGetHandle
m04 DD OFFSET LocalGetParam
m05 DD OFFSET LocalGetBus
m06 DD OFFSET LocalGetIrq
m07 DD OFFSET LocalGetCap
m08 DD OFFSET LocalGetPciName
m09 DD OFFSET LocalReadConfigByte
m010 DD OFFSET LocalReadConfigWord
m011 DD OFFSET LocalReadConfigDword
m012 DD OFFSET LocalWriteConfigByte
m013 DD OFFSET LocalWriteConfigWord
m014 DD OFFSET LocalWriteConfigDword
m015 DD OFFSET LocalLockPci
m016 DD OFFSET LocalUnlockPci
m017 DD OFFSET LocalGetMsi
m018 DD OFFSET LocalGetMsiX
m019 DD OFFSET LocalSetupIrq
m020 DD OFFSET LocalSetupMsi
m021 DD OFFSET LocalEnableMsi
m022 DD OFFSET LocalIsLocked

WaitForMsg_    Proc near
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
;
    xor eax,eax
    WaitForDevCmd
    jc wfmDone
;
    mov edi,edx
    mov [edi].fc_handle,ebx
    mov eax,[edi].fc_eax
    mov ebx,[edi].fc_ebx
    mov ecx,[edi].fc_ecx
    mov esi,[edi].fc_esi
    mov ebp,[edi].fc_op
    mov edx,[edi].fc_edx
    shl ebp,2
    call dword ptr [ebp].msgtab
    mov eax,1

wfmDone:
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret
WaitForMsg_    Endp

_TEXT   ends

    END
