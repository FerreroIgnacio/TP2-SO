//
// Created by nacho on 6/6/2025.
//

#ifndef FONTMANAGER_H
#define FONTMANAGER_H
#include <stdint.h>
#define MAX_FONTNAME_LENGTH 255

typedef enum {                       // fuentes cargadas
    FONT_8x8_BASIC = 0,              // 8x8 monochrome bitmap font
    FONT_8x16_HUBENCHANG0515,        // 8x16 HUBENCHANG0515 font
    FONT_16x32_CONSOLE,              // 16x32 console font
    FONT_TYPE_COUNT                  
} font_type_t;

typedef struct {
    uint8_t width;                  // Char width en pixels
    uint8_t height;                 // Char height en pixels
    uint16_t char_count;            // Chars en la fuente
    uint16_t bytes_per_char;        // Bytes por cada char
    char name[MAX_FONTNAME_LENGTH]; // nombre/desc
    const void* data;               // puntero a la fuente
    int mirrored;                   
} font_info_t;

typedef struct {
    font_type_t current_font;       // fuente actual
    int currentIndex;               // indice de la fuente en el array
    font_info_t fonts[FONT_TYPE_COUNT]; 
    int initialized;               
} font_manager_t;


// External font data declarations
extern const char font8x8_basic[256][8];
extern const char font8x16_ref[];
extern const unsigned char console_font_16x32[];

//Retorna 1 si se pudo cambiar, 0 si hubo algun error (indice invalido)
int fontmanager_set_font(font_type_t font_type);

font_info_t fontmanager_get_current_font();

int fontmanager_get_current_font_index();

uint8_t fontmanager_get_font_count();

const char* fontmanager_get_font_name(font_type_t index);
#endif //FONTMANAGER_H
