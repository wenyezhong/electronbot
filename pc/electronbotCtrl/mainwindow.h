#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTimer>
#include "commusb.h"
#include "thread.h"
namespace Ui {
class MainWindow;
}
#define BUFF_SIZE  43232
#define PAR_INDEX  43200
#define ONCE_SIZE  512

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

private:
    Ui::MainWindow *ui;     
     QTimer *recvUSB_Timer;
     Thread *readUsbTread;
     uint8_t txData[60 * 240 * 3 + 32];
     void initPara(void);
public:
    commUSB *electronbot_usb;
    void sendPacket(void);
};

#endif // MAINWINDOW_H
