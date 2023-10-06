#include <xc.h>
#include "mcc_generated_files/usb/usb.h"
#include "mcc_generated_files/device_config.h"

#include "epd.h"
#include "usb.h"
#include "board.h"
#include "graphics.h"

void main(void) {
    // Set primary clock
    OSCTUNEbits.PLLEN = 0;
    OSCCON = 0x73;
    
    bool from_sleep = WDTCONbits.DS;
    if (from_sleep) {
        WDTCONbits.DS = 0; // Clear flag
        DSCONLbits.RELEASE = 0; // Release GPIO from values held during sleep
    }
    
    // Enable interrupts
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    if (!from_sleep) {
        rtc_init();
        DSGPR0 = 0; // mark that we haven't had our time set by USB
        // DSGPR0 is a general purpose register maintained in deep sleep
    }
    
    ANCON0bits.PCFG2 = 1; // Set AN2 (A2) as digital input
    TRISA2 = 1;
    if (RA2 && !DSGPR0) { // USB is connected and the time hasn't been set
        __delay_ms(500); // see if we are about to be programmed
        draw_screen_usb();
        epd_update();
        
        // Start up the fast oscillator for USB
        OSCTUNEbits.PLLEN = 1;
        OSCCON = 0x70;
        while (!OSCCONbits.OSTS);
        USBDeviceInit();
        USBDeviceAttach();

        bool success = false;
        while (!success && RA2) {
            success = usb_heartbeat();
        }
        DSGPR0 = success;
        // Clock out success message
        for (uint8_t i = 0; i < 16; i++) {
            usb_heartbeat();
        }
        USBDeviceDetach();
    } else if (!RA2) { // USB has been removed, mark ourselves as settable again
        DSGPR0 = 0;
    }
    
    draw_screen_main();
    epd_update();
    
    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISC = 0xFF;
    
    // Enter deep sleep
    INTCONbits.GIE = 0;
    WDTCONbits.REGSLP = 1;
    OSCCONbits.IDLEN = 0;
    DSCONHbits.DSEN = 1;
    SLEEP();
    
    while (1);
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
    }
    if (INTCON3bits.INT1IF) {
        // Used to wake ourselves from sleep on EPD actions complete
        INTCON3bits.INT1IF = 0;
    }
}