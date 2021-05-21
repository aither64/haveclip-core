#if 0
#  HaveClip

#  Copyright (C) 2013-2016 Jakub Skokan <aither@havefun.cz>

#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.

#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.

#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#endif

#ifndef VERSION_H
#define VERSION_H

#define VERSION _D_VERSION
#if 0 // start of .pri file

VERSION  = 0.15.0
DEFINES += _D_VERSION=\"\\\"$$VERSION\\\"\"

#endif // end of .pri file

#define HISTORY_MAGIC_NUMBER 0x84D3C117
#define HISTORY_VERSION 3

#define PROTO_MAGIC_NUMBER 0x84D3C117
#define PROTO_VERSION 4

#define CONFIG_VERSION 3

#define DISCOVERY_PORT 9999

#define VERIFICATION_TRIES 5

#define SOCKET_NAME "haveclip.sock"

#endif // VERSION_H
