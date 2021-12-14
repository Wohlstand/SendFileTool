#-------------------------------------------------
#
# Project created by QtCreator 2016-03-09T15:05:22
#
#-------------------------------------------------

QT       += core gui widgets network

TARGET = Wohlnet_SendFile_oneday_Qt
TEMPLATE = app
CONFIG += c++11

DESTDIR = $$PWD/bin

SOURCES += main.cpp\
        wohlnet_sendfile_window.cpp

HEADERS  += \
    wohlnet_sendfile_window.h \
    defines.h

FORMS    += wohlnet_sendfile_window.ui
