#-------------------------------------------------
#
# Project created by QtCreator 2022-11-20T10:19:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = electronbotCtrl
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    commusb.cpp \
    thread.cpp

HEADERS += \
        mainwindow.h \
    commusb.h \
    thread.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libusb-1.0.26-binaries/libusb-MinGW-x64/lib/ -llibusb-1.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libusb-1.0.26-binaries/libusb-MinGW-x64/lib/ -llibusb-1.0
else:unix:!macx: LIBS += -L$$PWD/../libusb-1.0.26-binaries/libusb-MinGW-x64/lib/ -llibusb-1.0

INCLUDEPATH += $$PWD/../libusb-1.0.26-binaries/libusb-MinGW-x64/include/libusb-1.0
DEPENDPATH += $$PWD/../libusb-1.0.26-binaries/libusb-MinGW-x64/include/libusb-1.0
