#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T09:13:11
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = aip-debug
TEMPLATE = app

RC_FILE += ico.rc

FORMS += \
    app/CWinDebug.ui

HEADERS += \
    app/CWinDebug.h

SOURCES += \
    app/aip-debug.cpp \
    app/CWinDebug.cpp

RESOURCES += \
    aip-image.qrc

