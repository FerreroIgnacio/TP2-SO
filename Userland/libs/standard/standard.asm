

GLOBAL getRegisters

SECTION .text

getRegisters:
    mov     rax, 0x6        ; ID de syscall para  getRegisters (6)
    int     0x80
    ret



