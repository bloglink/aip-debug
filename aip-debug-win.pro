#-------------------------------------------------
#
# Project created by QtCreator 2016-09-01T09:13:11
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = aip-debug
TEMPLATE = app


SOURCES += main.cpp\
        CWinDebug.cpp

HEADERS  += CWinDebug.h

FORMS    += CWinDebug.ui

RESOURCES += \
    image.qrc

RC_FILE += ico.rc

