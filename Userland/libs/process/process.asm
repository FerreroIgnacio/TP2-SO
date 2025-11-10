GLOBAL proc_spawn
GLOBAL proc_kill
GLOBAL proc_list

SECTION .text


proc_spawn : mov rax,
    0x28 int 0x80 
    ret;

proc_kill : mov rax,
    0x29 int 0x80 
    ret;
    
proc_list : mov rax,
    0x2A int 0x80 
    ret