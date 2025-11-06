[BITS 64]
section .text
global _jumpToUserland

_jumpToUserland:
    cli
    mov ax, 0x23        ; user data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; rdi = entryPoint, rsi = stack
    mov rax, rsi
    push 0x23           ; SS
    push rax            ; RSP
    pushfq
    push 0x1B           ; CS
    push rdi            ; RIP
    iretq

