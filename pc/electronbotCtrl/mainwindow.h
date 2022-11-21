#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTimer>
#include "commusb.h"
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

private:
    Ui::MainWindow *ui;
     commUSB *electronbot_usb;
     QTimer *recvUSB_Timer;
};

#endif // MAINWINDOW_H
