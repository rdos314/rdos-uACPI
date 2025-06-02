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
