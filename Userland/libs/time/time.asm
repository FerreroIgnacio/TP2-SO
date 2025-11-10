GLOBAL getLocalTime
GLOBAL getLocalDate
GLOBAL getBootTime

SECTION .text

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
