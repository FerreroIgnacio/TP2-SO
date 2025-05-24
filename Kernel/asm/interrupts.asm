GLOBAL irq01Handler
EXTERN keyPressedAction

section .text
irq01Handler:			; Keyboard irq
    push rax
    push rdi
    
    in al, 0x60
    movzx rdi, al
    
    mov al, 0x20
    out 0x20, al
    
    call keyPressedAction
    
    pop rdi
    pop rax
    iretq



