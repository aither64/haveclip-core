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

#include "Cli.h"
#include "Version.h"

#include <QCoreApplication>
#include <QStringList>
#include <QDataStream>
#include <QDebug>

#include "RemoteControls/Sync.h"

QLocalSocket *Cli::m_socket = 0;

Cli::Cli(QObject *parent)
	: QObject(parent)
{

}

int Cli::doExec()
{
	QString str = QCoreApplication::arguments()[1];
	RemoteControls::RemoteBase *cmd = 0;
	QByteArray buf;
	QDataStream ds(&buf, QIODevice::WriteOnly);

	if (!str.compare("sync"))
	{
		cmd = new RemoteControls::Sync(this);

	} else {
		qWarning() << "Unknown command" << str;
		return 1;
	}

	cmd->setCli(this);

	ds << (qint32) cmd->type();
	ds << (quint64) 0; // Filled later

	cmd->sendCommand(ds);

	ds.device()->seek(4); // seek to message length field
	ds << (quint64) buf.size();

	m_socket->write(buf);
	m_socket->waitForBytesWritten();

	return 0;
}

Cli::~Cli()
{
	delete m_socket;
}

bool Cli::remoteConnect()
{
	bool connected;

	m_socket = new QLocalSocket();
	m_socket->connectToServer(SOCKET_NAME, QIODevice::WriteOnly);

	connected = m_socket->waitForConnected();

	if (!connected)
		delete m_socket;

	return connected;
}

int Cli::exec()
{
	Cli c;

	return c.doExec();
}

QLocalSocket *Cli::socket() const
{
	return m_socket;
}

