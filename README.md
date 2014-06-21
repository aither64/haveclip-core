HaveClip - a simple clipboard synchronization tool
==================================================

Have you ever grown tired of thinking how to transfer a link or a text from one
computer to another? Will you email yourself, use 3rd party service like
Pastebin, or will you copy it by hand?

HaveClip is the solution. Ctrl+C on one computer, Ctrl+V on another. Simple.

Clipboard is instantly synchronized across all your devices. It can handle all
forms of text, images and even application-specific data.

HaveClip is multiplatform. Copy something in one OS, paste in another.
Supports Windows, Linux, Mac OS X and Sailfish OS.

It is not possible to copy files over network with HaveClip.

Requirements
------------
HaveClip for desktop is written in C++ using the Qt framework.

 - Qt at least 4.7. It might work with older versions, but it's not tested.
   - modules core, gui (widgets for Qt5) and network
 - Linux version has additional dependencies
   - X11 header files

### Debian based distributions

    # apt-get install g++ libqt4-dev libqt4-dev-bin libX11-dev

### Fedora and other RPM based distributions

    # yum install gcc-c++ qt-devel xorg-x11-proto-devel

Download
--------
[https://sourceforge.net/projects/haveclip/files/](https://sourceforge.net/projects/haveclip/files/)

Build
-----
    $ tar -xzf haveclip-desktop-0.13.0-src.tar.gz
    $ cd haveclip-desktop-0.13.0-src
    $ qmake (or qmake-qt4, qmake-qt5)
    $ make

You should now have executable at bin/haveclip and a library at haveclip-core/bin/libhaveclipcore.so.1.

Installation
------------
    $ sudo make install

or as root

    # make install

Usage
-----
HaveClip runs in background. There is no window when you launch it, only tray
icon will show up.

If you left-click the tray icon, menu with clipboard history will pop up.
Right-click triggers menu with options and pasting. These two menus are merged
in one in OS X.

First thing you want to do is to setup devices where the clipboard will be
synced. Right-click on the tray icon and select Settings. Go to tab Pool
and add IP addresses of your devices. The format for each entry is
IP address:port. For example:

    1.2.3.4:9999

Now when you copy something into clipboard, it will be sent to all devices
listed in Pool.

It is not recommended to use HaveClip over slow or network connections with FUP,
since it can transfer a lot of data. For instance, image copied from GIMP
is saved to clipboard in multiple formats, so the size can exceed a few
tens of MB.

Sailfish OS
-----------
Sailfish version does not support encryption.

In order to sync clipboard from Sailfish to PC, one must bring HaveClip to foreground
after copying text, or else HaveClip will not notice it. Sync from PC to Sailfish works
fine.

For planned features see TODO.

License
-------
HaveClip is released under GNU/GPL.

WARNING
-------
The clipboard is definitely a private thing, so be careful where and how you use
HaveClip. Be sure to use SSL/TLS encryption, generate your own certificate
and use access password.
