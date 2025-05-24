; idt.asm
GLOBAL idtStart
EXTERN idtTable

SECTION .data
idt_descriptor:
    dw 256 * 16 - 1    ; limit (256 entradas * 16 bytes - 1)
    dq idtTable       ; base
section .text
idtStart:
    lidt [idt_descriptor]
    sti
    ret
