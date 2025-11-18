GLOBAL write
GLOBAL read
GLOBAL fd_open
GLOBAL fd_list
GLOBAL pipe_create
GLOBAL fd_bind_std

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

; pipe_create: returns pipe id
pipe_create:
    mov     rax, 0x48     ; syscall número 72
    int     0x80
    ret

; fd_bind_std: rdi=pid, rsi=which(0 stdin/1 stdout), rdx=pipe_id; returns 0/-1
fd_bind_std:
    mov     rax, 0x49     ; syscall número 73
    int     0x80
    ret
