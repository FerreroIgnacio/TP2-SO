GLOBAL write
GLOBAL read
GLOBAL isKeyPressed
GLOBAL getBootTime
GLOBAL getLocalTime 
GLOBAL getLocalDate
GLOBAL getRegisters
GLOBAL putPixel
GLOBAL fbSet
GLOBAL startSound
GLOBAL stopSound
GLOBAL malloc
GLOBAL calloc
GLOBAL realloc
GLOBAL free
GLOBAL proc_spawn
GLOBAL proc_kill
GLOBAL proc_list
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

putPixel:
    mov     rax, 0xA      ; syscall número 10
    int     0x80          ; llamar a syscall
    ret

fbSet:
    mov     rax, 0xB
    int     0x80
    ret

startSound:
    mov     rax, 0x14     ; syscall número 20
    int     0x80          ; llamar a syscall
    ret

stopSound:
    mov     rax, 0x15     ; syscall número 21
    int     0x80          ; llamar a syscall
    ret

malloc:
    mov     rax, 0x1E     ; syscall número 30
    int     0x80
    ret

calloc:
    mov     rax, 0x1F     ; syscall número 31
    int     0x80
    ret

realloc:
    mov     rax, 0x20     ; syscall número 32
    int     0x80
    ret

free:
    mov     rax, 0x21     ; syscall número 33
    int     0x80
    ret

; proc management: IDs 40,41,42
; int proc_spawn(task_fn_t entry)
proc_spawn:
    mov     rax, 0x28
    int     0x80
    ret
; int proc_kill(int pid)
proc_kill:
    mov     rax, 0x29
    int     0x80
    ret
; int proc_list(proc_info_t* out, int max)
proc_list:
    mov     rax, 0x2A
    int     0x80
    ret
