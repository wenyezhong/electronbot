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
private:
    libusb_device **devs;
};

#endif // COMMUSB_H
