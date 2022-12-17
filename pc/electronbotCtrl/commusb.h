#ifndef COMMUSB_H
#define COMMUSB_H

#include"libusb.h"
#define USBD_CMPSIT_VID     0x1215
#define USBD_CMPSIT_PID     0x0D3b

#define WIN_USB_EPIN_ADDR   0x83
#define WIN_USB_EPOUT_ADDR  0x03

class commUSB
{
public:
    commUSB();
    ~commUSB();
    void print_dev(void);
    void test_my_usb_devices(int vid,int pid);
    bool openElectronbotUSB(int vid,int pid);
    int ReadElectronbotUSB(uint8_t *ptr,uint32_t len);
    int WriteElectronbotUSB(uint8_t *ptr,uint32_t len);
    void CloseElectronbotUSB(void);
private:
    libusb_device **devs;
    libusb_device_handle *handle;
    int8_t endpoint_in = 0;
    int8_t endpoint_out = 0;
    int iface_index=0xff;
};

#endif // COMMUSB_H
