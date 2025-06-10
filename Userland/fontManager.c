//
// Created by nacho on 6/6/2025.
//

#include "fontManager.h"
#include <stdint.h>
#include "font8x8_basic.h"
#include "font8x16.h"
#include "font-16x32.h"

// Font names
static const font_info_t font_infos[FONT_TYPE_COUNT] = {
    [FONT_8x8_BASIC] = {
        .width = 8,
        .height = 8,
        .char_count = 256,
        .bytes_per_char = 8,
        .name = "8x8 Basic Font",
        .data = font8x8_basic,
        .mirrored = 0 // Not mirrored, readable format
    },
    [FONT_8x16_HUBENCHANG0515] = {
        .width = 8,
        .height = 16,
        .char_count = 256,
        .bytes_per_char = 16,
        .name = "8x16 hubenchang0515",
        .data = font8x16,
        .mirrored = 1 // Not mirrored, readable format
    },
    [FONT_16x32_CONSOLE] = {
        .width = 16,
        .height = 32,
        .char_count = 256,
        .bytes_per_char = 64,
        .name = "16x32 Console Font",
        .data = console_font_16x32,
        .mirrored = 1
    }
};

static font_manager_t g_font_manager = {
    .current_font = FONT_8x8_BASIC,
    .currentIndex = 0,
    .initialized = 0,
    .fonts = {0} // Initialize fonts array to zero
    //.fonts = font_infos,
};
// Initialize the font manager
static void fontmanager_init() {
    // Copy font_infos to g_font_manager.fonts
    for (int i = 0; i < FONT_TYPE_COUNT; i++) {
        g_font_manager.fonts[i] = font_infos[i];
    }
    g_font_manager.initialized = 1;
}

int fontmanager_set_font(font_type_t font_type) {
  if(g_font_manager.initialized == 0) {
      fontmanager_init(); // Initialize if not already done
      }
    if (!g_font_manager.initialized || font_type >= FONT_TYPE_COUNT) {
        return 0;
    }

    g_font_manager.current_font = font_type;
    return 1;
}

font_info_t fontmanager_get_current_font() {
  if(g_font_manager.initialized == 0) {
      fontmanager_init(); // Initialize if not already done
      }
    return g_font_manager.fonts[g_font_manager.current_font];
}

uint8_t fontmanager_get_font_count(){
  return FONT_TYPE_COUNT;
  }


const char* fontmanager_get_font_name(font_type_t index) {
    if (index < 0 || index >= FONT_TYPE_COUNT) {
        return "Invalid font index";
    }
    return g_font_manager.fonts[index].name;
}
int fontmanager_get_current_font_index() {
    return g_font_manager.currentIndex;
}