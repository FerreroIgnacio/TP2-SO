GLOBAL sys_getTime
GLOBAL sys_getDate
GLOBAL sys_saveRegisters
GLOBAL sys_getRegisters

section .text

read_rtc:			; en al se recibe el registro a leer y se escribe la respuesta
	push rdx
    mov dx, 0x70
    out dx, al      ; escribir registro a leer en 0x70
    mov dx, 0x71    
    in al, dx       ; leer dato del RTC
	pop rdx
    ret

bcd_to_bin:			; en el rtc se guardan los valores en bcd, hay que pasarlos a decimal 
    mov ah, al      ; copiar AL en AH
    and al, 0x0F    ; AL = unidades
    shr ah, 4       ; AH = decenas
    mov bl, ah      ; copiar decenas a BL
    shl ah, 3       ; AH = decenas * 8
    shl bl, 1       ; BL = decenas * 2  
    add ah, bl      ; AH = decenas * 10
    add al, ah      ; AL = unidades + decenas*10
    ret

sys_getTime:
	mov al, 0x00 	; leer segundos
	call read_rtc		
	call bcd_to_bin
	mov [rdx], al   ; cargar los segundos

	mov al, 0x02 	; leer minutos
	call read_rtc	
	call bcd_to_bin	
	mov [rsi], al	; cargar los minutos

	mov al, 0x04 	; leer horas
	call read_rtc	
	call bcd_to_bin	
	mov [rdi], al	; cargar las horas

	ret

sys_getDate:
	mov al, 0x07 	; leer día
	call read_rtc		
	call bcd_to_bin
	mov [rdx], al   ; cargar dia

	mov al, 0x08 	; leer mes
	call read_rtc	
	call bcd_to_bin	
	mov [rsi], al	; cargar mes

	mov al, 0x09 	; leer año
	call read_rtc	
	call bcd_to_bin	
	mov [rdi], al	; cargar año

	ret

sys_saveRegisters:
    mov [saved_registers + 32], rax    ; rax 
    mov [saved_registers + 40], rbx    ; rbx
    mov [saved_registers + 48], rcx    ; rcx
    mov [saved_registers + 56], rdx    ; rdx
    mov [saved_registers + 64], rsi    ; rsi
    mov [saved_registers + 72], rdi    ; rdi
    mov [saved_registers + 80], r8     ; r8
    mov [saved_registers + 88], r9     ; r9
    mov [saved_registers + 96], r10    ; r10
    mov [saved_registers + 104], r11   ; r11
    mov [saved_registers + 112], r12   ; r12
    mov [saved_registers + 120], r13   ; r13
    mov [saved_registers + 128], r14   ; r14
    mov [saved_registers + 136], r15   ; r15
    mov [saved_registers + 24], rbp    ; rbp
    mov rax, rsp                       ; 
    add rax, 8                         ; sumar 8 por el return address
    mov [saved_registers + 16], rax    ; rsp original
    pushfq
    pop rax
    mov [saved_registers + 8], rax     ; rflags
    mov rax, [rsp]                     ; obtener return address
    mov [saved_registers], rax         ; rip
    ret

sys_getRegisters:
    mov rsi, saved_registers
    mov rcx, 18
bucle:
    mov rax, [rsi]
    mov [rdi], rax
    add rsi, 8
    add rdi, 8
    loop bucle
    ret

section .bss
saved_registers: resq 18

