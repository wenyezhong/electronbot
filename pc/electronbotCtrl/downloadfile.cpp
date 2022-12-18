#include "downloadfile.h"
#include<QDebug>
#include<mainwindow.h>

//extern MainWindow *pWindows;

downLoadFile::downLoadFile(commUSB *pcomm)
{
    //sendpacket_to_terminal=callBackFun;
    pcomm_usb = pcomm;
    //downloadfileName = "F:/electronbot.bin";
}
downLoadFile::~downLoadFile()
{
    pcomm_usb = nullptr;
    if(downLoad_file.isOpen())
    {
        downLoad_file.close();
    }
}

void downLoadFile::sendPacket(uint8_t *ptr)
{
    int sendcnt=0;
    int lastSize = BUFF_SIZE%ONCE_SIZE;
    int fullPackets=BUFF_SIZE/ONCE_SIZE;
    //int sendtotal=lastSize?(fullPackets+1):fullPackets;
    for(sendcnt=0 ; sendcnt<fullPackets;sendcnt++)
    {
        pcomm_usb->WriteElectronbotUSB(ptr+ONCE_SIZE*sendcnt,ONCE_SIZE);
//        Sleep(50);
    }
    if(lastSize)
    {
        pcomm_usb->WriteElectronbotUSB(ptr+ONCE_SIZE*sendcnt,lastSize);
    }

}

void downLoadFile::seFilename(QString &filename)
{
    //sendpacket_to_terminal=callBack;
    downloadfileName = filename;
    currentPacketNo = 0;
    totalPackets = 0;
}
void downLoadFile::NoticeAppIntoBootLoader(uint8_t *ptr)
{
    currentPacketNo = 0;
    totalPackets = 0;
    qDebug()<<"NoticeAppIntoBootLoader";
    ptr[PAR_INDEX+31] = 0xfe; //通知进入bootloader
    ptr[PAR_INDEX+30] = 0x01; //
    sendPacket(ptr);
    //test
    //NoticeEraseFlash(ptr);
}
void downLoadFile::QueryBootLoaderReady(uint8_t *ptr)
{
    currentPacketNo = 0;
    totalPackets = 0;
    ptr[PAR_INDEX+31] = 0xfe; //查询是否进入bootloader
    ptr[PAR_INDEX+30] = 0x02; //

    sendPacket(ptr);

}
void downLoadFile::NoticeEraseFlash(uint8_t *ptr)
{
    uint32_t i;
    downLoad_file.setFileName(downloadfileName);

    if(!downLoad_file.exists())//判断是否建立成功
    {

        //QString str = "world";
        //qDebug()<<"hello "<<str<<"!"<<endl;

    }
    else
    {
        //this->showMaximized();//成功则窗口会最大化，这只是我用检测的方法
        qDebug("read file\r\n");
        qint64 fileSize=downLoad_file.size();
        qDebug("size=%ld\r\n",fileSize);
        totalPackets = fileSize%SIZE_PER?(fileSize/SIZE_PER+1):(fileSize/SIZE_PER);
        qDebug("totalPackets=%d\r\n",totalPackets);
        if(downLoad_file.open(QIODevice::ReadOnly|QIODevice::Truncate))//打开文件，以只读的方式打开文本文件
        {
            /*QDateTime time= QDateTime::currentDateTime();//获取系统当前的时间
            uint sTime = time.toTime_t();
            qDebug("sTime=%d\r\n",sTime);*/
//            for(i=0;i<totalPackets;i++)
//            {
//                qint32 n=file.read((char*)buf,512);
//                electronbot_usb->WriteElectronbotUSB(buf,512);
//            }
            /*time= QDateTime::currentDateTime();//获取系统当前的时间
            uint eTime = time.toTime_t();
            qDebug("op time=%d\r\n",eTime-sTime);*/


            //因为readline函数读取文件内容成功的话就会返回文件的字节数，如果失败就会返回-1
            /*qDebug("len=%d\r\n",n);
            if(n!=-1)
            {

                for(i=0;i<n;i++)
                    qDebug("%02x",buf[i]);
                qDebug("\r\n");

            }*/
            //downLoadfile.close();
            *(uint32_t*)&ptr[PAR_INDEX+26] = fileSize;
            ptr[PAR_INDEX+31] = 0xfe; //查询是否进入bootloader
            ptr[PAR_INDEX+30] = 0x03; //

            sendPacket(ptr);
         }
       else
       {
            qDebug()<<downLoad_file.errorString();
       }
    }



}
void downLoadFile::DownLoadAppData(uint8_t *ptr)
{
    qint32 n=downLoad_file.read((char*)ptr,SIZE_PER);
//	size_t readLen;

//	memset(updateProgramInfo.pSendCache,0,(CODE_CACHE_LEN+2));
//	updateProgramInfo.pSendCache[0]=(u8)updateProgramInfo.packetSeq;
//	updateProgramInfo.pSendCache[1]=(u8)(updateProgramInfo.packetSeq>>8);
//	//readLen=fread(&updateProgramInfo.pSendCache[2],CODE_CACHE_LEN,1,updateProgramInfo.fp);
//	readLen=fread(&updateProgramInfo.pSendCache[2],1,CODE_CACHE_LEN,updateProgramInfo.fp);
//	if(readLen!=CODE_CACHE_LEN)
//	{

//			debug_printf("readLen=%d\r\n",readLen);
////				debug_printf("读数据长度不足\r\n");

//	}
////		debug_printf("packetSeq=%d\r\n",updateProgramInfo.packetSeq);
//	updateProgramInfo.packetSeq++;
//	updateProgramInfo.pM_DownFileProgress->StepIt();
//	sendPack(DOWN_APP_CMD,130,updateProgramInfo.pSendCache);

    ptr[PAR_INDEX+31] = 0xfe; //查询是否进入bootloader
    ptr[PAR_INDEX+30] = 0x04; //
    *(uint16_t*)&ptr[PAR_INDEX+28] = currentPacketNo;
    *(uint16_t*)&ptr[PAR_INDEX+24] = SIZE_PER;
    sendPacket(ptr);
    currentPacketNo ++;
//    if(currentPacketNo>=totalPackets)
//    {
//        qDebug("%d",n);
//    }


}
bool downLoadFile::isTxCplt(void)
{
    if(currentPacketNo>=totalPackets)
    {
        return true;
    }
    else {
        return false;
    }
}
void downLoadFile::NoticeComplete(uint8_t *ptr)
{
    if(downLoad_file.isOpen())
        downLoad_file.close();
    ptr[PAR_INDEX+31] = 0xfe; //查询是否进入bootloader
    ptr[PAR_INDEX+30] = 0x05; //

    sendPacket(ptr);


}
