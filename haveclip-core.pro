QT       += core gui network

TARGET = bin/haveclipcore
TEMPLATE = lib

SOURCES +=\
    src/Network/Sender.cpp \
    src/Network/Receiver.cpp \
    src/PasteServices/BasePasteService.cpp \
    src/PasteServices/Stikked/Stikked.cpp \
    src/PasteServices/Pastebin/Pastebin.cpp \
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
    src/Network/Conversations/SerialModeNext.cpp \
    src/Network/Conversations/SerialModeRestart.cpp \
    src/PasteServices/HaveSnippet/HaveSnippet.cpp

HEADERS  += \
    src/Network/Sender.h \
    src/Network/Receiver.h \
    src/PasteServices/BasePasteService.h \
    src/PasteServices/Stikked/Stikked.h \
    src/PasteServices/Pastebin/Pastebin.h \
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
    src/Network/Conversations/SerialModeNext.h \
    src/Network/Conversations/SerialModeRestart.h \
    src/PasteServices/HaveSnippet/HaveSnippet.h

OTHER_FILES += \
    TODO \
    utils/stikked_lang_generator.php \
    utils/pastebin_lang_generator.py \
    LICENSE \
    README.md \
    doc/protocol.md \
    CHANGELOG

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += x11
