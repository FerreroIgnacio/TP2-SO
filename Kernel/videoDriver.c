#include <videoDriver.h>
#include <string.h>
#include "../include/font8x8/font8x8.h"

struct vbe_mode_info_structure {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint8_t * framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;
VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00;




// Copia el framebuffer pasado por parámetro al hardware
void setFramebuffer(uint8_t * fb){
    if (!fb){
        return;
    }
    uint64_t size = VBE_mode_info->pitch * VBE_mode_info->height;
    uint8_t * dest = (uint8_t *) VBE_mode_info->framebuffer;
    for (uint64_t i = 0; i < size; i++) {
        dest[i] = fb[i];
    }
}

// Copia una region de bitmap al framebuffer con mascara de color sobre los px a ignorar
void setFrameBufferRegion(uint32_t topLeftX, uint32_t topLeftY, uint32_t width, uint32_t height, uint8_t* bmp, uint32_t maskColor) {
    if (!bmp || !VBE_mode_info) {
        return;
    }
    
    uint8_t* dest = (uint8_t*)VBE_mode_info->framebuffer;
    uint32_t pitch = VBE_mode_info->pitch;
    
    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            uint64_t bmpOffset = (y * width + x) * 3;
            // Construir color RGB para comparar con máscara
	        //  R, G, B
            uint32_t pixelColor = bmp[bmpOffset] | (bmp[bmpOffset + 1] << 8) | (bmp[bmpOffset + 2] << 16);
            
            if (pixelColor != maskColor) {
                uint64_t fbOffset = (topLeftY + y) * pitch + (topLeftX + x) * 3;
                dest[fbOffset] = bmp[bmpOffset];
                dest[fbOffset + 1] = bmp[bmpOffset + 1];
                dest[fbOffset + 2] = bmp[bmpOffset + 2];
            }
        }
    }
}
uint16_t getWidth() {
    return VBE_mode_info->width;
}

uint16_t getHeight() {
    return VBE_mode_info->height;
}
uint16_t getBpp() {
    return (uint16_t)VBE_mode_info->bpp;
}

uint16_t getPitch() {
    return VBE_mode_info->pitch;
}


// Cambia el color del pixel (x,y) a hexColor</b></font>
void putPixel(uint32_t hexColor, uint64_t x, uint64_t y) {
    uint8_t * framebuffer = VBE_mode_info->framebuffer;
    uint64_t offset = (x * ((VBE_mode_info->bpp)/8)) + (y * VBE_mode_info->pitch);
    framebuffer[offset]     =  (hexColor) & 0xFF;
    framebuffer[offset+1]   =  (hexColor >> 8) & 0xFF; 
    framebuffer[offset+2]   =  (hexColor >> 16) & 0xFF;
}


// A PARTIR DE ACÁ NO NECESITARÍAMOS LAS SIGUIENTES FUNCIONES



// Escribe el string str en la posición (x,y)
void putText(char* str, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size){
	//para cada letra
	int i = 0;
	while(str[i] != 0){
		putChar(str[i], hexColor, backColor, x, y, size);
		x+= 8 * size;
		i++;
	}
}
// Escribe el char str en la posición (x,y)
 void putChar(char ascii, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size){
    if(ascii < 0 || ascii > 128) 
	    return;
	char * bmp = font8x8_basic[(unsigned char)ascii];

    for(int j = 0; j < 64; j++){
        int fil = j / 8;    // fila (0-7)
        int col = j % 8;    // columna (0-7)

        // Acceder al byte de la fila y verificar el bit de la columna
        int isOn = bmp[fil] & (1 << col);  // Sin invertir el bit
        int color = isOn ? hexColor : backColor;

        for (int dx = 0; dx < size; dx++) {
            for (int dy = 0; dy < size; dy++) {
                putPixel(color,
                        x + col * size + dx,     // X: posición + columna
                        y + fil * size + dy);    // Y: posición + fila
            }
        }
    }
 }

// Dibuja un círculo de r píxeles de radio en la posición (x,y)
void drawCircle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t r){
    uint32_t screenWidth = VBE_mode_info->width;
    uint32_t screenHeight = VBE_mode_info->height;

    for(int dx = -r; dx <= r; dx++){
        for(int dy = -r; dy <= r; dy++){
            // Verificar si el punto está dentro del círculo usando la ecuación x² + y² <= r²
            if(dx * dx + dy * dy <= r * r){
                int64_t pixelX = x + dx;
                int64_t pixelY = y + dy;
                if(pixelX >= 0 && pixelY >= 0 && pixelX < screenWidth && pixelY < screenHeight) {
                    putPixel(hexColor, pixelX, pixelY);
                }
	    }
            
	}
   }

}
// Dibuja un rectángulo de w pixeles por h pixeles en la posición (x,y)
void drawRectangle(uint32_t hexColor, uint64_t x, uint64_t y, uint64_t w, uint64_t h){
    uint32_t screenWidth = VBE_mode_info->width;
    uint32_t screenHeight = VBE_mode_info->height;

    for(uint64_t i = 0; i < h; i++){
        for(uint64_t j = 0; j < w; j++){
            uint64_t pixelX = x + j;
            uint64_t pixelY = y + i;

            // Verificar límites de pantalla
            if(pixelX < screenWidth && pixelY < screenHeight) {
                putPixel(hexColor, pixelX, pixelY);
            }
        }
    }
}
/*
 * :D
 */
void fillScreen(uint32_t hexColor){
	uint32_t screenWidth = VBE_mode_info->width;
	uint32_t screenHeight = VBE_mode_info->height;
	drawRectangle(hexColor,0,0,screenWidth,screenHeight);
}

// Dibuja un número entero en la pantalla en la posición (x,y)
void drawInt(int num, uint32_t hexColor, uint32_t backColor, uint64_t x, uint64_t y, uint64_t size) {
    char buffer[12];
    int i = 0;
    
    // Manejo numeros negativos
    if (num < 0) {
        buffer[i++] = '-';
        num = -num;
    }
    
    // Caso especial: 0
    if (num == 0) {
        buffer[i++] = '0';
    } else {
        // Extraer dígitos en orden inverso
        int start = i;
        while (num > 0) {
            buffer[i++] = (num % 10) + '0';
            num /= 10;
        }
        // Invertir solo los dígitos
        for (int j = start; j < (start + i) / 2; j++) {
            char temp = buffer[j];
            buffer[j] = buffer[i - 1 - (j - start)];
            buffer[i - 1 - (j - start)] = temp;
        }
    }
    
    buffer[i] = '\0';
    putText(buffer, hexColor, backColor, x, y, size);
}
