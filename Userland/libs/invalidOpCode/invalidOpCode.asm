GLOBAL testInvalidCode

SECTION .text

testInvalidCode:
    ud2         ; Genera excepción #6 (Invalid Opcode)
    ret         