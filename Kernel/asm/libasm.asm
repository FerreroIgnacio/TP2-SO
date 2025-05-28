GLOBAL cpuVendor
GLOBAL sys_getTime
GLOBAL sys_getDate

section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

read_rtc:			; en al se recibe el registro a leer y se escribe la respuesta
	push rdx
    mov dx, 0x70
    out dx, al      ; escribir registro a leer en 0x70
    mov dx, 0x71    
    in al, dx       ; leer dato del RTC
	pop rdx
    ret

bcd_to_bin:			; en el rtc se guardan los valores en bcd, hay que pasarlos a decimal 
    mov ah, al        ; copiar AL en AH
    and al, 0x0F      ; AL = unidades
    shr ah, 4         ; AH = decenas
    mov bl, ah        ; copiar decenas a BL
    shl ah, 3         ; AH = decenas * 8
    shl bl, 1         ; BL = decenas * 2  
    add ah, bl        ; AH = decenas * 10
    add al, ah        ; AL = unidades + decenas*10
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