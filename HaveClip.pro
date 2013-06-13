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
    src/AboutDialog.cpp \
    src/SettingsDialog.cpp \
    src/ClipboardContent.cpp \
    src/Sender.cpp \
    src/Receiver.cpp \
    src/CertificateTrustDialog.cpp

HEADERS  += src/HaveClip.h \
    src/AboutDialog.h \
    src/SettingsDialog.h \
    src/ClipboardContent.h \
    src/Sender.h \
    src/Receiver.h \
    src/CertificateTrustDialog.h

FORMS    += src/AboutDialog.ui \
    src/SettingsDialog.ui \
    src/CertificateTrustDialog.ui

RESOURCES += \
    HaveClip.qrc

OTHER_FILES += \
    README \
    TODO
