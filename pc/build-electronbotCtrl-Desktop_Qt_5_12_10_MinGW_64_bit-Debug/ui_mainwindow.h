/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton;
    QPushButton *openFile;
    QPushButton *sendFile;
    QTextEdit *pathTextEdit;
    QTabWidget *tabWidget;
    QWidget *tab;
    QPushButton *pushButton_body;
    QTextEdit *textEdit_setPoint;
    QWidget *tab_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(831, 609);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(520, 500, 75, 23));
        openFile = new QPushButton(centralWidget);
        openFile->setObjectName(QString::fromUtf8("openFile"));
        openFile->setGeometry(QRect(330, 500, 75, 23));
        sendFile = new QPushButton(centralWidget);
        sendFile->setObjectName(QString::fromUtf8("sendFile"));
        sendFile->setGeometry(QRect(420, 500, 75, 23));
        pathTextEdit = new QTextEdit(centralWidget);
        pathTextEdit->setObjectName(QString::fromUtf8("pathTextEdit"));
        pathTextEdit->setGeometry(QRect(10, 500, 301, 31));
        pathTextEdit->setInputMethodHints(Qt::ImhNone);
        pathTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        pathTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 0, 791, 471));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        pushButton_body = new QPushButton(tab);
        pushButton_body->setObjectName(QString::fromUtf8("pushButton_body"));
        pushButton_body->setGeometry(QRect(60, 230, 75, 23));
        textEdit_setPoint = new QTextEdit(tab);
        textEdit_setPoint->setObjectName(QString::fromUtf8("textEdit_setPoint"));
        textEdit_setPoint->setGeometry(QRect(60, 100, 121, 31));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 831, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton->setText(QApplication::translate("MainWindow", "testSend", nullptr));
        openFile->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266", nullptr));
        sendFile->setText(QApplication::translate("MainWindow", "\347\203\247\345\275\225", nullptr));
        pushButton_body->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256\350\272\253\344\275\223", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Tab 1", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Tab 2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
