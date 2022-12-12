#include "mainwindow.h"
#include <QApplication>

MainWindow *pWindows;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    pWindows = &w;
    w.show();

    return a.exec();
}
