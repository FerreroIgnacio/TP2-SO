GLOBAL irq01Handler
EXTERN keyPressedAction


irq01Handler:			; Keyboard irq
	push rax
	push rdi
	
	in al,0x60
	movzx rdi,al;
	
	call keyPressedAction

	mov al, 0x20
	out 0x20, al
	
	pop rdi
	pop rax
	iretq

