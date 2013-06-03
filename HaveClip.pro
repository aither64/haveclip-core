#-------------------------------------------------
#
# Project created by QtCreator 2013-05-31T16:06:06
#
#-------------------------------------------------

QT       += core gui network

TARGET = HaveClip
TEMPLATE = app
CONFIG  += qxt
QXT     += core gui

SOURCES +=\
    src/HaveClip.cpp \
    src/main.cpp \
    src/Distributor.cpp \
    src/Client.cpp \
    src/AboutDialog.cpp

HEADERS  += src/HaveClip.h \
    src/Distributor.h \
    src/Client.h \
    src/AboutDialog.h

FORMS    += mainwindow.ui \
    src/AboutDialog.ui

RESOURCES += \
    HaveClip.qrc
