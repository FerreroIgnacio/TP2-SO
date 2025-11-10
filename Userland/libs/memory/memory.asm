GLOBAL malloc
GLOBAL calloc
GLOBAL realloc
GLOBAL free
GLOBAL getMemInfo

SECTION .text

malloc:
    mov     rax, 0x1E     ; syscall número 30
    int     0x80
    ret

calloc:
    mov     rax, 0x1F     ; syscall número 31
    int     0x80
    ret

realloc:
    mov     rax, 0x20     ; syscall número 32
    int     0x80
    ret

free:
    mov     rax, 0x21     ; syscall número 33
    int     0x80
    ret

getMemInfo:
    mov     rax, 0x22     ; syscall número 34
    int     0x80
    ret