#include "mcc_generated_files/usb/usb.h"

#include "usb.h"

static uint8_t readBuffer[64];
static uint8_t writeBuffer[64];

bool alarm = false;
void usb_trig() {
    alarm = true;
}

void usb_heartbeat() {
    if (USBGetDeviceState() < CONFIGURED_STATE || USBIsDeviceSuspended()) {
        return;
    }

    if (USBUSARTIsTxTrfReady()) {
        uint8_t i;
        uint8_t numBytesRead;

        numBytesRead = getsUSBUSART(readBuffer, sizeof(readBuffer));

        for(i=0; i<numBytesRead; i++)
        {
            switch(readBuffer[i])
            {
                case 0x0A:
                case 0x0D:
                    writeBuffer[i] = readBuffer[i];
                    break;

                default:
                    writeBuffer[i] = readBuffer[i] + 1;
                    break;
            }
        }

        if(numBytesRead > 0)
        {
            putUSBUSART(writeBuffer,numBytesRead);
        }
        
        if (alarm) {
            alarm = 0;
            putsUSBUSART("alarm\n");
        }
    }

    CDCTxService();
}
