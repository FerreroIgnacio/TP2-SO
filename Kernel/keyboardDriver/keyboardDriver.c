#include "keyboardDriver.h"
#include <stdint.h>
#include "../IDT/idtInit.h"
#include "../videoDriver/videoDriver.h"
#include "../syscalls/syscalls.h"
#include "../filesDescriptors/fd.h"
#include "../filesDescriptors/pipes.h"

#define KEYCODE_BUFFER_SIZE 128

#define CTRL_MAKECODE 0x1D
#define SHIFT_LEFT_MAKECODE 0x2A
#define SHIFT_RIGHT_MAKECODE 0x36
#define ALT_MAKECODE 0x38
#define CAPSLOCK_MAKECODE 0x3A
#define ALTGR_MAKECODE 0x38      // (E0) extendido
#define CTRL_RIGHT_MAKECODE 0x1D // (E0) extendido
#define F1_MAKECODE 0x3B

typedef struct registers
{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
    uint64_t rflags;
    uint64_t rip;
} registers_t;

// Reemplaza las tablas en standard.c con estas versiones corregidas para layout argentino
// Make code to ASCII mapping for Set 1 makecodes - Layout Argentino
static char makeCodeToAscii[128] = {
    0,                            // 0x00 - unused
    '\e',                         // 0x01 - ESC
    '1',                          // 0x02
    '2',                          // 0x03
    '3',                          // 0x04
    '4',                          // 0x05
    '5',                          // 0x06
    '6',                          // 0x07
    '7',                          // 0x08
    '8',                          // 0x09
    '9',                          // 0x0A
    '0',                          // 0x0B
    '\'',                         // 0x0C - apostrofe (en lugar de -)
    191,                          // 0x0D - ¿ (en lugar de =)
    '\b',                         // 0x0E - Backspace
    '\t',                         // 0x0F - Tab
    'q',                          // 0x10
    'w',                          // 0x11
    'e',                          // 0x12
    'r',                          // 0x13
    't',                          // 0x14
    'y',                          // 0x15
    'u',                          // 0x16
    'i',                          // 0x17
    'o',                          // 0x18
    'p',                          // 0x19
    0,                            // 0x1A - ´ (dead key, puede ser problemático)
    '+',                          // 0x1B - +
    '\n',                         // 0x1C - Enter
    0,                            // 0x1D - Left Ctrl
    'a',                          // 0x1E
    's',                          // 0x1F
    'd',                          // 0x20
    'f',                          // 0x21
    'g',                          // 0x22
    'h',                          // 0x23
    'j',                          // 0x24
    'k',                          // 0x25
    'l',                          // 0x26
    241,                          // 0x27 - ñ
    '{',                          // 0x28 - { (en algunos layouts argentinos)
    '|',                          // 0x29 - |
    0,                            // 0x2A - Left Shift
    '}',                          // 0x2B - }
    'z',                          // 0x2C
    'x',                          // 0x2D
    'c',                          // 0x2E
    'v',                          // 0x2F
    'b',                          // 0x30
    'n',                          // 0x31
    'm',                          // 0x32
    ',',                          // 0x33
    '.',                          // 0x34
    '-',                          // 0x35 - - (guión)
    0,                            // 0x36 - Right Shift
    '*',                          // 0x37 - Keypad *
    0,                            // 0x38 - Left Alt
    ' ',                          // 0x39 - Space
    0,                            // 0x3A - Caps Lock
    0,                            // 0x3B - F1
    0,                            // 0x3C - F2
    0,                            // 0x3D - F3
    0,                            // 0x3E - F4
    0,                            // 0x3F - F5
    0,                            // 0x40 - F6
    0,                            // 0x41 - F7
    0,                            // 0x42 - F8
    0,                            // 0x43 - F9
    0,                            // 0x44 - F10
    0,                            // 0x45 - Num Lock
    0,                            // 0x46 - Scroll Lock
    '7',                          // 0x47 - Keypad 7
    '8',                          // 0x48 - Keypad 8
    '9',                          // 0x49 - Keypad 9
    '-',                          // 0x4A - Keypad -
    '4',                          // 0x4B - Keypad 4
    '5',                          // 0x4C - Keypad 5
    '6',                          // 0x4D - Keypad 6
    '+',                          // 0x4E - Keypad +
    '1',                          // 0x4F - Keypad 1
    '2',                          // 0x50 - Keypad 2
    '3',                          // 0x51 - Keypad 3
    '0',                          // 0x52 - Keypad 0
    '.',                          // 0x53 - Keypad .
    0,                            // 0x54
    0,                            // 0x55
    '<',                          // 0x56 '<'
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x54-0x5D unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x5E-0x67 unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x68-0x71 unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x72-0x7B unused
    0                             // 0x7C-0x7F unused
};
// SHIFT + MAKECODE
static char makeCodeToAscii_shift[128] = {
    0,    // 0x00 - unused
    '\e', // 0x01 - ESC
    '!',  // 0x02 - Shift+1
    '"',  // 0x03 - Shift+2
    '#',  // 0x04 - Shift+3
    '$',  // 0x05 - Shift+4
    '%',  // 0x06 - Shift+5
    '&',  // 0x07 - Shift+6
    '/',  // 0x08 - Shift+7
    '(',  // 0x09 - Shift+8
    ')',  // 0x0A - Shift+9
    '=',  // 0x0B - Shift+0
    '?',  // 0x0C - Shift+' (apostrofe)
    161,  // 0x0D - Shift+¿ = ¡
    '\b', // 0x0E - Backspace
    '\t', // 0x0F - Tab
    'Q',  // 0x10
    'W',  // 0x11
    'E',  // 0x12
    'R',  // 0x13
    'T',  // 0x14
    'Y',  // 0x15
    'U',  // 0x16
    'I',  // 0x17
    'O',  // 0x18
    'P',  // 0x19
    0,    // 0x1A - Shift+´ (dead key)
    '*',  // 0x1B - Shift++
    '\n', // 0x1C - Enter
    0,    // 0x1D - Left Ctrl
    'A',  // 0x1E
    'S',  // 0x1F
    'D',  // 0x20
    'F',  // 0x21
    'G',  // 0x22
    'H',  // 0x23
    'J',  // 0x24
    'K',  // 0x25
    'L',  // 0x26
    209,  // 0x27 - Shift+ñ
    '[',  // 0x28 - Shift+{
    176,  // 0x29 - Shift+| = °
    0,    // 0x2A - Left Shift
    ']',  // 0x2B - Shift+}
    'Z',  // 0x2C
    'X',  // 0x2D
    'C',  // 0x2E
    'V',  // 0x2F
    'B',  // 0x30
    'N',  // 0x31
    'M',  // 0x32
    ';',  // 0x33 - Shift+,
    ':',  // 0x34 - Shift+.
    '_',  // 0x35 - Shift+-
    0,    // 0x36 - Right Shift
    '*',  // 0x37 - Keypad *
    0,    // 0x38 - Left Alt
    ' ',  // 0x39 - Space
    0,    // 0x3A - Caps Lock
    0,    // 0x3B - F1
    0,    // 0x3C - F2
    0,    // 0x3D - F3
    0,    // 0x3E - F4
    0,    // 0x3F - F5
    0,    // 0x40 - F6
    0,    // 0x41 - F7
    0,    // 0x42 - F8
    0,    // 0x43 - F9
    0,    // 0x44 - F10
    0,    // 0x45 - Num Lock
    0,    // 0x46 - Scroll Lock
    '7',  // 0x47 - Keypad 7
    '8',  // 0x48 - Keypad 8
    '9',  // 0x49 - Keypad 9
    '-',  // 0x4A - Keypad -
    '4',  // 0x4B - Keypad 4
    '5',  // 0x4C - Keypad 5
    '6',  // 0x4D - Keypad 6
    '+',  // 0x4E - Keypad +
    '1',  // 0x4F - Keypad 1
    '2',  // 0x50 - Keypad 2
    '3',  // 0x51 - Keypad 3
    '0',  // 0x52 - Keypad 0
    '.',  // 0x53 - Keypad .i
    0,    // 0x54
    0,    // 0x55
    '>',  // 0x56 '>'
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, // 0x54-0x5D unused
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, // 0x5E-0x67 unused
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, // 0x68-0x71 unused
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, // 0x72-0x7B unused
    0, // 0x7C-0x7F unused
};
// ALT GR + MAKECODE
static char makeCodeToAscii_altgr[128] = {
    0,                            // 0x00 - unused
    '\e',                         // 0x01 - ESC
    '|',                          // 0x02
    '@',                          // 0x03
    249,                          // 0x04
    '~',                          // 0x05
    128,                          // 0x06 '€'
    172,                          // 0x07
    '7',                          // 0x08
    '8',                          // 0x09
    '9',                          // 0x0A
    '0',                          // 0x0B
    '\\',                         // 0x0C - apostrofe (en lugar de -)
    191,                          // 0x0D - ¿ (en lugar de =)
    '\b',                         // 0x0E - Backspace
    '\t',                         // 0x0F - Tab
    '@',                          // 0x10
    'w',                          // 0x11
    128,                          // 0x12
    'r',                          // 0x13
    't',                          // 0x14
    'y',                          // 0x15
    'u',                          // 0x16
    'i',                          // 0x17
    'o',                          // 0x18
    'p',                          // 0x19
    0,                            // 0x1A - ´ (dead key, puede ser problemático)
    '~',                          // 0x1B - +
    '\n',                         // 0x1C - Enter
    0,                            // 0x1D - Left Ctrl
    'a',                          // 0x1E
    's',                          // 0x1F
    'd',                          // 0x20
    'f',                          // 0x21
    'g',                          // 0x22
    'h',                          // 0x23
    'j',                          // 0x24
    'k',                          // 0x25
    'l',                          // 0x26
    241,                          // 0x27 - ñ
    '^',                          // 0x28 - { (en algunos layouts argentinos)
    0,                            // 0x29 - |
    0,                            // 0x2A - Left Shift
    0,                            // 0x2B - }
    'z',                          // 0x2C
    'x',                          // 0x2D
    'c',                          // 0x2E
    'v',                          // 0x2F
    'b',                          // 0x30
    'n',                          // 0x31
    'm',                          // 0x32
    ',',                          // 0x33
    249,                          // 0x34
    '_',                          // 0x35 - - (guión)
    0,                            // 0x36 - Right Shift
    '*',                          // 0x37 - Keypad *
    0,                            // 0x38 - Left Alt
    ' ',                          // 0x39 - Space
    0,                            // 0x3A - Caps Lock
    0,                            // 0x3B - F1
    0,                            // 0x3C - F2
    0,                            // 0x3D - F3
    0,                            // 0x3E - F4
    0,                            // 0x3F - F5
    0,                            // 0x40 - F6
    0,                            // 0x41 - F7
    0,                            // 0x42 - F8
    0,                            // 0x43 - F9
    0,                            // 0x44 - F10
    0,                            // 0x45 - Num Lock
    0,                            // 0x46 - Scroll Lock
    '7',                          // 0x47 - Keypad 7
    '8',                          // 0x48 - Keypad 8
    '9',                          // 0x49 - Keypad 9
    '-',                          // 0x4A - Keypad -
    '4',                          // 0x4B - Keypad 4
    '5',                          // 0x4C - Keypad 5
    '6',                          // 0x4D - Keypad 6
    '+',                          // 0x4E - Keypad +
    '1',                          // 0x4F - Keypad 1
    '2',                          // 0x50 - Keypad 2
    '3',                          // 0x51 - Keypad 3
    '0',                          // 0x52 - Keypad 0
    '.',                          // 0x53 - Keypad .
    0,                            // 0x54
    0,                            // 0x55
    '|',                          // 0x56 '<'
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x54-0x5D unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x5E-0x67 unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x68-0x71 unused
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0x72-0x7B unused
    0                             // 0x7C-0x7F unused
};

// Vector de teclas presionadas
static char keysPressed[KEYCODE_BUFFER_SIZE * 2] = {0};
int extended = 0;

// Handler de la interrupción irq01

static int shiftPressed = 0;
static int ctrlPressed = 0;
static int altPressed = 0;
static int altgrPressed = 0;
static int capsLockOn = 0;

static void updateSpecialKeys()
{
    shiftPressed = (keysPressed[SHIFT_LEFT_MAKECODE] || keysPressed[SHIFT_RIGHT_MAKECODE]) ? 1 : 0;
    ctrlPressed = (keysPressed[CTRL_MAKECODE] || keysPressed[CTRL_RIGHT_MAKECODE + KEYCODE_BUFFER_SIZE]) ? 1 : 0;
    altPressed = keysPressed[ALT_MAKECODE];
    altgrPressed = keysPressed[ALTGR_MAKECODE + KEYCODE_BUFFER_SIZE];
}

static char getAscii(uint8_t makecode)
{
    if (makecode >= 128)
        return 0;
    if (altgrPressed)
        return makeCodeToAscii_altgr[makecode];
    if (shiftPressed && !capsLockOn)
        return makeCodeToAscii_shift[makecode];
    if (!shiftPressed && capsLockOn)
        return makeCodeToAscii_shift[makecode];
    return makeCodeToAscii[makecode];
}

void keyPressedAction(uint8_t makecode, registers_t *regs)
{

    if (makecode == 0xE0)
    {
        extended++;
        return;
    }

    uint8_t keycode = makecode & 0x7F;

    if (extended)
    { // makecode extendido (E0)
        if (makecode & 0x80)
        {
            keysPressed[keycode + KEYCODE_BUFFER_SIZE] = 0;
            updateSpecialKeys();
        }
        else
        {
            keysPressed[keycode + KEYCODE_BUFFER_SIZE] = 1;
            updateSpecialKeys();
        }
        extended--;
    }
    else
    {
        if (makecode & 0x80)
        { // makecode normal (sin E0)
            keysPressed[keycode] = 0;
            updateSpecialKeys();
        }
        else
        {
            keysPressed[keycode] = 1;
            updateSpecialKeys();

            // KeyCombo save registers
            if (makecode == F1_MAKECODE)
            {
                saveRegisters((uint64_t *)regs); // guarda la copia de los registros que se recibió con la interrupción
                return;
            }

            if (keycode == CAPSLOCK_MAKECODE)
            {
                capsLockOn = !capsLockOn;
            }
            char c = getAscii(makecode);
            if (c != 0) {
                // NO usar pipe_write (bloqueante) dentro de la IRQ: causa bloqueo y no despierta la shell.
                // Insertar non-blocking; read() seguirá siendo bloqueante y despertará al tener datos.
                int r = pipe_try_kernel_nonblocking_write(0, c);
                if (r != 1) {
                    // opcional: drop si lleno; nunca bloquear aquí.
                    // printf("[DBG] teclado: pipe llena, char descartado\n");
                }
            }
        }
    }
}
int areKeysPressed(uint16_t *makeCodeVec)
{
    if (makeCodeVec == 0)
    {
        return 0;
    }
    for (int i = 0; makeCodeVec[i] != 0; i++)
    {
        if (!isKeyPressed(makeCodeVec[i]))
        {
            return 0;
        }
    }
    return 1;
}
int isKeyPressed(uint16_t makeCode)
{
    if (makeCode & 0xE000)
    { // Si se recibió un makecode especial
        return keysPressed[(makeCode & 0x7F) + KEYCODE_BUFFER_SIZE];
    }
    return keysPressed[makeCode & 0x7F];
}
void setKeyPressed(uint8_t makeCode)
{
    keysPressed[makeCode] = 1;
}
void setKeyReleased(uint8_t makeCode)
{
    keysPressed[makeCode] = 0;
}
