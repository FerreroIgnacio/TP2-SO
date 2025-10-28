#include "../standard.h"

uint8_t newFb [600 * 800 * 4 * 2]; // CAMBIAR POR MALLOC
frame_t newFrame;


static frame_t * frame;
#define SHELL_COLOR 0x00000A
int main() {
  /*  write(STDOUT, "asdf\n", 5);
    int16_t width, height;
    getVideoData( &width,&height, NULL, NULL);
    frame = &newFrame;
    frameInit(frame, newFb);
    frameFill (frame, SHELL_COLOR);
    setFrame(frame);
      //      while(1){

       //     }*/
    return 0;
}
