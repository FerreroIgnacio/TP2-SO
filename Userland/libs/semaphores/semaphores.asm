GLOBAL sem_open
GLOBAL sem_wait
GLOBAL sem_post
GLOBAL sem_close
GLOBAL sem_set


#define SYSCALL_SEM_OPEN 60
#define SYSCALL_SEM_WAIT 61
#define SYSCALL_SEM_POST 62
#define SYSCALL_SEM_CLOSE 63
#define SYSCALL_SEM_SET 64


SECTION .text

sem_open:
    mov rax, 0x3C
    int 0x80
    ret

sem_wait:
    mov rax, 0x3D
    int 0x80
    ret

sem_post:
    mov rax, 0x3E
    int 0x80
    ret

sem_close:
    mov rax, 0x3F
    int 0x80
    ret

sem_set:
    mov rax, 0x40
    int 0x80
    ret
