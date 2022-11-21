#ifndef COMMUSB_H
#define COMMUSB_H

#include"libusb.h"

class commUSB
{
public:
    commUSB();
    ~commUSB();
    void print_dev(void);
    void test_my_usb_devices(int vid,int pid);
    void openElectronbotUSB(int vid,int pid);
    int ReadElectronbotUSB(uint8_t *ptr,uint32_t len);
    int WriteElectronbotUSB(uint8_t *ptr,uint32_t len);
    void CloseElectronbotUSB(void);
private:
    libusb_device **devs;
    libusb_device_handle *handle;
    int8_t endpoint_in = 0;
    int8_t endpoint_out = 0;
};

#endif // COMMUSB_H
