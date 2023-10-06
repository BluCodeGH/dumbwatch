#ifndef EPD_H
#define	EPD_H

#include <stdint.h>

extern const uint8_t SIZE_X;
extern const uint8_t SIZE_Y;

void spi_init();

void epd_init();

void epd_blit();

void epd_off();

void epd_update(void);

#endif