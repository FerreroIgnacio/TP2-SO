GLOBAL getFB
GLOBAL sys_setFB
GLOBAL sys_freeFB

getFB:
    mov     rax, 0x7        
    int     0x80
    ret

sys_setFB:
    mov     rax, 0x8        
    int     0x80
    ret

sys_freeFB:
    mov     rax, 0x9        
    int     0x80
    ret