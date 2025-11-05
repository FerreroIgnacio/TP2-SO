GLOBAL kernel_setRegisters
GLOBAL kernel_getRegisters

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

; sys_setRegister(rdi = pointer to reg_screenshot_t)
; Loads all CPU registers from the provided struct and transfers control to RIP.
; Struct layout (qword each, in order):
; 0: rax, 1: rbx, 2: rcx, 3: rdx, 4: rsi, 5: rdi, 6: rbp, 7: rsp,
; 8: r8, 9: r9, 10: r10, 11: r11, 12: r12, 13: r13, 14: r14, 15: r15,
; 16: rflags, 17: rip
kernel_setRegisters:
    ; Preserve a base pointer to the struct in RBP while we overwrite registers
    push rbp
    mov  rbp, rdi

    ; Load general-purpose registers (except rbp, rsp)
    mov  rax, [rbp + 0*8]
    mov  rbx, [rbp + 1*8]
    mov  rcx, [rbp + 2*8]
    mov  rdx, [rbp + 3*8]
    mov  rsi, [rbp + 4*8]
    mov  rdi, [rbp + 5*8]

    mov  r8,  [rbp + 8*8]
    mov  r9,  [rbp + 9*8]
    mov  r10, [rbp + 10*8]
    mov  r11, [rbp + 11*8]
    mov  r12, [rbp + 12*8]
    mov  r13, [rbp + 13*8]
    mov  r14, [rbp + 14*8]
    mov  r15, [rbp + 15*8]

    ; Load RFLAGS
    mov  rax, [rbp + 16*8]
    push rax
    popfq

    ; Prepare transfer: load RIP into RAX, then set RSP and RBP
    mov  rax, [rbp + 17*8]    ; target RIP
    mov  rsp, [rbp + 7*8]     ; target RSP
    mov  rbp, [rbp + 6*8]     ; target RBP

    ; Jump to RIP via RET using the new stack
    push rax
    ret

kernel_getRegisters:
    backupRegisters regs_backup
    mov rdi, regs_backup
    ret

section .bss
    regs_backup resq 18
