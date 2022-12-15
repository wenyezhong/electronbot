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
    //electronbot_usb->print_dev();
    electronbot_usb->openElectronbotUSB(0x1214,0x0D3a);


    readUsbTread = new Thread;
    connect(readUsbTread,SIGNAL(sendRecDat(BYTE*)),this,SLOT(recUsbDatas(BYTE*)));
    readUsbTread->start();
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
    electronbot_usb->openElectronbotUSB(0x1214,0x0D3a);

}
void MainWindow::recUsbDatas(BYTE* ptr)
{
    int i;
    for(i=0;i<32;i++)
        qDebug("%x ",ptr[i]);
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
    }
    if(lastSize)
    {
        electronbot_usb->WriteElectronbotUSB(txData+ONCE_SIZE*sendcnt,lastSize);
    }

}
void MainWindow::on_pushButton_clicked()
{
    int ret;
    uint8_t buf[512];
    buf[0] = 0x12;
    buf[1] = 0x13;
    buf[2] = 0x14;
    buf[3] = 0x15;
//    electronbot_usb->ReadElectronbotUSB(buf,32);
    ret=electronbot_usb->WriteElectronbotUSB(buf,512);
    electronbot_usb->WriteElectronbotUSB(buf,512);
    electronbot_usb->WriteElectronbotUSB(buf,512);
    electronbot_usb->WriteElectronbotUSB(buf,224);
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
