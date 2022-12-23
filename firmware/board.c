#include "board.h"
#include <xc.h>

uint16_t get_batt_voltage_mv(void) {
    TRISCbits.TRISC2 = 1; // Set C2 as input
    ANCON1bits.PCFG11 = 0; // Enable analog input on C2
    ADCON1bits.ADFM = 1; // Right justify output
    ADCON1bits.ADCS = 0b110; // Select conversion clock of Fosc/64
    ADCON1bits.ACQT = 0b010; // Select aquisiton time of 4 Tad
    ADCON0bits.CHS = 11; // Select AN11
    ADCON0bits.ADON = 1; // Turn on ADC
    ADCON0bits.GO = 1; // Start conversion
    while (ADCON0bits.GO); // Wait for conversion to finish
    uint16_t batt_voltage = (((uint16_t)ADRESH) << 8) | ADRESL;
    return batt_voltage * 30 / 7; // Rough approximation of a 10 bit ADC with 3.3v power without overflowing
}

bool batt_charging(void) {
    ANCON0bits.PCFG1 = 1;
    TRISAbits.TRISA1 = 1;
    return !RA1; // active low
}

void rtc_init(void) {
    T1CONbits.TMR1CS = 0b10; // Set source to T1OSC
    T1CONbits.T1OSCEN = 1;
    T1CONbits.TMR1ON = 1;
    
     // Say the magic words to unlock the registers
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    RTCCFGbits.RTCWREN = 1;
    
    RTCCFGbits.RTCPTR0 = 1;
    RTCCFGbits.RTCPTR1 = 1;
    
    RTCVALL = 0;
    RTCVALH = 0; // dummy to decrement
    RTCVALL = 0;
    RTCVALH = 0;
    RTCVALL = 0;
    RTCVALH = 0;
    RTCVALL = 0;
    RTCVALH = 0;
    RTCCFGbits.RTCEN = 1;
    
    RTCCFGbits.RTCWREN = 0; // re-lock the registers
    INTCONbits.GIE = 1;
    
    // set up alarm to wake every minute
    ALRMCFGbits.CHIME = 1; // Allow alarm to repeat indefinitely
    ALRMCFGbits.AMASK = 0b0011; // Every minute
    ALRMVALL = 0x00; // On second 0
    ALRMCFGbits.ALRMEN = 1;
}

void rtc_set(uint8_t year, uint8_t month, uint8_t day, uint8_t weekday, uint8_t hour, uint8_t min) {
    while (RTCCFGbits.RTCSYNC);
    
    // Say the magic words to unlock the registers
    INTCONbits.GIE = 0;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    RTCCFGbits.RTCWREN = 1;
    
    RTCCFGbits.RTCPTR0 = 1;
    RTCCFGbits.RTCPTR1 = 1;
    
    RTCVALL = year;
    RTCVALH = 0x00; // dummy to decrement
    RTCVALL = day;
    RTCVALH = month;
    RTCVALL = hour;
    RTCVALH = weekday;
    RTCVALL = 0x00; // seconds
    RTCVALH = min;
    
    RTCCFGbits.RTCWREN = 0; // re-lock the registers
    INTCONbits.GIE = 1;
    
}

void rtc_get(uint8_t *year, uint8_t *month, uint8_t *day, uint8_t *weekday, uint8_t *hour, uint8_t *min) {
    while (RTCCFGbits.RTCSYNC); // Wait until it is safe to read (ie rollover isn't possible)
    
    RTCCFGbits.RTCPTR0 = 1;
    RTCCFGbits.RTCPTR1 = 1;
    
    *year = RTCVALL;
    uint8_t foo = RTCVALH; // unused
    *day = RTCVALL;
    *month = RTCVALH;
    *hour = RTCVALL;
    *weekday = RTCVALH;
    // sec = RTCVALL;
    *min = RTCVALH;
}

void enter_sleep() {
    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISC = 0xFF;
    
    PIE3bits.RTCCIE = 1;
    INTCONbits.GIE = 0;
    WDTCONbits.REGSLP = 1;
    OSCCONbits.IDLEN = 0;
    DSCONHbits.DSEN = 1;
    SLEEP();
    NOP();
}