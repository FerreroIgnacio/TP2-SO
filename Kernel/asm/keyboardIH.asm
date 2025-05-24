section .text
global keyboard_interrupt_handler
extern keyPressedAction

keyboard_interrupt_handler:
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
