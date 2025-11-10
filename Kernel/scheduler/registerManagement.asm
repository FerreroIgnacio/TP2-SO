GLOBAL interrupt_setRegisters
GLOBAL reg_read_cs
GLOBAL reg_read_ss
GLOBAL reg_read_rflags

SECTION .text

interrupt_setRegisters:

    mov  rbp, rdi

    ; Load general-purpose registers (except rip, cs, flags, rsp, ss, rbp)
    
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


    ; SS
    mov rax, [rbp + 19*8]
    push rax

    ; RSP 
    mov rax, [rbp + 7*8]
    push rax

    ; RFLAGS
    mov  rax, [rbp + 16*8]
    push rax
    
    ; CS 
    mov  rax, [rbp + 18*8]
    push rax 
    
    ; RIP
    mov rax, [rbp + 17*8]
    push rax

    mov al, 0x20
    out 0x20, al

    mov  rax, [rbp + 0*8]
    mov  rbp, [rbp + 6*8]
    
    iretq   

; Read current CS selector
reg_read_cs:
    xor eax, eax
    mov ax, cs
    ret

; Read current SS selector
reg_read_ss:
    xor eax, eax
    mov ax, ss
    ret

; Read current RFLAGS
reg_read_rflags:
    pushfq
    pop rax
    ret
