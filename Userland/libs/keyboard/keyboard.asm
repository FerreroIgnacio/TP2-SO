GLOBAL isKeyPressed

SECTION .text


isKeyPressed:
    mov rax, 2
    int 0x80
    ret
