GLOBAL startSound
GLOBAL stopSound

SECTION .text

startSound:
    mov     rax, 0x14     ; syscall número 20
    int     0x80          ; llamar a syscall
    ret

stopSound:
    mov     rax, 0x15     ; syscall número 21
    int     0x80          ; llamar a syscall
    ret
