GLOBAL cpu_halt
GLOBAL cpu_relax

section .text
; Halt CPU until next interrupt and return
cpu_halt:
	hlt
	ret

; Relax for spin-wait loops (PAUSE instruction)
cpu_relax:
	pause
	ret
