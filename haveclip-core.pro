QT       += core gui network

TARGET = haveclipcore
TEMPLATE = lib
CONFIG += staticlib

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
    src/CertificateGenerator.cpp \
    src/Node.cpp \
    src/Network/Conversations/Introduction.cpp \
    src/Network/Commands/Ping.cpp \
    src/Network/ConnectionManager.cpp \
    src/Network/Commands/Introduce.cpp \
    src/Network/Conversations/Verification.cpp \
    src/Network/Commands/SecurityCode.cpp \
    src/Network/AutoDiscovery.cpp \
    src/ConfigMigration.cpp \
    src/Settings.cpp \
    src/ConfigMigrations/V2Migration.cpp \
    src/CertificateInfo.cpp \
    src/RemoteControl.cpp \
    src/Cli.cpp \
    src/RemoteControls/Sync.cpp \
    src/RemoteClient.cpp \
    src/RemoteControls/RemoteBase.cpp \
    src/CertificateGeneratorThread.cpp

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
    src/CertificateGenerator.h \
    src/Node.h \
    src/Network/Conversations/Introduction.h \
    src/Network/Commands/Ping.h \
    src/Network/ConnectionManager.h \
    src/Network/Commands/Introduce.h \
    src/Network/Conversations/Verification.h \
    src/Network/Commands/SecurityCode.h \
    src/Network/AutoDiscovery.h \
    src/Version.h \
    src/ConfigMigration.h \
    src/Settings.h \
    src/ConfigMigrations/V2Migration.h \
    src/CertificateInfo.h \
    src/RemoteControl.h \
    src/Cli.h \
    src/RemoteControls/Sync.h \
    src/RemoteClient.h \
    src/RemoteControls/RemoteBase.h \
    src/CertificateGeneratorThread.h

OTHER_FILES += \
    TODO \
    LICENSE \
    README.md \
    doc/protocol.md \
    CHANGELOG

unix {
        CONFIG += link_pkgconfig
        PKGCONFIG += openssl

	!mac {
		PKGCONFIG += x11
                LIBS += -lX11
	}
}
