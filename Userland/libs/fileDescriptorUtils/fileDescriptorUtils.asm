GLOBAL write
GLOBAL read
GLOBAL fd_open
GLOBAL fd_list

SECTION .text

write:
    mov rax, 1              ; syscall ID para write
    int 0x80
    ret
read:
    mov rax, 0              ; syscall ID para read
    int 0x80
    ret

; open dynamic FD by name: returns fd>=3 or -1
fd_open:
    mov     rax, 0x46     ; syscall número 70
    int     0x80
    ret

; list dynamic FDs: rdi=out, rsi=max; returns count
fd_list:
    mov     rax, 0x47     ; syscall número 71
    int     0x80
    ret
