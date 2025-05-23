; idt.asm
GLOBAL IDTinit
EXTERN IDTTable

SECTION .data
idt_descriptor:
    dw 256 * 16 - 1    ; limit (256 entradas * 16 bytes - 1)
    dq IDTTable       ; base address

SECTION .text
IDTinit:
	; Limpiar toda la tabla IDT
    	mov rdi, IDTTable      ; dirección de la tabla
    	mov rcx, 256 * 2        ; 256 entradas * 16 bytes / 8 = 512 qwords
    	xor rax, rax            ; valor 0
    	rep stosq               ; limpiar con 0s
    
	; Cargar IDT
	lidt [idt_descriptor]
	sti
   	ret
