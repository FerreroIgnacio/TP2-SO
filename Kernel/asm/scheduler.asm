GLOBAL scheduler_save_and_switch
EXTERN scheduler_switch
SECTION .text

; registers + context backup
%macro backupRegisters 1

    mov [%1 + 0*8],  rax    ; rax

    pushfq
    pop rax
    mov [%1 + 16*8], rax    ; rflags

    pop rax
    mov [%1 + 17*8], rax    ; rip

    mov ax, cs
    movzx rax, ax
    mov [%1 + 18*8], rax    ; cs

    mov ax, ss
    movzx rax, ax
    mov [%1 + 19*8], rax    ; ss
    
    mov [%1 + 7*8], rsp     ; rsp
    
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

scheduler_save_and_switch:
    backupRegisters backup
    mov rdi, backup
    call scheduler_switch

SECTION .bss
    backup resq 20