#ifndef COMMON_INC_H
#define COMMON_INC_H

#define CONFIG_FW_VERSION 1.0





// #include "main.h"

// #include "freertos_inc.h"
// #include "usb_device.h"


//extern uint64_t serialNumber;
//extern char serialNumberStr[13];
typedef struct
{
    uint8_t extraDataTx[32];
    uint8_t rxData[2][60 * 240 * 3 + 32]; // 43232bytes, 43200 of which are lcd buffer
    // uint8_t rxData[2][60 * 3 + 32];
    volatile uint16_t receivedPacketLen;
    volatile uint8_t pingPongIndex;
    volatile uint32_t rxDataOffset;
}UsbBuffer_t;


/*---------------------------- C++ Scope ---------------------------*/




#endif
