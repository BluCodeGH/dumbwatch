#include "graphics.h"
#include <xc.h>

const uint8_t NUMS[] = {0b111,
                        0b101,
                        0b101,
                        0b101,
                        0b111,

                        0b110,
                        0b010,
                        0b010,
                        0b010,
                        0b111,

                        0b111,
                        0b001,
                        0b111,
                        0b100,
                        0b111,

                        0b111,
                        0b001,
                        0b111,
                        0b001,
                        0b111,

                        0b101,
                        0b101,
                        0b111,
                        0b001,
                        0b001,

                        0b111,
                        0b100,
                        0b111,
                        0b001,
                        0b111,

                        0b111,
                        0b100,
                        0b111,
                        0b101,
                        0b111,

                        0b111,
                        0b001,
                        0b001,
                        0b001,
                        0b001,

                        0b111,
                        0b101,
                        0b111,
                        0b101,
                        0b111,

                        0b111,
                        0b101,
                        0b111,
                        0b001,
                        0b111};

uint8_t sprite_map[3][8] = {0};

void draw_time(uint16_t batt_voltage) {
    RTCCFGbits.RTCPTR0 = 1;
    RTCCFGbits.RTCPTR1 = 0;
    
    uint8_t hour = RTCVALL;
    uint8_t foo = RTCVALH;
    uint8_t sec = RTCVALL;
    uint8_t min = RTCVALH;
    
    sprite_map[1][0] = (hour >> 4) + 1;
    sprite_map[1][1] = (hour & 0xF) + 1;
    
    sprite_map[1][3] = (min >> 4) + 1;
    sprite_map[1][4] = (min & 0xF) + 1;
    
    sprite_map[1][6] = (sec >> 4) + 1;
    sprite_map[1][7] = (sec & 0xF) + 1;
    
    for (uint8_t i = 0; i < 4; i++) {
        sprite_map[0][7 - i] = (batt_voltage % 10) + 1;
        batt_voltage /= 10;
    }
}

bool get_pixel(uint8_t x, uint8_t y) {
    uint8_t sprite = sprite_map[x / 6][y / 4];
    if (sprite == 0) {
        return false;
    }
    sprite -= 1;
    
    x = x % 6;
    y = y % 4;
    if (y == 3 || x == 5) {
        return false; // blank vertical line between sprites
    }
    return NUMS[sprite*5 + x] & (0b100 >> y);
}