; Copyright (c) 2004, Intel Corporation. All rights reserved.<BR>
; This program and the accompanying materials                          
; are licensed and made available under the terms and conditions of the BSD License         
; which accompanies this distribution.  The full text of the license may be found at        
; http://opensource.org/licenses/bsd-license.php                                            
;                                                                                           
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.    
;
; Module Name:
;
;   WriteDr7.Asm
;
; Abstract:
;
;   AsmWriteDr7 function
;
; Notes:
;
;------------------------------------------------------------------------------

    .code

;------------------------------------------------------------------------------
; UINTN
; EFIAPI
; AsmWriteDr7 (
;   IN UINTN Value
;   );
;------------------------------------------------------------------------------
AsmWriteDr7 PROC
    mov     dr7, rcx
    mov     rax, rcx
    ret
AsmWriteDr7 ENDP

    END
