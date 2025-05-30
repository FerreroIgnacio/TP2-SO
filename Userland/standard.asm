GLOBAL syscall_write
GLOBAL syscall_read
GLOBAL syscall_isKeyDown
GLOBAL getRegisters
GLOBAL getBootTime
GLOBAL getLocalTime 
GLOBAL getLocalDate
GLOBAL getVideoData
GLOBAL putPixel
GLOBAL fbSet
GLOBAL fbSetRegion

EXTERN incFramesCount

SECTION .text


syscall_write:
    mov rax, 1              ; syscall ID para write
    int 0x80
    ret
syscall_read:
    mov rax, 0              ; syscall ID para read
    int 0x80
    ret
syscall_isKeyDown:
	mov rax, 2
	int 0x80
	ret
    
getRegisters:               ; rdi contiene el puntero donde guardar los registros
                            ; rip, rflags, rsp, rbp, rax, rbx, rcx, rdx, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15
    
    mov [rdi + 32], rax     ; guardo primero rax para poder usarlo

    call getRip             ; pushea el instruction pointer a la pila
getRip:
    pop rax                 ; 
    mov [rdi], rax          ; rip
    pushfq
    pop rax
    mov [rdi + 8], rax      ; rflags
    mov [rdi + 16], rsp     ; rsp
    mov [rdi + 24], rbp     ; rbp
    mov [rdi + 40], rbx     ; rbx
    mov [rdi + 48], rcx     ; rcx
    mov [rdi + 56], rdx     ; rdx
    mov [rdi + 64], rsi     ; rsi
    mov [rdi + 72], rdi     ; rdi
    mov [rdi + 80], r8      ; r8
    mov [rdi + 88], r9      ; r9
    mov [rdi + 96], r10     ; r10
    mov [rdi + 104], r11    ; r11
    mov [rdi + 112], r12    ; r12
    mov [rdi + 120], r13    ; r13
    mov [rdi + 128], r14    ; r14
    mov [rdi + 136], r15    ; r15

    ret
getBootTime:
    mov     rax, 0x3        ; ID de syscall para bootTime (3)
    int     0x80
    ret

getLocalTime:
    mov     rax, 0x4        ; ID de syscall para localTime (4)
    int     0x80
    ret

getLocalDate:
    mov     rax, 0x5        ; ID de syscall para localDate (5)
    int     0x80
    ret

getVideoData:
    mov     rax, 0x9
    int     0x80
    ret

putPixel:
    mov     rax, 0xA      ; syscall número 10
    int     0x80          ; llamar a syscall
    ret

fbSet:
    mov     rax, 0xB
    int     0x80
    call    incFramesCount
    ret

;fbSetRegion(uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height, uint8_t* bmp, uint32_t maskColor)
fbSetRegion:
   mov      rax, 0xC        ; syscall ID para set framebuffer region             
   int      0x80            ; rdi=topLeftX, rsi=topLeftY, rdx=width, rcx=height, r8=bmp, r9=maskColor
   ret	
