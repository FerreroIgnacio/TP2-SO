
GLOBAL testFn

section .data
    msg db 'hola', 0    ; String "hola" con null terminator
    msg_len equ 4       ; Longitud sin contar el null terminator

section .text
testFn:
    ; Make a syscall using interrupt 0x80 (common in x64BareBones)
    ; Syscall convention for your kernel:
    ; rax = syscall number (1 = SYSCALL_WRITE)
    ; rdi = arg1 (fd = 2 for STDERR)
    ; rsi = arg2 (buffer = msg)
    ; rdx = arg3 (count = 4)
    
    mov rax, 1          ; syscall number for SYSCALL_WRITE
    mov rdi, 2          ; fd = 2 (STDERR)
    mov rsi, msg        ; buffer = "hola"
    mov rdx, msg_len    ; count = 4
    
    int 0x80            ; trigger interrupt (adjust this if your kernel uses different interrupt)
    
    ret                 ; return (resultado en rax)
