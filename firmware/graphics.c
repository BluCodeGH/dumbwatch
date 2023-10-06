#include "graphics.h"
#include <xc.h>

#include "board.h"
#include "font.h"

// 0, 0 in the tile map is the top left corner of the display
// x is horizontal, y is vertical

uint8_t tile_map[13][26] = {0};

const uint8_t BATT_HEIGHT = 1;
const uint8_t TIME_HEIGHT = 6;
const uint8_t DATE_HEIGHT = 19;

uint8_t get_pixel_byte(uint8_t x, uint8_t y, uint8_t row) {
    if (x > 12 || y > 25) {
        return 0xFF;
    }
    uint8_t tile = tile_map[x][y];
    return TILES[tile][row];
}

void draw_string(uint8_t tile_x, uint8_t tile_y, const char *str) {
    for (const char *c = str; *c != 0; c++) {
        tile_map[tile_x++][tile_y] = *c;
    }
}

void draw_bignum(uint8_t tile_x, uint8_t tile_y, uint8_t digit) {
    for (uint8_t i = 0; i < 5; i++) {
        uint8_t row = NUMS[digit][i];
        tile_map[tile_x + 0][tile_y + i] = (row >> 2) & 1 ? TILE_FULL : TILE_EMPTY;
        tile_map[tile_x + 1][tile_y + i] = (row >> 1) & 1 ? TILE_FULL : TILE_EMPTY;
        tile_map[tile_x + 2][tile_y + i] = (row >> 0) & 1 ? TILE_FULL : TILE_EMPTY;
    }
}

void draw_time(uint8_t hour, uint8_t min) {
    if (hour == 0) { // midnight
        draw_bignum(3, TIME_HEIGHT, 1);
        draw_bignum(7, TIME_HEIGHT, 2);
    } else {
        // Hour is BCD, so converting to proper 12 hour is a little funky
        hour = (hour >> 4) * 10 + (hour & 0xF);
        if (hour > 12) {
            hour -= 12;
        }
        uint8_t msd = hour / 10;
        draw_bignum(3, TIME_HEIGHT, msd);
        draw_bignum(7, TIME_HEIGHT, hour - msd * 10);
    }
    draw_bignum(3, TIME_HEIGHT + 6, min >> 4);
    draw_bignum(7, TIME_HEIGHT + 6, min & 0xF);
}

void draw_time_unknown() {
    draw_bignum(3, TIME_HEIGHT, 10);
    draw_bignum(7, TIME_HEIGHT, 10);
    draw_bignum(3, TIME_HEIGHT + 6, 10);
    draw_bignum(7, TIME_HEIGHT + 6, 10);
}

void draw_date(uint8_t year, uint8_t month, uint8_t day) {
    tile_map[3][DATE_HEIGHT] = ' ';
    tile_map[4][DATE_HEIGHT] = (day >>  4) + '0';
    tile_map[5][DATE_HEIGHT] = (day & 0xF) + '0';
    tile_map[6][DATE_HEIGHT] = '/';
    tile_map[7][DATE_HEIGHT] = (month >>  4) + '0';
    tile_map[8][DATE_HEIGHT] = (month & 0xF) + '0';
    tile_map[9][DATE_HEIGHT] = ' ';
}

void draw_datetime() {
    uint8_t year, month, day, weekday, hour, min;
    rtc_get(&year, &month, &day, &weekday, &hour, &min);
    
    if (year == 0) {
        draw_time_unknown();
        draw_string(3, DATE_HEIGHT, "Unknown");
    } else {
        draw_date(year, month, day);
        draw_time(hour, min);
    }
    
}

void draw_usb_text() {
    draw_string(3, DATE_HEIGHT, "Waiting");
}

void draw_batt() {
    uint16_t voltage_mv = get_batt_voltage_mv();
    draw_string(0, BATT_HEIGHT, "Batt: ");
    for (uint8_t i = 0; i < 4; i++) {
        tile_map[9 - i][BATT_HEIGHT] = voltage_mv % 10 + '0';
        voltage_mv /= 10;
    }
    if (batt_charging()) {
        draw_string(11, BATT_HEIGHT, "++");
    }
}


void draw_screen_main() {
    draw_datetime();
    draw_batt();
}

void draw_screen_usb() {
    draw_time_unknown();
    draw_usb_text();
    draw_batt();
}