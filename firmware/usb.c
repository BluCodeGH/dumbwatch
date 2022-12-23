#include "usb.h"

#include "board.h"
#include "mcc_generated_files/usb/usb.h"

static uint8_t buf[64];

uint8_t parse_hex(char digit) {
    if ('0' <= digit && digit <= '9') {
        return digit - '0';
    } else if ('a' <= digit && digit <= 'f') {
        return digit - 'a';
    } else if ('A' <= digit && digit <= 'F') {
        return digit - 'A';
    }
    return 0;
}

bool usb_heartbeat() {
    if (USBGetDeviceState() < CONFIGURED_STATE || USBIsDeviceSuspended()) {
        return false;
    }
    
    bool success = false;

    if (USBUSARTIsTxTrfReady()) {
        uint8_t numBytesRead = getsUSBUSART(buf, sizeof(buf));
        
        if (numBytesRead != 6*3) {
            if (numBytesRead > 0) {
                putrsUSBUSART("Invalid time\n");
            }
        } else {
            uint8_t year = (parse_hex(buf[0]) << 4) | parse_hex(buf[1]);
            uint8_t month = (parse_hex(buf[3]) << 4) | parse_hex(buf[4]);
            uint8_t day = (parse_hex(buf[6]) << 4) | parse_hex(buf[7]);
            uint8_t weekday = (parse_hex(buf[9]) << 4) | parse_hex(buf[10]);
            uint8_t hour = (parse_hex(buf[12]) << 4) | parse_hex(buf[13]);
            uint8_t min = (parse_hex(buf[15]) << 4) | parse_hex(buf[16]);

            rtc_set(year, month, day, weekday, hour, min);
            success = true;

            putrsUSBUSART("Set\n");
        }
    }

    CDCTxService();
    
    return success;
}
