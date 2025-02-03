#-------------------------------------------------
#
# Project created by QtCreator 2016-03-09T15:05:22
#
#-------------------------------------------------

QT       += core gui widgets network

TARGET = Wohlnet_SendFile_oneday_Qt
mac: TARGET = "WohlNet Sender"
TEMPLATE = app
CONFIG += c++11

mac: CONFIG += sdk_no_version_check

DESTDIR = $$PWD/bin
mac: DESTDIR = /Applications

SOURCES += \
    main.cpp\
    wohlnet_sendfile_window.cpp

mac: SOURCES += sender_app.cpp

QMAKE_INFO_PLIST = $$PWD/Info.plist

HEADERS  += \
    wohlnet_sendfile_window.h \
    defines.h \
    sender_app.h

FORMS    += wohlnet_sendfile_window.ui
