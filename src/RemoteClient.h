/*
  HaveClip

  Copyright (C) 2013 Jakub Skokan <aither@havefun.cz>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef REMOTECLIENT_H
#define REMOTECLIENT_H

#define REMOTE_CONTROL_HEADER_SIZE (sizeof(qint32) + sizeof(quint64))

#include <QObject>
#include <QLocalSocket>

#include "RemoteControls/RemoteBase.h"

class ClipboardManager;

class RemoteClient : public QObject
{
	Q_OBJECT
public:
	explicit RemoteClient(QLocalSocket *socket, ClipboardManager *manager, QObject *parent = 0);
	~RemoteClient();

private slots:
	void processInput();

private:
	QLocalSocket *m_sock;
	ClipboardManager *m_manager;
	QByteArray m_buffer;
	bool m_header;
	quint64 m_msgSize;
	RemoteControls::RemoteBase *cmd;

	void readHeader();
	void runCommand();
};

#endif // REMOTECLIENT_H
