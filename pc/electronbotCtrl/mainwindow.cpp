#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commusb.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    usb = new commUSB;
    usb->print_dev();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    usb->test_my_usb_devices(0x1209,0x0d32);
}
