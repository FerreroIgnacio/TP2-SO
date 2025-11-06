GLOBAL sys_getTime
GLOBAL sys_getDate
GLOBAL saveRegisters
GLOBAL sys_getRegisters
GLOBAL sys_playSound
GLOBAL sys_stopSound


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


; copia a saved_registers los 18 valores pasados por rdi
saveRegisters:
    push rsi
    push rax
    push rbx

    mov rsi, saved_registers   
    mov rbx, 18                  
saveregs_loop:
    mov rax, [rdi]               
    mov [rsi], rax               
    add rdi, 8                   
    add rsi, 8
    dec rbx
    jnz saveregs_loop

    pop rbx
    pop rax
    pop rsi

    ret
; retorna los 18 registros guardados en saved_registers
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

; para audio se usan 3 puertos en e/s:
; 0x42: divisor: la frecuencia en el speaker se obtiene al dividir la frecuencia base (1.193.180 Hz) por el valor en este registro
; 0x43: configura el pit, guardando 0xB6, se configura para: el canal 2 (speaker) - enviar primero la parte baja y luego la alta
;       del divisor (por 0x42) - generar una onda cuadrada - valores en binario (no BCD)
; 0x61: 

sys_playSound:              ; en rdi ya está la freq deseada

    cmp rdi, 18             ; Verificar rango válido (18 Hz - 20000 Hz)
    jb  end
    cmp rdi, 20000
    ja  end

    push rax
    push rdx

    mov al, 0xB6            ; config pit
    out 0x43, al

    mov rax, 1193180        ; freq del pit
    xor rdx, rdx            ; limpiar rdx (dividir solo con lo que está en rax)
    div rdi                 ; obtener divisor a enviar al pit
    out 0x42, al            ; enviar LSB
    mov al, ah              ; AL = MSB
    out 0x42, al            ; enviar MSB

    in  al, 0x61            ; poner los bits 0 y 1 en 1. (conectar en canal 2 al speaker y encenderlo)
    or  al, 0x03
    out 0x61, al

    pop rdx
    pop rax
end:   
    ret 

sys_stopSound:
    in al, 0x61
    and al, 0xFC            ;poner los bits 0 y 1 en 0. (apagar el speaker)
    out 0x61, al
    ret

section .bss
saved_registers: resq 18
