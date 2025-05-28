#include "../standard.h"
// Variables globales
uint8_t * fb;
uint16_t width=0, height=0, bpp=0, pitch=0;
uint8_t newFb [100000000]; // CAMBIAR POR MALLOC

int main() {
    return 10;
    fb = newFb;
    getVideoData(&width,&height,&bpp,&pitch);
    fbDrawText(fb, "Bienvenido a Pongis Golf", 0xFFFFFF, 0x000000, 20, 20, 3);
    fbSet(fb);
    for (int y = 0; y < 500; y++) {
        for (int x = 0; x < 500; x++) {
            putPixel(0x00AA00, x, y);
        }
    }
    //while(1);
    return 10;
}