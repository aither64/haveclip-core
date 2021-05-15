QT       += core gui network

TARGET = haveclipcore
TEMPLATE = lib
CONFIG += staticlib create_prl
# Note: add `CONFIG += link_prl` to an app:
#       https://doc.qt.io/qt-5.12/qmake-advanced-usage.html#library-dependencies

packagesExist(sailfishapp) {
	DEFINES += MER_SAILFISH
}

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
    src/CertificateGeneratorThread.cpp \
    src/Helpers/qmlclipboardmanager.cpp \
    src/Helpers/qmlhelpers.cpp \
    src/Helpers/qmlnode.cpp \
    src/ConfigMigrations/V3Migration.cpp

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
    src/CertificateGeneratorThread.h \
    src/Helpers/qmlclipboardmanager.h \
    src/Helpers/qmlhelpers.h \
    src/Helpers/qmlnode.h \
    src/ConfigMigrations/V3Migration.h

mac {
    CONFIG += objective_c

    # https://doc.qt.io/qt-5.12/qmake-variable-reference.html#objective-sources   #objective-headers
    # OBJECTIVE_SOURCES is obsolete since Qt5.6 (QTBUG-36575)
    # - https://codereview.qt-project.org/c/qt/qtbase/+/77117/  (Branch 5.6, "INCLUDED IN" button)
    # - before Qt5.6 it only shows the warning:
    #   - https://github.com/qt/qtbase/commit/9ff1310af51814e521572fa3de4e086907633a90#diff-b42d489993899e48fa93c92b344e9201
    #   - https://codereview.qt-project.org/c/qt/qtbase/+/77117/13/mkspecs/features/mac/objective_c.prf#b4
    #
    SOURCES           += src/darwin/AppNapPreventingActivity.mm
    OBJECTIVE_HEADERS += src/darwin/AppNapPreventingActivity.h

    LIBS += -framework Foundation
}

OTHER_FILES += \
    TODO \
    LICENSE \
    README.md \
    doc/protocol.md \
    CHANGELOG


# For dead code stripping:
#  Place each function/data in its own section,
#  so they can be discarded if unused in an app.
CONFIG(release, debug|release):!mac: CONFIG += gc_binaries


#================== Libs ==================
# How to set library paths (OpenSSL)?
# Several ways (by priority) to do this (with examples):
# 1. In "Projects" mode [ctrl+5] >
#        Build & Run > Build >
#         Build Settings (Edit build configuration: "Debug", "Profile", "Release") >
#          Build Step: qmake / Additional arguments:
#          ( https://doc.qt.io/qtcreator/creator-build-settings.html#qmake-build-steps )
#          - OPENSSL_ROOT="/usr/local/opt/openssl"
#          - OPENSSL_ROOT="" INCLUDEPATH+="/usr/local/opt/openssl/include" LIBS+="-L/usr/local/opt/openssl/lib -lcrypto"
#            (you can specify OPENSSL_ROOT empty to manually set INCLUDEPATH and LIBS)
# 2. Create file "libs.pri" and specify the paths to all libraries in it.
#    (see example below)
#    Note[for Git]: "libs.pri" is included in ".gitignore".
# 3. Add "pkgconfig" path (macOS, Homebrew: "/usr/local/bin") to PATH environment variable:
#    https://doc.qt.io/qtcreator/creator-project-settings-environment.html
#    ("Append Path..." [Finder: Shift+⌘+G] or "Edit")
#    or directly set a path to the "pkgconfig" through "Additional arguments" (see way 1)
#       PKG_CONFIG=/usr/local/bin/pkg-config
#       ( https://stackoverflow.com/questions/16972066/using-pkg-config-with-qt-creator-qmake-on-mac-osx/51602580#51602580 )
# Note: "by priority" means that you can set the paths, for example, in (2) and (1), and (1) will shadow (2).
# Note: if you prefer to use CPLUS_INCLUDE_PATH (CPATH, ...) and LIBRARY_PATH environment variables
#       instead of INCLUDEPATH and LIBS qmake variables
#       then specify OPENSSL_ROOT="" and LIBS+="-lcrypto" by (1) or (2) way.

exists(libs.pri):include(libs.pri)
# "libs.pri" file contents example:
#  !defined(OPENSSL_ROOT, var): OPENSSL_ROOT="/usr/local/opt/openssl"
# or
#  !defined(OPENSSL_ROOT, var) {
#      OPENSSL_ROOT=""
#      # https://github.com/openssl/openssl/blob/OpenSSL_1_1_1/NOTES.UNIX
#      INCLUDEPATH += /usr/local/opt/openssl/include
#      # for application (haveclip-desktop.pro, ...)
#      # https://doc.qt.io/qt-5.12/qmake-advanced-usage.html#library-dependencies
#      LIBS += -L/usr/local/opt/openssl/lib -lcrypto
#  }
# or
#  !defined(OPENSSL_ROOT, var) {
#      # for use with CPATH (CPLUS_INCLUDE_PATH, ...) and LIBRARY_PATH environment variables
#      OPENSSL_ROOT=""
#      # for application (haveclip-desktop.pro, ...)
#      # https://doc.qt.io/qt-5.12/qmake-advanced-usage.html#library-dependencies
#      LIBS += -lcrypto
#  }

defined(OPENSSL_ROOT, var) {
    !isEmpty(OPENSSL_ROOT) {
        INCLUDEPATH += $$OPENSSL_ROOT/include
        LIBS += -L$$OPENSSL_ROOT/lib -lcrypto
    }
} else:unix {
    CONFIG *= link_pkgconfig
    PKGCONFIG += openssl
}

# Note[darwin, macOS, Homebrew]: Where is OpenSSL located?
# See `brew info openssl` output in Terminal:
#   openssl@1.1 is keg-only, which means it was not symlinked into /usr/local,
#   because macOS provides LibreSSL.
#
#   For compilers to find openssl@1.1 you may need to set:
#    export LDFLAGS="-L/usr/local/opt/openssl@1.1/lib"
#    export CPPFLAGS="-I/usr/local/opt/openssl@1.1/include"
#
# Don't `ln -s`!: https://medium.com/@timmykko/using-openssl-library-with-macos-sierra-7807cfd47892
#                 https://stackoverflow.com/questions/38670295/homebrew-refusing-to-link-openssl#comment64722608_38670295

unix:!darwin:!packagesExist(sailfishapp) {
    CONFIG *= link_pkgconfig
    PKGCONFIG += x11
         LIBS += -lX11
}

GITVERSION = $$PWD/src/git_version.h
versiontarget.target = $$GITVERSION
versiontarget.commands = $$PWD/utils/git_version.sh \"$$PWD/../\" \"$$GITVERSION\"
versiontarget.depends = FORCE
PRE_TARGETDEPS += $$GITVERSION
QMAKE_EXTRA_TARGETS += versiontarget
