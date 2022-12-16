#include <xc.h>
#include "mcc_generated_files/usb/usb.h"

#include "epd.h"
#include "usb.h"
#include "rtc.h"
#include "graphics.h"

void main(void) {
    bool from_sleep = WDTCONbits.DS;
    if (from_sleep) {
        WDTCONbits.DS = 0; // Clear flag
        DSCONLbits.RELEASE = 0; // Release GPIO from values held during sleep
    }
    // Set up oscillators
    // SCS FOSC; IRCF 8MHz; IDLEN disabled; 
    OSCCON = 0x70;
    
    // Enable interrupts
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    if (!from_sleep) {
        rtc_init();
    }
    
    TRISCbits.TRISC2 = 1;
    ANCON1bits.PCFG11 = 0;
    ADCON1bits.ADFM = 1;
    ADCON1bits.ADCS = 0b110;
    ADCON1bits.ACQT = 0b010;
    ADCON0bits.CHS = 11;
    ADCON0bits.ADON = 1;
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    uint16_t batt_voltage = (((uint16_t)ADRESH) << 8) | ADRESL;
    uint16_t batt_voltage_mv = batt_voltage * 30 / 7;
    
    draw_time(batt_voltage_mv);
    
    spi_init();
    epd_init_pico_213_v2();
    epd_blit();
    epd_off();
    
    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISC = 0xFF;
    
    INTCONbits.GIE = 0;
    WDTCONbits.REGSLP = 1;
    OSCCONbits.IDLEN = 0;
    DSCONHbits.DSEN = 1;
    SLEEP();
    
    while (1);
    
    /*USBDeviceInit();
    USBDeviceAttach();
    
    while (1) {
        usb_heartbeat();
    }*/
}

void __interrupt() handler(void) {
    if (PIE2bits.USBIE == 1 && PIR2bits.USBIF == 1) {
        // USB interrupt handler
        USB_USBDeviceTasks();
    }
    if (PIR3bits.SSP2IF) {
        // SPI interrupt - ignore
    }
    if (PIR3bits.RTCCIF) {
        PIR3bits.RTCCIF = 0;
        usb_trig();
    }
}