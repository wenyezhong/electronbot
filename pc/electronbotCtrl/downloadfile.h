#ifndef DOWNLOADFILE_H
#define DOWNLOADFILE_H

#include <stdint.h>
//#include "mainwindow.h"
#include <QString>
#include <QWidget>
#include <QFile>

#include "commusb.h"
#define BUFF_SIZE  43232
#define PAR_INDEX  43200
#define ONCE_SIZE  512

#define  SIZE_PER  4096

typedef void (*psend_CallbackTypeDef)(void);

class downLoadFile
{

private:
    QString downloadfileName;
    QFile downLoad_file;
    uint32_t totalPackets;
    uint16_t currentPacketNo;

public:
    downLoadFile(commUSB *pcomm);
    ~downLoadFile();
    commUSB *pcomm_usb;
    void NoticeAppIntoBootLoader(uint8_t *ptr);
    void QueryBootLoaderReady(uint8_t *ptr);
    void NoticeEraseFlash(uint8_t *ptr);
    void DownLoadAppData(uint8_t *ptr);
    void NoticeComplete(uint8_t *ptr);
    bool isTxCplt(void);

//    void (*sendpacket_to_terminal)(void);

    void sendPacket(uint8_t *ptr);

    void seFilename(QString &filename);
};

#endif // DOWNLOADFILE_H
