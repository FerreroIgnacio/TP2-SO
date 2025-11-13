GLOBAL new_proc
GLOBAL exit
GLOBAL getpid
GLOBAL get_proc_list
GLOBAL kill
GLOBAL get_priority
GLOBAL set_priority 
GLOBAL block_proc
GLOBAL unblock_proc
GLOBAL yield
GLOBAL waitpid
GLOBAL sleep


SECTION .text

new_proc:
    mov rax, 0x28
    int 0x80 
    ret;

exit:
    mov rax, 0x29
    int 0x80 
    ret;

getpid:
    mov rax, 0x2A
    int 0x80 
    ret;

get_proc_list:
    mov rax, 0x2B
    int 0x80 
    ret;

kill:
    mov rax, 0x2C
    int 0x80 
    ret;

get_priority:
    mov rax, 0x2D
    int 0x80 
    ret;

set_priority:
    mov rax, 0x2E
    int 0x80 
    ret;

block_proc:
    mov rax, 0x2F
    int 0x80 
    ret;

unblock_proc:
    mov rax, 0x30
    int 0x80 
    ret;

yield:
    mov rax, 0x31
    int 0x80 
    ret;

waitpid:
    mov rax, 0x32
    int 0x80 
    ret;

sleep:
    mov rax, 0x33
    int 0x80 
    ret;
