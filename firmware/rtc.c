#include "rtc.h"
#include <xc.h>

void rtc_init(void) {
    T1CONbits.TMR1CS = 0b10; // Set source to T1OSC
    T1CONbits.T1OSCEN = 1;
    T1CONbits.TMR1ON = 1;
    
    while (RTCCFGbits.RTCSYNC);
    
    INTCONbits.GIE = 0;
    
    EECON2 = 0x55;
    EECON2 = 0xAA;
    RTCCFGbits.RTCWREN = 1;
    
    RTCCFGbits.RTCPTR0 = 1;
    RTCCFGbits.RTCPTR1 = 1;
    
    RTCVALL = 0x22; // Year
    RTCVALH = 0x00; // dummy to decrement
    RTCVALL = 0x10; // day
    RTCVALH = 0x12; // month
    RTCVALL = 0x23; // hours
    RTCVALH = 0x06; // weekday
    RTCVALL = 0x00; // seconds
    RTCVALH = 0x58; // minutes
    RTCCFGbits.RTCEN = 1;
    
    PIE3bits.RTCCIE = 1;
    
    ALRMCFGbits.CHIME = 1; // Allow alarm to repeat indefinitely
    ALRMCFGbits.AMASK = 0b0010; // Every 10 seconds
    ALRMVALL = 0x00; // On second 0
    ALRMCFGbits.ALRMEN = 1;
    
    RTCCFGbits.RTCWREN = 0;
    
    INTCONbits.GIE = 1;
}
