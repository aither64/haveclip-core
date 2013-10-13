#-------------------------------------------------
#
# Project created by QtCreator 2013-05-31T16:06:06
#
#-------------------------------------------------

QT       += core gui network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bin/HaveClip
TEMPLATE = app

SOURCES +=\
    src/HaveClip.cpp \
    src/AboutDialog.cpp \
    src/SettingsDialog.cpp \
    src/Sender.cpp \
    src/Receiver.cpp \
    src/CertificateTrustDialog.cpp \
    src/PasteServices/BasePasteService.cpp \
    src/PasteServices/Stikked/Stikked.cpp \
    src/PasteServices/PasteDialog.cpp \
    src/PasteServices/BasePasteServiceWidget.cpp \
    src/PasteServices/Stikked/StikkedSettings.cpp \
    src/PasteServices/Pastebin/PastebinSettings.cpp \
    src/PasteServices/Pastebin/Pastebin.cpp \
    src/LoginDialog.cpp \
    src/Main.cpp \
    src/PasteServices/PasteServiceEditDialog.cpp \
    src/ClipboardManager.cpp \
    src/ClipboardItem.cpp \
    src/ClipboardSerialBatch.cpp \
    src/History.cpp \
    src/ClipboardContainer.cpp

HEADERS  += src/HaveClip.h \
    src/AboutDialog.h \
    src/SettingsDialog.h \
    src/Sender.h \
    src/Receiver.h \
    src/CertificateTrustDialog.h \
    src/PasteServices/BasePasteService.h \
    src/PasteServices/Stikked/Stikked.h \
    src/PasteServices/PasteDialog.h \
    src/PasteServices/BasePasteServiceWidget.h \
    src/PasteServices/Stikked/StikkedSettings.h \
    src/PasteServices/Pastebin/PastebinSettings.h \
    src/PasteServices/Pastebin/Pastebin.h \
    src/LoginDialog.h \
    src/PasteServices/PasteServiceEditDialog.h \
    src/ClipboardManager.h \
    src/ClipboardItem.h \
    src/ClipboardSerialBatch.h \
    src/History.h \
    src/ClipboardContainer.h

FORMS    += src/AboutDialog.ui \
    src/SettingsDialog.ui \
    src/CertificateTrustDialog.ui \
    src/PasteServices/Stikked/StikkedSettings.ui \
    src/PasteServices/PasteDialog.ui \
    src/PasteServices/Pastebin/PastebinSettings.ui \
    src/LoginDialog.ui \
    src/PasteServices/PasteServiceEditDialog.ui

RESOURCES += \
    HaveClip.qrc

OTHER_FILES += \
    TODO \
    utils/stikked_lang_generator.php \
    utils/pastebin_lang_generator.py \
    LICENSE \
    README.md \
    src/HaveClip.rc \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/qtproject/qt5/android/bindings/QtApplication.java \
    android/src/org/qtproject/qt5/android/bindings/QtActivity.java \
    android/res/values-pl/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/layout/splash.xml \
    android/res/values-de/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-nl/strings.xml \
    android/version.xml \
    android/AndroidManifest.xml \
    CHANGELOG

win32:RC_FILE = src/HaveClip.rc

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += x11
