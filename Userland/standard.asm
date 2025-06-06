GLOBAL write
GLOBAL read
GLOBAL isKeyPressed
GLOBAL getBootTime
GLOBAL getLocalTime 
GLOBAL getLocalDate
GLOBAL getRegisters
GLOBAL getVideoData
GLOBAL putPixel
GLOBAL fbSet
GLOBAL fbSetRegion

EXTERN incFramesCount

SECTION .text


write:
    mov rax, 1              ; syscall ID para write
    int 0x80
    ret
read:
    mov rax, 0              ; syscall ID para read
    int 0x80
    ret
isKeyPressed:
	mov rax, 2
	int 0x80
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

getRegisters:
    mov     rax, 0x6        ; ID de syscall para  getRegisters (7)
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
