#ifndef AUDIO_H
#define AUDIO_H
#include <stdint.h>

void playFreq(uint16_t freq, uint64_t ms);
void startSound(uint16_t freq);
void stopSound();

#endif