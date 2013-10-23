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
    src/Network/Sender.cpp \
    src/Network/Receiver.cpp \
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
    src/ClipboardContainer.cpp \
    src/Network/Conversation.cpp \
    src/Network/Conversations/ClipboardUpdate.cpp \
    src/Network/Commands/ClipboardUpdateReady.cpp \
    src/Network/Commands/ClipboardUpdateSend.cpp \
    src/Network/Communicator.cpp \
    src/Network/Commands/ClipboardUpdateConfirm.cpp \
    src/Network/Commands/Confirm.cpp \
    src/Network/Command.cpp \
    src/Network/Conversations/SerialModeBegin.cpp \
    src/Network/Conversations/SerialModeEnd.cpp \
    src/Network/Conversations/SerialModeBase.cpp \
    src/Network/Commands/Cmd_SerialModeBase.cpp \
    src/Network/Commands/SerialModeToggle.cpp \
    src/Network/Conversations/SerialModeAppend.cpp \
    src/Network/Commands/SerialModeAppendReady.cpp \
    src/Network/Conversations/SerialModeCopy.cpp \
    src/Network/Commands/SerialModeInfo.cpp \
    src/Network/Conversations/HistoryMixin.cpp \
    src/Network/Conversations/SerialModeNext.cpp

HEADERS  += src/HaveClip.h \
    src/AboutDialog.h \
    src/SettingsDialog.h \
    src/Network/Sender.h \
    src/Network/Receiver.h \
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
    src/ClipboardContainer.h \
    src/Network/Conversation.h \
    src/Network/Conversations/ClipboardUpdate.h \
    src/Network/Commands/ClipboardUpdateReady.h \
    src/Network/Commands/ClipboardUpdateSend.h \
    src/Network/Communicator.h \
    src/Network/Commands/ClipboardUpdateConfirm.h \
    src/Network/Commands/Confirm.h \
    src/Network/Command.h \
    src/Network/Conversations/SerialModeBegin.h \
    src/Network/Conversations/SerialModeEnd.h \
    src/Network/Conversations/SerialModeBase.h \
    src/Network/Commands/Cmd_SerialModeBase.h \
    src/Network/Commands/SerialModeToggle.h \
    src/Network/Conversations/SerialModeAppend.h \
    src/Network/Commands/SerialModeAppendReady.h \
    src/Network/Conversations/SerialModeCopy.h \
    src/Network/Commands/SerialModeInfo.h \
    src/Network/Conversations/HistoryMixin.h \
    src/Network/Conversations/SerialModeNext.h

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
    CHANGELOG \
    doc/protocol.md

win32:RC_FILE = src/HaveClip.rc

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += x11
