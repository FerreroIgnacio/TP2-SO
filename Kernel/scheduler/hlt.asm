GLOBAL cpu_halt

section .text
; Halt CPU until next interrupt and return
cpu_halt:
	hlt
	ret
