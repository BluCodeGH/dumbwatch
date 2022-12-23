#ifndef BOARD_H
#define	BOARD_H

#include <stdbool.h>
#include <stdint.h>

uint16_t get_batt_voltage_mv(void);

bool batt_charging(void);

void rtc_init(void);

void rtc_set(uint8_t year, uint8_t month, uint8_t day, uint8_t weekday, uint8_t hour, uint8_t min);

void rtc_get(uint8_t *year, uint8_t *month, uint8_t *day, uint8_t *weekday, uint8_t *hour, uint8_t *min);

void enter_sleep(void);

#endif	/* BOARD_H */

