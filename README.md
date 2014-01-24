HaveClip - a simple clipboard synchronization tool
==================================================

Have you ever grown tired of thinking how to transfer a link or a text from one
computer to another? Will you email yourself, use 3rd party service like
Pastebin, or will you copy it by hand?

HaveClip is the solution. Ctrl+C on one computer, Ctrl+V on another. Simple.

Main features are:

 - synchronize clipboard on multiple devices
 - save clipboard history, easily get overwritten data
 - integration with paste services like Pastebin.com
 - (Linux) [Serial batches](#serial-batches)

Clipboard is instantly synchronized across all your devices. It can handle all
forms of text, images and even application-specific data.

HaveClip is multiplatform. Copy something in one OS, paste in another.
Supports Windows, Linux, Mac OS X and Sailfish OS.

So far it is not possible to copy files over network with HaveClip. I would
like to implement it though.

Requirements
------------
HaveClip for desktop is written in C++ using the Qt framework.

 - Qt at least 4.7. It might work with older versions, but it's not tested.
   - modules core, gui (widgets for Qt5) and network
 - Linux version has additional dependencies
   - [LibQxt](http://www.libqxt.org/) 0.6
   - X11 header files

### Debian based distributions

    # apt-get install g++ libqt4-dev libqt4-dev-bin libX11-dev libqxt-dev

### Fedora and other RPM based distributions

    # yum install gcc-c++ qt-devel xorg-x11-proto-devel libqxt-devel

Download
--------
[https://sourceforge.net/projects/haveclip/files/](https://sourceforge.net/projects/haveclip/files/)

Build
-----
    $ tar -xzf haveclip-desktop-0.12.1-src.tar.gz
    $ cd haveclip-desktop-0.12.1-src
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

Features
--------
Where as the main functionality is present on all platforms,
not all other features are. The most equipped is the Linux version. It can
show you a preview of clipboard contents in tooltip of tray icon and you can use
serial batches. This is not possible to easily implement on other platforms.

HaveClip can be handy even if not used for clipboard syncing. It saves
clipboard history, allowing you to jump back and forth. It has integrated
support for paste services. With HaveClip you can easily create pastes from
the contents of clipboard. Support of specific application you work in is not
required.

Currently, HaveClip supports these paste services:

 - HaveSnippet - [http://www.havefun.cz/projects/havesnippet/](http://www.havefun.cz/projects/havesnippet/)
 - Stikked - [https://github.com/claudehohl/Stikked](https://github.com/claudehohl/Stikked)
 - Pastebin.com - [http://pastebin.com](http://pastebin.com)

### Linux specific features

#### Persistance
When you copy something and then close the application, the copied
data is lost. HaveClip prevents that. If the clipboard contents suddenly
disappears, it will be reset.

#### Serial batches
Copy a series of data into clipboard and then paste it in the same order.

Serial mode can be enabled in tray icon's context menu (right click)
or with a global keyboard shortcut Ctrl+Alt+S.

How to use serial mode:

  1. Enable serial mode
  2. Copy the three strings below into clipboard
    - one
    - two
    - three
  3. Make three pastes, you will get the strings in the same order as you copied them
    - one
    - two
    - three
  4. Disable serial mode

Serial batches are of course also synchronized on all PCs in your pool, that support
them. Therefore it is possible to copy/paste data into/from the same batch from all
PCs in pool.

Serial mode has it's limitations. It does not work in all applications, especially
applications which are checking clipboard contents whenever it changes, e.g. VirtualBox,
caja. HaveClip has problematic applications on blacklist and is ignoring them. Serial
batches do not work in them. This blacklist is so far hardcoded, it will be user editable
in the future versions.

### Sailfish OS
Sailfish version does not support paste services, nor serial batches.

Encryption is currently not supported neither.

#### Known issues
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
