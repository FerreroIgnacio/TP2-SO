// Opción 1: Mapeo directo de caracteres Unicode a patrones bitmap personalizados
// Agregar esto a standard.h

#ifndef UNICODE_SUPPORT_H
#define UNICODE_SUPPORT_H

// Definir los caracteres Unicode específicos que necesitas para Among Us
// Estos son los códigos de los caracteres de bloque que estás usando
#define UNICODE_FULL_BLOCK      0x2588  // █ (no usado en tu art)
#define UNICODE_UPPER_HALF      0x2580  // ▀ 
#define UNICODE_LOWER_HALF      0x2584  // ▄
#define UNICODE_LEFT_HALF       0x258C  // ▌
#define UNICODE_RIGHT_HALF      0x2590  // ▐
#define UNICODE_LIGHT_SHADE     0x2591  // ░
#define UNICODE_MEDIUM_SHADE    0x2592  // ▒
#define UNICODE_DARK_SHADE      0x2593  // ▓

// Patrones bitmap 8x8 para los caracteres de bloque de Braille que usas
// Estos son los patrones reales de tus caracteres ⠀⢀⣀ etc.

// ⠀ (U+2800) - espacio Braille vacío
static char braille_2800[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// ⢀ (U+2880) 
static char braille_2880[8] = {0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00};

// ⣀ (U+28C0)
static char braille_28C0[8] = {0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00};

// ⣤ (U+28E4)
static char braille_28E4[8] = {0x00, 0x60, 0x60, 0x00, 0xC0, 0xC0, 0x00, 0x00};

// ⣼ (U+28FC)
static char braille_28FC[8] = {0x00, 0x7C, 0x7C, 0x00, 0xC0, 0xC0, 0x00, 0x00};

// ⠟ (U+281F)
static char braille_281F[8] = {0x1F, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// ⠉ (U+2809)
static char braille_2809[8] = {0x09, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// ⠙ (U+2819)
static char braille_2819[8] = {0x19, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// ⠻ (U+283B)
static char braille_283B[8] = {0x3B, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// ⢶ (U+28B6)
static char braille_28B6[8] = {0x00, 0x36, 0x36, 0x00, 0x80, 0x80, 0x00, 0x00};

// ⣄ (U+28C4)
static char braille_28C4[8] = {0x00, 0x04, 0x04, 0x00, 0xC0, 0xC0, 0x00, 0x00};

// ⣾ (U+28FE)
static char braille_28FE[8] = {0x00, 0x7E, 0x7E, 0x00, 0xC0, 0xC0, 0x00, 0x00};

// ⡏ (U+284F)
static char braille_284F[8] = {0x0F, 0x0F, 0x00, 0x40, 0x40, 0x40, 0x00, 0x00};


// ⣶ (U+28F6)
static char braille_28F6[8] = {0x76, 0x76, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00};

// ⣿ (U+28FF)
static char braille_28FF[8] = {0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00};

// ⢠ (U+28A0)
static char braille_28A0[8] = {0x20, 0x20, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00};

// ⢤ (U+28A4)
static char braille_28A4[8] = {0x24, 0x24, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00};

// ⣠ (U+28E0)
static char braille_28E0[8] = {0x60, 0x60, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00};

// ⣴ (U+28F4)
static char braille_28F4[8] = {0x74, 0x74, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x00};

// ⢧ (U+28A7)
static char braille_28A7[8] = {0x27, 0x27, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00};


// ⠀ y otros caracteres... (necesitarías definir todos los que usas)

// Estructura para mapeo de caracteres Unicode
typedef struct {
    uint32_t unicode;
    char* bitmap;
} UnicodeChar;

// Tabla de mapeo
static UnicodeChar unicode_map[] = {
    {0x2800, braille_2800},  // ⠀
    {0x2880, braille_2880},  // ⢀
    {0x28C0, braille_28C0},  // ⣀
    {0x28E4, braille_28E4},  // ⣤
    {0x28FC, braille_28FC},  // ⣼
    {0x281F, braille_281F},  // ⠟
    {0x2809, braille_2809},  // ⠉
    {0x2819, braille_2819},  // ⠙
    {0x283B, braille_283B},  // ⠻
    {0x28B6, braille_28B6},  // ⢶
    {0x28C4, braille_28C4},  // ⣄
    {0x28FE, braille_28FE},  // ⣾
    {0x284F, braille_284F},  // (ups! lo borre y no se cual iba :'c, perdon)
    {0x28F6, braille_28F6},  // ⣶
{0x28FF, braille_28FF},  // ⣿
{0x28A0, braille_28A0},  // ⢠
{0x28A4, braille_28A4},  // ⢤
{0x28E0, braille_28E0},  // ⣠
{0x28F4, braille_28F4},  // ⣴
{0x28A7, braille_28A7},  // ⢧
    // ... agregar más según necesites
   {0, 0}  // Terminador
};

// Función para obtener bitmap de carácter Unicode
char* getUnicodeBitmap(uint32_t unicode) {
    for (int i = 0; unicode_map[i].bitmap != 0; i++) {
        if (unicode_map[i].unicode == unicode) {
            return unicode_map[i].bitmap;
        }
    }
    return 0;  // Carácter no encontrado
}

// Función mejorada para dibujar caracteres con soporte Unicode
void drawCharExtended(uint32_t unicode, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size) {
    char* bmp = 0;
    
    // Si es ASCII básico, usar la tabla existente
    if (unicode < 128) {
        extern char font8x8_basic[128][8];
        bmp = font8x8_basic[unicode];
    } else {
        // Buscar en tabla Unicode
        bmp = getUnicodeBitmap(unicode);
        if (bmp == 0) {
            // Si no se encuentra, dibujar un rectángulo como placeholder
            for (int j = 0; j < 64; j++) {
                int fil = j / 8;
                int col = j % 8;
                int color = (fil == 0 || fil == 7 || col == 0 || col == 7) ? hexColor : backColor;
                
                for (int dx = 0; dx < size; dx++) {
                    for (int dy = 0; dy < size; dy++) {
                        putPixel(color, x + col * size + dx, y + fil * size + dy);
                    }
                }
            }
            return;
        }
    }
    
    // Dibujar el carácter
    for(int j = 0; j < 64; j++){
        int fil = j / 8;
        int col = j % 8;
        int isOn = bmp[fil] & (1 << col);
        int color = isOn ? hexColor : backColor;

        for (int dx = 0; dx < size; dx++) {
            for (int dy = 0; dy < size; dy++) {
                putPixel(color, x + col * size + dx, y + fil * size + dy);
            }
        }
    }
}

// Función para parsear string UTF-8 y extraer códigos Unicode
uint32_t parseUTF8(const char** str) {
    const unsigned char* s = (const unsigned char*)*str;
    uint32_t unicode = 0;
    
    if (s[0] < 0x80) {
        // ASCII de 1 byte
        unicode = s[0];
        *str += 1;
    } else if ((s[0] & 0xE0) == 0xC0) {
        // UTF-8 de 2 bytes
        unicode = ((s[0] & 0x1F) << 6) | (s[1] & 0x3F);
        *str += 2;
    } else if ((s[0] & 0xF0) == 0xE0) {
        // UTF-8 de 3 bytes (aquí están tus caracteres Braille)
        unicode = ((s[0] & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        *str += 3;
    } else if ((s[0] & 0xF8) == 0xF0) {
        // UTF-8 de 4 bytes
        unicode = ((s[0] & 0x07) << 18) | ((s[1] & 0x3F) << 12) | 
                  ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
        *str += 4;
    } else {
        // Carácter inválido
        unicode = '?';
        *str += 1;
    }
    
    return unicode;
}

// Función mejorada para dibujar texto con soporte Unicode
void drawTextUnicode(const char* str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size) {
    uint64_t current_x = x;
    
    while (*str != 0) {
        if (*str == '\n') {
            current_x = x;
            y += 8 * size;
            str++;
            continue;
        }
        
        uint32_t unicode = parseUTF8(&str);
        drawCharExtended(unicode, hexColor, backColor, current_x, y, size);
        current_x += 8 * size;
    }
}

#endif // UNICODE_SUPPORT_H
