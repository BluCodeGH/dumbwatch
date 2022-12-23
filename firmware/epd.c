#include "epd.h"
#include <xc.h>
#include "mcc_generated_files/device_config.h"

#include <stdbool.h>

#include "graphics.h"

const bool BUSY = 1; // Whether busy pin is active high (1) or low (0)

const uint8_t EPD_2IN13_V2_lut_full_update[] = {
    0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00, //LUT0: BB:     VS 0 ~7
    0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, //LUT1: BW:     VS 0 ~7
    0x80, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00, //LUT2: WB:     VS 0 ~7
    0x10, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, //LUT3: WW:     VS 0 ~7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //LUT4: VCOM:   VS 0 ~7

    0x03, 0x03, 0x00, 0x00, 0x02, // TP0 A~D RP0
    0x09, 0x09, 0x00, 0x00, 0x02, // TP1 A~D RP1
    0x03, 0x03, 0x00, 0x00, 0x02, // TP2 A~D RP2
    0x00, 0x00, 0x00, 0x00, 0x00, // TP3 A~D RP3
    0x00, 0x00, 0x00, 0x00, 0x00, // TP4 A~D RP4
    0x00, 0x00, 0x00, 0x00, 0x00, // TP5 A~D RP5
    0x00, 0x00, 0x00, 0x00, 0x00, // TP6 A~D RP6

    0x15, 0x41, 0xA8, 0x32, 0x30, 0x0A,
};

void spi_init() {
    LATB2 = 1; // Set CS inactive
    LATB3 = 0; // Set D/C default to command
    LATB4 = 1; // Set reset inactive
    TRISB2 = 0; // Set CS, D/C, reset as outputs
    TRISB3 = 0;
    TRISB4 = 0;
    TRISB5 = 1; // Set busy as input
    
    RPINR1 = 8; // Map RB5/RP8 to pin interrupt 1
    INTCON2bits.INTEDG1 = !BUSY; // Set trigger edge
    
    PIE3bits.SSP2IE = 0; // Enable interrupts
    
    RPINR22 = 4; // Set SCK2 input to RB1/RP4
    RPOR4 = 11; // Set RB1/RP4 output to SCK2
    RPOR3 = 10; // Set RB0/RP3 output to SDO2
    
    LATB1 = 0; // Set clock idle low
    TRISB1 = 0; // Set clock, SDO as outputs
    TRISB0 = 0;
    
    // SPI Mode 0
    SSP2STATbits.SMP = 0;
    SSP2STATbits.CKE = 1;
    SSP2CON1bits.CKP = 0;
    
    SSP2CON1bits.SSPM = 0x0; // Clock = Fosc / 4
    SSP2CON1bits.SSPEN = 1; // Enable SPI
}

void epd_command(uint8_t cmd) {
    LATB3 = 0; // D/C command
    LATB2 = 0; // CS active
    PIR3bits.SSP2IF = 0; // Clear interrupt flag
    SSP2BUF = cmd;
    while (!PIR3bits.SSP2IF);
    PIR3bits.SSP2IF = 0; // Clear interrupt flag
    LATB2 = 1; // CS inactive
}

void epd_data(uint8_t data) {
    LATB3 = 1; // D/C data
    LATB2 = 0; // CS active
    PIR3bits.SSP2IF = 0; // Clear interrupt flag
    SSP2BUF = data;
    while (!PIR3bits.SSP2IF);
    PIR3bits.SSP2IF = 0; // Clear interrupt flag
    LATB2 = 1; // CS inactive
}

void epd_wait() {
    INTCON3bits.INT1IE = 1;
    OSCCONbits.IDLEN = 0;
    DSCONHbits.DSEN = 0;
    if (RB5 == BUSY) {
        SLEEP();
        NOP();
    }
    INTCON3bits.INT1IE = 0;
    while (RB5 == BUSY); // in case we woke early
}

void epd_reset() {
    uint8_t prev_clk = OSCCON;
    OSCCON = 0x03;
    LATB4 = 1;
    for (uint8_t i = 0; i < 25; i++); // roughly 10ms
    LATB4 = 0;
    for (uint8_t i = 0; i < 25; i++); // roughly 10ms
    LATB4 = 1;
    for (uint8_t i = 0; i < 25; i++); // roughly 10ms
    OSCCON = prev_clk;
}

void epd_init_pico_213_v2() {   
    epd_reset();
    
    epd_command(0x12);
    epd_wait();

    epd_command(0x74); //set analog block control
    epd_data(0x54);
    epd_command(0x7E); //set digital block control
    epd_data(0x3B);

    epd_command(0x01); //Driver output control
    epd_data(SIZE_Y - 1);
    epd_data(0x00);
    epd_data(0x00);

    epd_command(0x11); //data entry mode
    epd_data(0x01);

    epd_command(0x44); //set Ram-X address start/end position
    epd_data(0x00);
    epd_data(SIZE_X / 8 - 1);

    epd_command(0x45); //set Ram-Y address start/end position
    epd_data(SIZE_Y - 1);
    epd_data(0x00);
    epd_data(0x00);
    epd_data(0x00);

    // Copied from the waveshare sample code and idk what it's doing
    epd_command(0x3C); //BorderWaveform
    epd_data(0x03);

    epd_command(0x2C); //VCOM Voltage
    epd_data(0x55); // ??

    epd_command(0x03);
    epd_data(EPD_2IN13_V2_lut_full_update[70]);

    epd_command(0x04); //
    epd_data(EPD_2IN13_V2_lut_full_update[71]);
    epd_data(EPD_2IN13_V2_lut_full_update[72]);
    epd_data(EPD_2IN13_V2_lut_full_update[73]);

    epd_command(0x3A); //Dummy Line
    epd_data(EPD_2IN13_V2_lut_full_update[74]);
    epd_command(0x3B); //Gate time
    epd_data(EPD_2IN13_V2_lut_full_update[75]);

    epd_command(0x32);
    for (uint8_t count = 0; count < 70; count++) {
        epd_data(EPD_2IN13_V2_lut_full_update[count]);
    }

    epd_command(0x4E); // set RAM x address count to 0;
    epd_data(0x00);
    epd_command(0x4F); // set RAM y address count to 0X127;
    epd_data(SIZE_Y - 1);
    epd_data(0x00);
    epd_wait();
}

void epd_blit() {
    epd_command(0x24);
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t row = 0;
    for (uint16_t i = 0; i < SIZE_X * SIZE_Y / 8; i++) {
        epd_data(get_pixel_byte(x, y, row));
        x += 1;
        if (x >= SIZE_X / 8) {
            x = 0;
            row += 1;
            if (row >= 8) {
                row = 0;
                y += 1;
            }
        }
    }
    
    epd_command(0x22);
    epd_data(0xC7);
    epd_command(0x20);
    epd_wait();
}

void epd_off() {
    epd_command(0x22);
    epd_data(0xC3);
    epd_command(0x20);
    
    epd_command(0x10);
    epd_data(0x01);
}

void epd_update() {
    spi_init();
    epd_init_pico_213_v2();
    epd_blit();
    epd_off();
}