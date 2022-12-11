#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commusb.h"
#include "QFileDialog"
#include <QMessageBox>
#include<QTextEdit>
#include<QFile>
#include<QtDebug>
#include <QDateTime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    recvUSB_Timer = new QTimer(this);
    //connect(recvUSB_Timer, SIGNAL(timeout()), this, SLOT(RecvUSBTask()));
    //recvUSB_Timer->start(50);
    electronbot_usb = new commUSB;
    //electronbot_usb->print_dev();
    electronbot_usb->openElectronbotUSB(1155,22336);


}

MainWindow::~MainWindow()
{
    delete electronbot_usb;
    delete recvUSB_Timer;
    delete ui;
}
void MainWindow::reconnectElectronbotUSB(void)
{
    electronbot_usb->CloseElectronbotUSB();
    delete electronbot_usb;
    electronbot_usb = new commUSB;
    electronbot_usb->openElectronbotUSB(0x120f,0x0333);

}
void MainWindow::on_pushButton_clicked()
{
    int ret;
    uint8_t buf[512];
    buf[0] = 0x12;
    buf[1] = 0x13;
    buf[2] = 0x14;
    buf[3] = 0x15;
    ret=electronbot_usb->WriteElectronbotUSB(buf,512);
    if(ret<0)
    {
        qDebug("reconnect!");
        reconnectElectronbotUSB();
    }
    //usb->test_my_usb_devices(0x1209,0x0d32);
}
void MainWindow::RecvUSBTask()
{
    uint8_t buf[100];
    //qDebug("hello timer!");
    electronbot_usb->ReadElectronbotUSB(buf,64);

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
        ui->pathTextEdit->setText(fileName);
    }


}

void MainWindow::on_sendFile_clicked()
{
    uint8_t buf[512];
    int i;
    QString fileName=ui->pathTextEdit->toPlainText();
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
        fflush(stdout);
        qint64 size=file.size();
        qDebug("size=%ld\r\n",size);
        int totalPackets = size%512?(size/512+1):(size/512);
        qDebug("totalPackets=%d\r\n",totalPackets);
        if(file.open(QIODevice::ReadOnly|QIODevice::Truncate))//打开文件，以只读的方式打开文本文件
        {
            /*QDateTime time= QDateTime::currentDateTime();//获取系统当前的时间
            uint sTime = time.toTime_t();
            qDebug("sTime=%d\r\n",sTime);*/
            for(i=0;i<totalPackets;i++)
            {
                qint32 n=file.read((char*)buf,512);
                electronbot_usb->WriteElectronbotUSB(buf,512);
            }
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
            file.close();
         }
       else
       {
            qDebug()<<file.errorString();
       }
    }


}
