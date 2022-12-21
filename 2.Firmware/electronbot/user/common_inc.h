#ifndef COMMON_INC_H
#define COMMON_INC_H

#define CONFIG_FW_VERSION 1.0

/*---------------------------- C Scope ---------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
// #include "freertos_inc.h"
#include "usb_device.h"

void Main(void);

extern uint64_t serialNumber;
extern char serialNumberStr[13];
typedef struct
{
    uint8_t extraDataTx[32];
    uint8_t rxData[2][60 * 240 * 3 + 32]; // 43232bytes, 43200 of which are lcd buffer
    // uint8_t rxData[2][60 * 3 + 32];
    volatile uint16_t receivedPacketLen;
    volatile uint8_t pingPongIndex;
    volatile uint32_t rxDataOffset;
}UsbBuffer_t;
extern  UsbBuffer_t usbBuffer;
#ifdef __cplusplus
}


/*---------------------------- C++ Scope ---------------------------*/



#endif
#endif
