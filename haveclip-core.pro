QT       += core gui network

TARGET = bin/haveclipcore
TEMPLATE = lib

target.path = /usr/lib/
INSTALLS += target

SOURCES +=\
    src/Network/Sender.cpp \
    src/Network/Receiver.cpp \
    src/ClipboardManager.cpp \
    src/ClipboardItem.cpp \
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
    src/Network/Conversations/HistoryMixin.cpp \
    src/CertificateGenerator.cpp

HEADERS  += \
    src/Network/Sender.h \
    src/Network/Receiver.h \
    src/ClipboardManager.h \
    src/ClipboardItem.h \
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
    src/Network/Conversations/HistoryMixin.h \
    src/CertificateGenerator.h

OTHER_FILES += \
    TODO \
    LICENSE \
    README.md \
    doc/protocol.md \
    CHANGELOG

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += x11

unix|win32: LIBS += -lqca
