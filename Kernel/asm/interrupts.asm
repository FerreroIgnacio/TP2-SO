GLOBAL irq01Handler
GLOBAL irq00Handler
GLOBAL syscallInterruptHandler

GLOBAL enableTimerIRQ

EXTERN keyPressedAction
EXTERN syscallHandler
EXTERN timerTickHandler
section .text

enableTimerIRQ:
    in al, 0x21        ; Read current PIC mask
    and al, 0xFE       ; Clear bit 0 (enable IRQ0/timer) - 0xFE = 11111110
    out 0x21, al       ; Write back the mask
    ret

irq00Handler:
    push rax
    
    call timerTickHandler   
    
    mov al, 0x20
    out 0x20, al
    
    pop rax
    iretq

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

syscallInterruptHandler:

    push r8
    push r9
    push r10
    push r11
    push r12                
                        ;(syscall_id, arg1, arg2, arg3, arg4, arg5, arg6)
    push r12            ; arg6
    mov r9 , r8         ; arg5 (original r8) -> r9
    mov r8 , rcx        ; arg4 (original rcx) -> r8
    mov rcx, rdx        ; arg3 (original rdx) -> rcx
    mov rdx, rsi        ; arg2 (original rsi) -> rdx
    mov rsi, rdi        ; arg1 (original rdi) -> rsi
    mov rdi, rax        ; syscall number      -> rdi

    call syscallHandler
    add rsp, 8          

    pop r12
    pop r11
    pop r10
    pop r9
    pop r8

    iretq 