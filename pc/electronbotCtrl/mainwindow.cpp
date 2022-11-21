#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commusb.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    recvUSB_Timer = new QTimer(this);
    connect(recvUSB_Timer, SIGNAL(timeout()), this, SLOT(RecvUSBTask()));
    recvUSB_Timer->start(50);
    electronbot_usb = new commUSB;
    electronbot_usb->print_dev();
    electronbot_usb->openElectronbotUSB(0x1209,0x0d32);
}

MainWindow::~MainWindow()
{
    delete recvUSB_Timer;
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    uint8_t buf[100];
    buf[0] = 0x12;
    buf[1] = 0x13;
    buf[2] = 0x14;
    buf[3] = 0x15;
    electronbot_usb->WriteElectronbotUSB(buf,12);
    //usb->test_my_usb_devices(0x1209,0x0d32);
}
void MainWindow::RecvUSBTask()
{
    uint8_t buf[100];
    //qDebug("hello timer!");
    electronbot_usb->ReadElectronbotUSB(buf,64);

}
