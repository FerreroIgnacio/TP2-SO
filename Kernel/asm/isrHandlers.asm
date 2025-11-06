GLOBAL ex00Handler
GLOBAL ex06Handler
GLOBAL irq01Handler
GLOBAL irq00Handler
GLOBAL syscallInterruptHandler
GLOBAL enableTimerIRQ
GLOBAL endIrq00
GLOBAL getInteruptRegsBackup


EXTERN keyPressedAction
EXTERN syscallHandler
EXTERN timerTickHandler
EXTERN zeroDivDispatcher
EXTERN invalidInstrucionDispatcher
EXTERN getStackBase
EXTERN exceptionDispatcher
EXTERN loader

SECTION .text

; backupRegisters espera rip en [rsp]
%macro backupRegisters 1
    push rax

	mov rax, [rsp+8]      
	mov [%1 + 17*8], rax    ; rip 
    mov rax, [rsp+24]
    mov [%1 + 16*8], rax    ; flags

    pop rax

    mov [%1 + 0*8],  rax
    mov [%1 + 1*8],  rbx
    mov [%1 + 2*8],  rcx
    mov [%1 + 3*8],  rdx
    mov [%1 + 4*8],  rsi
    mov [%1 + 5*8],  rdi
    mov [%1 + 6*8],  rbp
    mov [%1 + 7*8],  rsp
    mov [%1 + 8*8],  r8
    mov [%1 + 9*8],  r9
    mov [%1 + 10*8], r10
    mov [%1 + 11*8], r11
    mov [%1 + 12*8], r12
    mov [%1 + 13*8], r13
    mov [%1 + 14*8], r14
    mov [%1 + 15*8], r15
%endmacro

; registers + context backup
%macro interruptBackupRegisters 1
    push rax

    ; Los valores de RIP, CS, RFLAGS, RSP y SS vienen del stack
    mov rax, [rsp+8]      
    mov [%1 + 17*8], rax    ; rip 
    mov rax, [rsp+16]
    mov [%1 + 18*8], rax    ; cs
    mov rax, [rsp+24]
    mov [%1 + 16*8], rax    ; rflags
    mov rax, [rsp+32]
    mov [%1 + 7*8], rax     ; rsp
    mov rax, [rsp+40]
    mov [%1 + 19*8], rax    ; ss

    pop rax

    mov [%1 + 0*8],  rax
    mov [%1 + 1*8],  rbx
    mov [%1 + 2*8],  rcx
    mov [%1 + 3*8],  rdx
    mov [%1 + 4*8],  rsi
    mov [%1 + 5*8],  rdi
    mov [%1 + 6*8],  rbp
    mov [%1 + 8*8],  r8
    mov [%1 + 9*8],  r9
    mov [%1 + 10*8], r10
    mov [%1 + 11*8], r11
    mov [%1 + 12*8], r12
    mov [%1 + 13*8], r13
    mov [%1 + 14*8], r14
    mov [%1 + 15*8], r15
%endmacro

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro exceptionHandler 1

    backupRegisters exRegsBackup

	pushState
    
    sti
	
    mov rdi, %1 
    mov rsi, exRegsBackup
	call exceptionDispatcher

    call loader     ; reiniciar sistema 
    
    popState

    iretq           ; no debería llegar a esta instrucción
%endmacro

ex00Handler:    ; División por cero
    exceptionHandler 0


ex06Handler:    ; Instrucción inválida
    exceptionHandler 6


enableTimerIRQ:
    in al, 0x21        ; Read current PIC mask
    and al, 0xFE       ; Clear bit 0 (enable IRQ0/timer) - 0xFE = 11111110
    out 0x21, al       ; Write back the mask
    ret

irq00Handler:               ; Solo tick del sistema y EOI
    interruptBackupRegisters interruptRegsBackup
    
    push rax
    push rdi
    
    mov rdi, interruptRegsBackup

    call timerTickHandler
    
    mov al, 0x20
    out 0x20, al

    pop rdi
    pop rax
    iretq

irq01Handler:			; Keyboard irq
    backupRegisters irq01RegsBackup
    
    push rax
    push rdi
    push rsi
    
    in al, 0x60
    movzx rdi, al
    
    mov rsi, irq01RegsBackup

    
    call keyPressedAction

    mov al, 0x20
    out 0x20, al

    pop rsi
    pop rdi
    pop rax
    iretq

syscallInterruptHandler:
    interruptBackupRegisters interruptRegsBackup

    push r8
    push r9
    push r10
    push r11
    push r12                
                        ;(syscall_id, arg1, arg2, arg3, arg4, arg5, arg6)
    push r12            ; arg6
    mov r9 , r8         ; arg5 (original r8) -> r9
    mov r8 , rcx        ; arg4 (original rcx) -> r8
    mov rcx, rdx        ; arg3 (original rdx) -> rcx
    mov rdx, rsi        ; arg2 (original rsi) -> rdx
    mov rsi, rdi        ; arg1 (original rdi) -> rsi
    mov rdi, rax        ; syscall num         -> rdi

    call syscallHandler
    add rsp, 8          

    pop r12
    pop r11
    pop r10
    pop r9
    pop r8

    iretq 

getInteruptRegsBackup:
    mov rdi, interruptRegsBackup
    ret

SECTION .rodata
    userland equ 0x400000

section .bss
    exRegsBackup resq 18
    irq01RegsBackup resq 18
    interruptRegsBackup resq 20
    


