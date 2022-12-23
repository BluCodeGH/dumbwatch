#ifndef GRAPHICS_H
#define	GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>

uint8_t get_pixel_byte(uint8_t x, uint8_t y, uint8_t row);

void draw_screen_main(void);

void draw_screen_usb(void);

#endif