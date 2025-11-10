#ifndef TIME_H
#define TIME_H

#include <stdint.h>

/* TIME */
// Tiempo de booteo en ms
uint64_t getBootTime();
// Guarda el tiempo según el reloj interno del dispositivo (por lo general, UTC 0)
void getLocalTime(uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
// Guarda la fecha según el reloj interno del dispositivo (por lo general, UTC 0)
void getLocalDate(uint8_t *year, uint8_t *month, uint8_t *day);

#endif