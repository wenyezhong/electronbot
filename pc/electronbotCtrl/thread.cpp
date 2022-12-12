#include "thread.h"
#include <minwindef.h>
#include "commusb.h"
#include<mainwindow.h>

extern MainWindow *pWindows;
void Thread::run()
{
    int ret = 0;
    BYTE a[48]={0x01,0x02,0x03,0x04};
    while(1)
    {
        //commUSB::ReadElectronbotUSB(a,32);
        ret = pWindows->electronbot_usb->ReadElectronbotUSB((uint8_t *)a,32);
        if(ret>=0)
        {
            //qDebug("tttttt");
            emit sendRecDat(a);
        }
        QThread::usleep(1000000);
    }
}
