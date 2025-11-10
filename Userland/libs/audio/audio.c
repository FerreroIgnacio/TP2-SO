#include "audio.h"

void playFreq(uint16_t freq, uint64_t ms)
{
    startSound(freq);
    uint64_t start = getBootTime();
    while (getBootTime() - start < ms)
        ;
    stopSound();
}