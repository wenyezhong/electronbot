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

private slots:
    void on_pushButton_clicked();
    void RecvUSBTask();
private:
    Ui::MainWindow *ui;
     commUSB *electronbot_usb;
     QTimer *recvUSB_Timer;
};

#endif // MAINWINDOW_H
