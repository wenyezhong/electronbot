#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commusb.h"
#include "QFileDialog"
#include <QMessageBox>
#include<QTextEdit>
#include<QFile>
#include<QtDebug>
#include <QDateTime>
#include "thread.h"


#include<QPainter>
#include<QPixmap>
#include<QRect>
#include<QImage>



commUSB *pcommUSB;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initPara();
    recvUSB_Timer = new QTimer(this);
    connect(recvUSB_Timer, SIGNAL(timeout()), this, SLOT(RecvUSBTask()));
    //recvUSB_Timer->start(500);
    electronbot_usb = new commUSB;
    pcommUSB = electronbot_usb;
    //electronbot_usb->print_dev();
    bool ret=electronbot_usb->openElectronbotUSB(USBD_CMPSIT_VID,USBD_CMPSIT_PID);
    if(ret)
    {
        ui->statusBar->showMessage(tr("已连接"));
        ui->statusBar->setStyleSheet("background-color: rgb(0, 170, 0)");
    }
    else
    {
        ui->statusBar->showMessage(tr("未连接"));
        ui->statusBar->setStyleSheet("background-color: rgb(213, 0, 0)");
    }


    readUsbTread = new Thread;
    connect(readUsbTread,SIGNAL(sendRecDat(BYTE*)),this,SLOT(recUsbDatas(BYTE*)));
    readUsbTread->runFlag = true;
    readUsbTread->start();

    //pDownLoadFile = new downLoadFile(electronbot_usb);


}

MainWindow::~MainWindow()
{
    if(pDownLoadFile)
        delete pDownLoadFile;
    if(electronbot_usb)
        delete electronbot_usb;
    if(recvUSB_Timer)
        delete recvUSB_Timer;
    if(readUsbTread)
        delete readUsbTread;
    delete ui;
}
void MainWindow::reconnectElectronbotUSB(void)
{
    readUsbTread->runFlag = false;
    Sleep(500);
    readUsbTread->terminate();

    pcommUSB=nullptr;
    delete electronbot_usb;

    electronbot_usb = new commUSB;
    pcommUSB = electronbot_usb;
    bool ret=electronbot_usb->openElectronbotUSB(USBD_CMPSIT_VID,USBD_CMPSIT_PID);
    if(ret)
    {
        ui->statusBar->showMessage(tr("已连接"));
        ui->statusBar->setStyleSheet("background-color: rgb(0, 170, 0)");
    }
    else
    {
        ui->statusBar->showMessage(tr("未连接"));
        ui->statusBar->setStyleSheet("background-color: rgb(213, 0, 0)");
    }
    readUsbTread->runFlag = true;
    readUsbTread->start();

}
void MainWindow::recUsbDatas(BYTE* ptr)
{
    int i;
    uint16_t cmd = *(uint16_t*)&ptr[30];
    qDebug("cmd = %.4x",cmd);
    switch(cmd)
    {
        case 0xef01:{

            if(ptr[29]==0)
            {
                //reconnectElectronbotUSB();
//                Sleep(1000);
                pDownLoadFile->QueryBootLoaderReady(txData);
            }

        }break;

        case 0xef02:{

            if(ptr[29]==0)
            {
                pDownLoadFile->NoticeEraseFlash(txData);

            }

        }break;

        case 0xef03:{

            if(ptr[29]==0)
            {

                pDownLoadFile->DownLoadAppData(txData);

            }

        }break;

        case 0xef04:{

            if(ptr[29]==0)
            {
                //pDownLoadFile->NoticeComplete(txData);
                uint16_t curPakcetNo = *(uint16_t*)&ptr[27];
                ui->progressBar->setValue(curPakcetNo);
                qDebug("curPakcetNo = %d",curPakcetNo);
                if(pDownLoadFile->isTxCplt())
                {
                    qDebug("send txcplt notice");
                    pDownLoadFile->NoticeComplete(txData);
                }
                else
                {
                    qDebug("DownLoadAppData");
                    pDownLoadFile->DownLoadAppData(txData);

                }

            }
            else {
                qDebug("DownLoadAppData failed");
            }

        }break;

        case 0xef05:{

            if(ptr[29]==0)
            {
                uint16_t recvTotalPackets = *(uint16_t*)&ptr[28];
                ui->progressBar->setValue(recvTotalPackets);
                qDebug("recvTotalPackets = %d",recvTotalPackets);
                //pDownLoadFile->QueryBootLoaderReady(txData);
                ui->statusBar->showMessage(ui->statusBar->currentMessage()+"     下载成功！");
                delete pDownLoadFile;
//                ui->statusBar->showMessage(tr("未连接"));
//                ui->statusBar->setStyleSheet("background-color: rgb(0, 170, 0)");

            }

        }break;

        default:{

                float head_angle=*(float*)&ptr[1];
                float lroll_angle=*(float*)&ptr[5];
                float lpitch_angle=*(float*)&ptr[9];
                float rroll_angle=*(float*)&ptr[13];
                float rpitch_angle=*(float*)&ptr[17];
                float body_angle=*(float*)&ptr[21];
            //    for(i=0;i<32;i++)
            //        qDebug("%x ",ptr[i]);
                QString val;

                ui->lineEdit_head_angle->setText(QString::number(head_angle,'f',2));
                ui->lineEdit_lroll_angle->setText(QString::number(lroll_angle,'f',2));
                ui->lineEdit_lpitch_angle->setText(QString::number(lpitch_angle,'f',2));
                ui->lineEdit_rroll_angle->setText(QString::number(rroll_angle,'f',2));
                ui->lineEdit_rpitch_angle->setText(QString::number(rpitch_angle,'f',2));
                ui->lineEdit_body_angle->setText(QString::number(body_angle,'f',2));

            }break;


    }


}
void MainWindow::sendPacket(void)
{
    int sendcnt=0;
    int lastSize = BUFF_SIZE%ONCE_SIZE;
    int fullPackets=BUFF_SIZE/ONCE_SIZE;
    //int sendtotal=lastSize?(fullPackets+1):fullPackets;
    for(sendcnt=0 ; sendcnt<fullPackets;sendcnt++)
    {
        electronbot_usb->WriteElectronbotUSB(txData+ONCE_SIZE*sendcnt,ONCE_SIZE);
//        Sleep(50);
    }
    if(lastSize)
    {
        electronbot_usb->WriteElectronbotUSB(txData+ONCE_SIZE*sendcnt,lastSize);
    }

}
void MainWindow::on_pushButton_clicked()
{
    QString bmpFileName;
    bmpFileName = QFileDialog::getOpenFileName(this,tr("Open Image"),"./",tr("JPEG Files(*.jpg);;Bmp Files(*.bmp)"));//xx/xx/xx.xx
    qDebug("%s",bmpFileName.toStdString().data());

//    QPixmap *backgroundPixmap = new QPixmap("D:/gnuarmstm32/electronbot/robot2.jpg");
    QPixmap *backgroundPixmap = new QPixmap(bmpFileName);
    QGraphicsScene *scene = new QGraphicsScene(this);
    //scene->addPixmap(QPixmap("D:\gnuarmstm32\electronbot/robot2.jpg"));

    ui->graphicsView->resize(240, 240);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QPixmap sized = backgroundPixmap->scaled(
            QSize(ui->graphicsView->width(),
                  ui->graphicsView->height()),
            Qt::KeepAspectRatioByExpanding); // This scales the image too tall

    QImage sizedImage = QImage(sized.toImage()).convertToFormat(QImage::Format_RGB888);


    scene->addPixmap(QPixmap::fromImage(sizedImage));
    ui->graphicsView->setScene(scene);
    qDebug()<<"h:"<<ui->graphicsView->height()<<"w: "<<ui->graphicsView->width();
    qDebug()<<"h:"<<sizedImage.height()<<"w: "<<sizedImage.width();

    //scene->
    //ui->graphicsView->
    uint8_t *ptr=(uint8_t *)malloc(1024*1024);//[=(uint8_t*)sizedImage.data_ptr();
    int step = sizedImage.bytesPerLine(); //image->width();
    int wd = sizedImage.width();
    int ht = sizedImage.height();

    uchar* iptr = sizedImage.bits();
    uchar* ptr_var = iptr;
    qDebug()<<"step:"<<step;
    qDebug()<<"h:"<<ht<<"w: "<<wd;
//    uchar **pptr = new uchar*[ht];

    for (int y = 0; y < sizedImage.height(); y++) {
        memcpy(ptr+720*y,iptr+y*wd*3,240*3);
        //ptr_var += step;
        //pptr[y] = ptr_var; //&(((uchar*)ptr_var)[0]);
    }


    //memcpy(ptr,sizedImage.bits(),240*240*3);
    //memset(ptr,100,1024*1024);
    for(int i=0;i<4;i++)
    {
        txData[PAR_INDEX+31] = 0x01;
        memcpy(txData,ptr+i*240*60*3,60*240*3);
        sendPacket();
        Sleep(10);
    }
    free(ptr);
    //ui->graphicsView->show();
#if 0
    BITMAPFILEHEADER *pBITMAPFILEHEADER;
    BMP_INFOHEADER *pBMP_INFOHEADER;
    int w,h;
    char data[1024];
    QString bmpFileName;
    QFile bmpFile;
    bmpFileName = QFileDialog::getOpenFileName(this,tr("Open Image"),"./",tr("Image Files(*.bmp)"));//xx/xx/xx.xx
    qDebug("%s",bmpFileName.toStdString().data());

    bmpFile.setFileName(bmpFileName);
    bmpFile.open(QIODevice::ReadOnly);

    bmpFile.read(data,sizeof(BITMAPFILEHEADER)+sizeof(BMP_INFOHEADER));
    pBITMAPFILEHEADER=(BITMAPFILEHEADER*)data;
    pBMP_INFOHEADER=(BMP_INFOHEADER*)(data+sizeof(BITMAPFILEHEADER));
    qDebug("offset = %d",pBITMAPFILEHEADER->bfOffBits);
    qDebug("biSize = %d",pBMP_INFOHEADER->biSize);
    qDebug("biWidth = %d  biHeight = %d",pBMP_INFOHEADER->biWidth,pBMP_INFOHEADER->biHeight);
    qDebug("biPlanes = %d",pBMP_INFOHEADER->biPlanes);
    qDebug("biBitCount = %d",pBMP_INFOHEADER->biBitCount);
    qDebug("biCompression = %d",pBMP_INFOHEADER->biCompression);
    qDebug("biSizeImage = %d",pBMP_INFOHEADER->biSizeImage);
    qDebug("biClrUsed = %d",pBMP_INFOHEADER->biClrUsed);
    qDebug("biClrImportant = %d",pBMP_INFOHEADER->biClrImportant);

    w=pBMP_INFOHEADER->biWidth;
    h=pBMP_INFOHEADER->biHeight;

    char palette[256][4];
    bmpFile.seek(sizeof(BITMAPFILEHEADER)+sizeof(BMP_INFOHEADER));
    bmpFile.read(palette[0],1024);
    bmpFile.seek(pBITMAPFILEHEADER->bfOffBits);
    int i,j;
    uint8_t r,g,b;
    uint8_t index;

    for(i=h; i >0; --i)
    {
        bmpFile.read(data,w);
        for(j=0;j<w;j++)
        {
            index=(uint8_t)data[j];
            r = (uint8_t)palette[index][2];
            g = (uint8_t)palette[index][1];
            b = (uint8_t)palette[index][0];
//                qDebug("%x",index);
//                qDebug("%x",r);
//                qDebug("%x",g);
//                qDebug("b=%x",b);
//                painter.setPen(QColor(palette[index][0],palette[index][1],palette[index][2],palette[index][3]));
            //painter.setPen(QColor(r,g,b));
            //painter.drawPoint(j,i);
        }

    }
    bmpFile.close();
#endif
}
void MainWindow::RecvUSBTask()
{
    uint8_t buf[100];
    //qDebug("hello timer!");
    //electronbot_usb->ReadElectronbotUSB(buf,32);

}

void MainWindow::on_openFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,QStringLiteral("文件对话框！"),"F:",QStringLiteral("二进制文件(*bin);;""本本文件(*txt)"));
    if(fileName.length() == 0)
    {
        QMessageBox::information(NULL, tr("Path"), tr("You didn't select any files."));
    }
    else
    {
        //QMessageBox::information(NULL, tr("Path"), tr("You selected ") + fileName);
        ui->lineEdit_path->setText(fileName);
    }


}

void MainWindow::on_sendFile_clicked()
{
//    uint8_t buf[512];
//    int i;
    QString fileName=ui->lineEdit_path->text();
    if(pDownLoadFile)
        delete pDownLoadFile;
    pDownLoadFile = new downLoadFile(electronbot_usb);
    pDownLoadFile->seFilename(fileName);
    pDownLoadFile->NoticeAppIntoBootLoader(txData);
    QFile file(fileName);//与文件建立联系
    if(!file.exists())//判断是否建立成功
    {

        QString str = "world";
        //qDebug()<<"hello "<<str<<"!"<<endl;

    }
    else
    {
        //this->showMaximized();//成功则窗口会最大化，这只是我用检测的方法
        qDebug("read file\r\n");
        qint64 size=file.size();
        qDebug("size=%ld\r\n",size);
        int totalPackets = size%SIZE_PER?(size/SIZE_PER+1):(size/SIZE_PER);
        qDebug("totalPackets=%d\r\n",totalPackets);
        ui->progressBar->setMinimum(0);
        ui->progressBar->setMaximum(totalPackets);
        ui->progressBar->setValue(0);
//        if(file.open(QIODevice::ReadOnly|QIODevice::Truncate))//打开文件，以只读的方式打开文本文件
//        {
//            /*QDateTime time= QDateTime::currentDateTime();//获取系统当前的时间
//            uint sTime = time.toTime_t();
//            qDebug("sTime=%d\r\n",sTime);*/
//            for(i=0;i<totalPackets;i++)
//            {
//                qint32 n=file.read((char*)buf,512);
//                electronbot_usb->WriteElectronbotUSB(buf,512);
//            }
//            /*time= QDateTime::currentDateTime();//获取系统当前的时间
//            uint eTime = time.toTime_t();
//            qDebug("op time=%d\r\n",eTime-sTime);*/


//            //因为readline函数读取文件内容成功的话就会返回文件的字节数，如果失败就会返回-1
//            /*qDebug("len=%d\r\n",n);
//            if(n!=-1)
//            {

//                for(i=0;i<n;i++)
//                    qDebug("%02x",buf[i]);
//                qDebug("\r\n");

//            }*/
//            file.close();
//         }
//       else
//       {
//            qDebug()<<file.errorString();
//       }
    }


}


void MainWindow::initPara(void)
{
    //head
    ui->lineEdit_head_kp->setText("30");
    ui->lineEdit_head_ki->setText("0.4");
    ui->lineEdit_head_kv->setText("0");
    ui->lineEdit_head_kd->setText("200");
    ui->lineEdit_head_tq->setText("0.5");
    //left roll
    ui->lineEdit_lroll_kp->setText("50");
    ui->lineEdit_lroll_ki->setText("0.8");
    ui->lineEdit_lroll_kv->setText("0");
    ui->lineEdit_lroll_kd->setText("600");
    ui->lineEdit_lroll_tq->setText("1.0");
    //left pitch
    ui->lineEdit_lpitch_kp->setText("50");
    ui->lineEdit_lpitch_ki->setText("0.8");
    ui->lineEdit_lpitch_kv->setText("0");
    ui->lineEdit_lpitch_kd->setText("300");
    ui->lineEdit_lpitch_tq->setText("0.5");
    //right roll
    ui->lineEdit_rroll_kp->setText("50");
    ui->lineEdit_rroll_ki->setText("0.8");
    ui->lineEdit_rroll_kv->setText("0");
    ui->lineEdit_rroll_kd->setText("600");
    ui->lineEdit_rroll_tq->setText("1.0");
    //right pitch
    ui->lineEdit_rpitch_kp->setText("50");
    ui->lineEdit_rpitch_ki->setText("0.8");
    ui->lineEdit_rpitch_kv->setText("0");
    ui->lineEdit_rpitch_kd->setText("300");
    ui->lineEdit_rpitch_tq->setText("0.5");
    //body
    ui->lineEdit_body_kp->setText("150");
    ui->lineEdit_body_ki->setText("0.8");
    ui->lineEdit_body_kv->setText("0");
    ui->lineEdit_body_kd->setText("300");
    ui->lineEdit_body_tq->setText("0.5");
}
//  id  更新标志  kp  更新标志  ki   更新标志   kv    更新标志  kd    更新标志   tq    更新标志   使能     cmd
//   0   1      2~5  6      7~10     11    12~15   16    17~20    21    22~25     26      27      31
void MainWindow::on_pushButton_body_clicked()
{
    float setKp=ui->lineEdit_body_kp->text().toFloat();
    float setKi=ui->lineEdit_body_ki->text().toFloat();
    float setKv=ui->lineEdit_body_kv->text().toFloat();
    float setKd=ui->lineEdit_body_kd->text().toFloat();
    float setTq=ui->lineEdit_body_tq->text().toFloat();

//    qDebug("setPoint=%f",setPoint);
    txData[PAR_INDEX+31] = 0xee; //设置参数
    txData[PAR_INDEX] = 12;
//    txData[PAR_INDEX+1] = 0x01;
    txData[PAR_INDEX+1]=(ui->checkBox_body_kp->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+2] = setKp;

    txData[PAR_INDEX+6]=(ui->checkBox_body_ki->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+7] = setKi;

    txData[PAR_INDEX+11]=(ui->checkBox_body_kv->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+12] = setKv;

    txData[PAR_INDEX+16]=(ui->checkBox_body_kd->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+17] = setKd;

    txData[PAR_INDEX+21]=(ui->checkBox_body_tq->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+22] = setTq;
    txData[PAR_INDEX+26]=0x00;
    txData[PAR_INDEX+27]=0x00;
    sendPacket();
}


void MainWindow::on_pushButton_rpitch_clicked()
{
    float setKp=ui->lineEdit_rpitch_kp->text().toFloat();
    float setKi=ui->lineEdit_rpitch_ki->text().toFloat();
    float setKv=ui->lineEdit_rpitch_kv->text().toFloat();
    float setKd=ui->lineEdit_rpitch_kd->text().toFloat();
    float setTq=ui->lineEdit_rpitch_tq->text().toFloat();

//    qDebug("setPoint=%f",setPoint);
    txData[PAR_INDEX+31] = 0xee; //设置参数
    txData[PAR_INDEX] = 10;
//    txData[PAR_INDEX+1] = 0x01;
    txData[PAR_INDEX+1]=(ui->checkBox_rpitch_kp->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+2] = setKp;

    txData[PAR_INDEX+6]=(ui->checkBox_rpitch_ki->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+7] = setKi;

    txData[PAR_INDEX+11]=(ui->checkBox_rpitch_kv->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+12] = setKv;

    txData[PAR_INDEX+16]=(ui->checkBox_rpitch_kd->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+17] = setKd;

    txData[PAR_INDEX+21]=(ui->checkBox_rpitch_tq->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+22] = setTq;
    txData[PAR_INDEX+26]=0x00;
    txData[PAR_INDEX+27]=0x00;
    sendPacket();

}

void MainWindow::on_pushButton_rroll_clicked()
{
    float setKp=ui->lineEdit_rroll_kp->text().toFloat();
    float setKi=ui->lineEdit_rroll_ki->text().toFloat();
    float setKv=ui->lineEdit_rroll_kv->text().toFloat();
    float setKd=ui->lineEdit_rroll_kd->text().toFloat();
    float setTq=ui->lineEdit_rroll_tq->text().toFloat();

//    qDebug("setPoint=%f",setPoint);
    txData[PAR_INDEX+31] = 0xee; //设置参数
    txData[PAR_INDEX] = 8;
//    txData[PAR_INDEX+1] = 0x01;
    txData[PAR_INDEX+1]=(ui->checkBox_rroll_kp->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+2] = setKp;

    txData[PAR_INDEX+6]=(ui->checkBox_rroll_ki->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+7] = setKi;

    txData[PAR_INDEX+11]=(ui->checkBox_rroll_kv->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+12] = setKv;

    txData[PAR_INDEX+16]=(ui->checkBox_rroll_kd->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+17] = setKd;

    txData[PAR_INDEX+21]=(ui->checkBox_rroll_tq->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+22] = setTq;
    txData[PAR_INDEX+26]=0x00;
    txData[PAR_INDEX+27]=0x00;
    sendPacket();

}

void MainWindow::on_pushButton_lpitch_clicked()
{
    float setKp=ui->lineEdit_lpitch_kp->text().toFloat();
    float setKi=ui->lineEdit_lpitch_ki->text().toFloat();
    float setKv=ui->lineEdit_lpitch_kv->text().toFloat();
    float setKd=ui->lineEdit_lpitch_kd->text().toFloat();
    float setTq=ui->lineEdit_lpitch_tq->text().toFloat();

//    qDebug("setPoint=%f",setPoint);
    txData[PAR_INDEX+31] = 0xee; //设置参数
    txData[PAR_INDEX] = 6;
//    txData[PAR_INDEX+1] = 0x01;
    txData[PAR_INDEX+1]=(ui->checkBox_lpitch_kp->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+2] = setKp;

    txData[PAR_INDEX+6]=(ui->checkBox_lpitch_ki->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+7] = setKi;

    txData[PAR_INDEX+11]=(ui->checkBox_lpitch_kv->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+12] = setKv;

    txData[PAR_INDEX+16]=(ui->checkBox_lpitch_kd->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+17] = setKd;

    txData[PAR_INDEX+21]=(ui->checkBox_lpitch_tq->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+22] = setTq;
    txData[PAR_INDEX+26]=0x00;
    txData[PAR_INDEX+27]=0x00;
    sendPacket();

}

void MainWindow::on_pushButton_lroll_clicked()
{
    float setKp=ui->lineEdit_lroll_kp->text().toFloat();
    float setKi=ui->lineEdit_lroll_ki->text().toFloat();
    float setKv=ui->lineEdit_lroll_kv->text().toFloat();
    float setKd=ui->lineEdit_lroll_kd->text().toFloat();
    float setTq=ui->lineEdit_lroll_tq->text().toFloat();

//    qDebug("setPoint=%f",setPoint);
    txData[PAR_INDEX+31] = 0xee; //设置参数
    txData[PAR_INDEX] = 4;
//    txData[PAR_INDEX+1] = 0x01;
    txData[PAR_INDEX+1]=(ui->checkBox_lroll_kp->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+2] = setKp;

    txData[PAR_INDEX+6]=(ui->checkBox_lroll_ki->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+7] = setKi;

    txData[PAR_INDEX+11]=(ui->checkBox_lroll_kv->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+12] = setKv;

    txData[PAR_INDEX+16]=(ui->checkBox_lroll_kd->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+17] = setKd;

    txData[PAR_INDEX+21]=(ui->checkBox_lroll_tq->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+22] = setTq;
    txData[PAR_INDEX+26]=0x00;
    txData[PAR_INDEX+27]=0x00;
    sendPacket();

}

void MainWindow::on_pushButton_head_clicked()
{
    float setKp=ui->lineEdit_head_kp->text().toFloat();
    float setKi=ui->lineEdit_head_ki->text().toFloat();
    float setKv=ui->lineEdit_head_kv->text().toFloat();
    float setKd=ui->lineEdit_head_kd->text().toFloat();
    float setTq=ui->lineEdit_head_tq->text().toFloat();

//    qDebug("setPoint=%f",setPoint);
    txData[PAR_INDEX+31] = 0xee; //设置参数
    txData[PAR_INDEX] = 2;
//    txData[PAR_INDEX+1] = 0x01;
    txData[PAR_INDEX+1]=(ui->checkBox_head_kp->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+2] = setKp;

    txData[PAR_INDEX+6]=(ui->checkBox_head_ki->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+7] = setKi;

    txData[PAR_INDEX+11]=(ui->checkBox_head_kv->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+12] = setKv;

    txData[PAR_INDEX+16]=(ui->checkBox_head_kd->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+17] = setKd;

    txData[PAR_INDEX+21]=(ui->checkBox_head_tq->checkState()==Qt::Checked)?1:0;
    *(float*)&txData[PAR_INDEX+22] = setTq;
    txData[PAR_INDEX+26]=0x00;
    txData[PAR_INDEX+27]=0x00;
    sendPacket();

}
//  id  angle  使能     cmd
//   0   1~4    5       31
void MainWindow::on_pushButton_sendAngle_clicked()
{
    int id = ui->lineEdit_id->text().toInt();
    float angle = ui->lineEdit_angle->text().toFloat();

    txData[PAR_INDEX+31] = 0xef; //设置角度
    txData[PAR_INDEX] = id;
    *(float*)&txData[PAR_INDEX+1] = angle;
    txData[PAR_INDEX+5]=(ui->checkBox_enable->checkState()==Qt::Checked)?1:0;
    sendPacket();

}
//  oldid  newid      cmd
//   0      1          31
void MainWindow::on_pushButton_set_id_clicked()
{
    int oldid = ui->lineEdit_id_old->text().toInt();
    int newid = ui->lineEdit_id_new->text().toInt();


    txData[PAR_INDEX+31] = 0xf0; //设置角度
    txData[PAR_INDEX] = oldid;
    txData[PAR_INDEX+1] = newid;
    sendPacket();

}
//  id  init_angle   cmd
//   0       1~4      31
void MainWindow::on_pushButton_set_init_angle_clicked()
{
    int id = ui->lineEdit_id_2->text().toInt();
    float angle = ui->lineEdit_id_init_angle->text().toFloat();

    txData[PAR_INDEX+31] = 0xf1; //设置初始角度
    txData[PAR_INDEX] = id;
    *(float*)&txData[PAR_INDEX+1] = angle;
    sendPacket();

}
