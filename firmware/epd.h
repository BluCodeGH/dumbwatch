#ifndef EPD_H
#define	EPD_H

#include <stdint.h>

const uint8_t SIZE_X = 128;
const uint8_t SIZE_Y = 250;

void spi_init();

void epd_init_pico_213_v2();

void epd_blit();

void epd_off();

#endif