#include "../standard.h"
// Variables globales
uint8_t * fb;
uint16_t width=0, height=0, bpp=0, pitch=0;
uint8_t newFb [100000000]; // CAMBIAR POR MALLOC

int main() {
//    return 10;
    fb = newFb;
    getVideoData(&width,&height,&bpp,&pitch);
    fbFill(fb,0x0A0105);
    fbDrawText(fb, "Bienvenido a Pongis Golf", 0xFFFFFF, 0x000000, 20, 20, 3);
    fbSet(fb);
    //while(1);
    return 10;
}
