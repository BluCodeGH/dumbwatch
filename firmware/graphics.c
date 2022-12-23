#include "graphics.h"
#include <xc.h>

#include "board.h"
#include "font.h"

uint8_t tile_map[16][32] = {0};

uint8_t get_pixel_byte(uint8_t x, uint8_t y, uint8_t row) {
//    uint8_t tile = tile_map[x / TILE_SIZE][y / TILE_SIZE];
//    uint8_t row = 7 - y % TILE_SIZE;
    uint8_t tile = tile_map[x][y];
    return TILES[tile][7 - row];
}

void draw_string(uint8_t tile_x, uint8_t tile_y, const char *str) {
    for (const char *c = str; *c != 0; c++) {
        tile_map[tile_x++][tile_y] = *c;
    }
}

void draw_bignum(uint8_t tile_x, uint8_t tile_y, uint8_t digit) {
    for (uint8_t i = 0; i < 5; i++) {
        uint8_t row = NUMS[digit][4 - i];
        tile_map[tile_x + 0][tile_y + i] = (row >> 2) & 1 ? TILE_FULL : TILE_EMPTY;
        tile_map[tile_x + 1][tile_y + i] = (row >> 1) & 1 ? TILE_FULL : TILE_EMPTY;
        tile_map[tile_x + 2][tile_y + i] = (row >> 0) & 1 ? TILE_FULL : TILE_EMPTY;
    }
}

void draw_time(uint8_t hour, uint8_t min) {
    if (hour == 0) { // midnight
        draw_bignum(4, 17, 1);
        draw_bignum(8, 17, 2);
    } else {
        if (hour > 0x12) {
            hour -= 0x12;
        }
        draw_bignum(4, 17, hour >> 4);
        draw_bignum(8, 17, hour & 0xF);
    }
    draw_bignum(4, 11, min >> 4);
    draw_bignum(8, 11, min & 0xF);
}

void draw_time_unknown() {
    draw_bignum(4, 17, 10);
    draw_bignum(8, 17, 10);
    draw_bignum(4, 11, 10);
    draw_bignum(8, 11, 10);
}

void draw_date(uint8_t year, uint8_t month, uint8_t day) {
    tile_map[4][7] = ' ';
    tile_map[5][7] = (day >>  4) + '0';
    tile_map[6][7] = (day & 0xF) + '0';
    tile_map[7][7] = '/';
    tile_map[8][7] = (month >>  4) + '0';
    tile_map[9][7] = (month & 0xF) + '0';
    tile_map[10][7] = ' ';
}

void draw_datetime() {
    uint8_t year, month, day, weekday, hour, min;
    rtc_get(&year, &month, &day, &weekday, &hour, &min);
    
    if (year == 0) {
        draw_time_unknown();
        draw_string(4, 7, "Unknown");
    } else {
        draw_date(year, month, day);
        draw_time(hour, min);
    }
    
}

void draw_usb_text() {
    draw_string(4, 7, "Waiting");
}

void draw_batt() {
    uint16_t voltage_mv = get_batt_voltage_mv();
    draw_string(0, 30, "Batt: ");
//    tile_map[0][30] = 'B';
//    tile_map[1][30] = 'a';
//    tile_map[2][30] = 't';
//    tile_map[3][30] = 't';
//    tile_map[4][30] = ':';
//    tile_map[5][30] = ' ';
    for (uint8_t i = 0; i < 4; i++) {
        tile_map[9 - i][30] = voltage_mv % 10 + '0';
        voltage_mv /= 10;
    }
    if (batt_charging()) {
        draw_string(11, 30, "mv++");
    } else {
        draw_string(11, 30, "mv");
    }
//    tile_map[10][30] = ' ';
//    tile_map[11][30] = 'm';
//    tile_map[12][30] = 'v';
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