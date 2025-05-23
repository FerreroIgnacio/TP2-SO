section .text
global keyboard_interrupt_handler

extern keyPressedAction


keyboard_interrupt_handler:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11

    in al, 0x60             ; Leer scancode del puerto del teclado
    movzx rdi, al           ; Pasar scancode como primer argumento (RDI)

    ; Enviar EOI al PIC maestro
    mov al, 0x20
    out 0x20, al

    ; Si usas PIC esclavo (IRQ > 7), necesitarías también:
    ; out 0xA0, al

    call keyPressedAction

    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    iretq
