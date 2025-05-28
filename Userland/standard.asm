GLOBAL putPixel
GLOBAL getVideoData
GLOBAL fbSet
global syscall_write
global syscall_read
global syscall_isKeyDown
global syscall_time
global fbSetRegion
SECTION .text


;fbSetRegion(uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height, uint8_t* bmp, uint32_t maskColor)
fbSetRegion:
   mov rax, 12         ; syscall ID para set framebuffer region
   ; rdi=topLeftX, rsi=topLeftY, rdx=width, rcx=height, r8=bmp, r9=maskColor
   int 0x80
   ret	

; int syscall_write(int fd, const char *buf, unsigned long count)
syscall_write:
    mov rax, 1          ; syscall ID para write
    ; rdi, rsi, rdx ya tienen fd, buf, count por convención de llamada
    int 0x80
    ret

; int syscall_read(int fd, char *buf, unsigned long count)
syscall_read:
    mov rax, 0          ; syscall ID para read
    ; rdi, rsi, rdx ya tienen fd, buf, count por convención de llamada
    int 0x80
    ret
syscall_isKeyDown:
	mov rax, 2
	int 0x80
	ret
syscall_time:
    mov rax, 3          ; ID de syscall para time (3)
    int 0x80
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
    ret


