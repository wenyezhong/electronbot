#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTimer>
#include "commusb.h"
#include "thread.h"
#include "downloadfile.h"

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void reconnectElectronbotUSB(void);

private slots:
    void on_pushButton_clicked();
    void RecvUSBTask();
    void on_openFile_clicked();
    void on_sendFile_clicked();
    void recUsbDatas(BYTE* ptr);
//signals:
//    int sendRecDat(BYTE a[32]);
    void on_pushButton_body_clicked();
    void on_pushButton_sendAngle_clicked();

    void on_pushButton_rpitch_clicked();

    void on_pushButton_rroll_clicked();

    void on_pushButton_lpitch_clicked();

    void on_pushButton_lroll_clicked();

    void on_pushButton_head_clicked();

    void on_pushButton_set_id_clicked();

    void on_pushButton_set_init_angle_clicked();

    void on_pushButton_sendAngle_read_clicked();

    void on_horizontalSlider_RRoll_valueChanged(int value);

    void on_horizontalSlider_RPitch_valueChanged(int value);

    void on_horizontalSlider_RPitch_sliderReleased();

    void on_horizontalSlider_RRoll_sliderReleased();

private:
     Ui::MainWindow *ui;
     QTimer *recvUSB_Timer;
     Thread *readUsbTread;

     uint8_t txData[60 * 240 * 3 + 32];
     void initPara(void);
public:
     int readId=0xff;
    commUSB *electronbot_usb;
    downLoadFile *pDownLoadFile=nullptr;
    void sendPacket(void);
};

#endif // MAINWINDOW_H
