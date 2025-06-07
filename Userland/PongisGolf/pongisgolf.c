#include "../standard.h"
// Variables globales
uint8_t * fb;
uint16_t width=0, height=0, bpp=0, pitch=0;
uint8_t newFb [100000000]; // CAMBIAR POR MALLOC

int main() {
    fb = newFb;
    getVideoData(&width,&height,&bpp,&pitch);
    
    fbFill(fb,0x0A0105);
    fbDrawText(fb, "Bienvenido a Pongis Golf", 0xFFFFFF, 0x0A0105, 20, 20);
    fbSet(fb);
    while(1);
    //for(int i = 10000000 ; i > 0 ; i --);
    return 1;
}
