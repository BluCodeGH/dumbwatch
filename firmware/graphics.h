#ifndef GRAPHICS_H
#define	GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>

void draw_time(uint16_t batt);

bool get_pixel(uint8_t x, uint8_t y);

#endif