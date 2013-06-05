#-------------------------------------------------
#
# Project created by QtCreator 2013-05-31T16:06:06
#
#-------------------------------------------------

QT       += core gui network xml

TARGET = HaveClip
TEMPLATE = app
CONFIG  += qxt
QXT     += core gui

SOURCES +=\
    src/HaveClip.cpp \
    src/main.cpp \
    src/Distributor.cpp \
    src/Client.cpp \
    src/AboutDialog.cpp \
    src/SettingsDialog.cpp \
    src/ClipboardContent.cpp

HEADERS  += src/HaveClip.h \
    src/Distributor.h \
    src/Client.h \
    src/AboutDialog.h \
    src/SettingsDialog.h \
    src/ClipboardContent.h

FORMS    += src/AboutDialog.ui \
    src/SettingsDialog.ui

RESOURCES += \
    HaveClip.qrc

OTHER_FILES += \
    README
