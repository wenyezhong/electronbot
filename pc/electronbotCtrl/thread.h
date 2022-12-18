#ifndef THREAD_H
#define THREAD_H
#include<QThread>
#include <minwindef.h>
class Thread:public QThread
{
    Q_OBJECT
    //....
signals:
    int sendRecDat(BYTE*);
protected:
    void run();
public:
    bool runFlag;
    bool usbReadFlag;
};



#endif // THREAD_H
