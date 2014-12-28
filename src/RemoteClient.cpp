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

#include "RemoteClient.h"

#include "RemoteControl.h"
#include "RemoteControls/Sync.h"

RemoteClient::RemoteClient(QLocalSocket *socket, ClipboardManager *manager, QObject *parent)
	: QObject(parent),
	  m_sock(socket),
	  m_manager(manager),
	  m_header(false),
	  m_msgSize(0)
{
	connect(m_sock, SIGNAL(readyRead()), this, SLOT(processInput()));
	connect(m_sock, SIGNAL(disconnected()), this, SLOT(deleteLater()));
}

RemoteClient::~RemoteClient()
{

}

void RemoteClient::processInput()
{
	while (m_sock->bytesAvailable())
	{
		m_buffer.append(m_sock->readAll());

		if (!m_header && ((quint64) m_buffer.size()) >= REMOTE_CONTROL_HEADER_SIZE)
			readHeader();
	}

	if (m_msgSize > 0 && m_header && ((quint64) m_buffer.size()) == m_msgSize)
		runCommand();
}

void RemoteClient::readHeader()
{
	QDataStream ds(&m_buffer, QIODevice::ReadOnly);
	qint32 type;

	ds >> type;
	ds >> m_msgSize;

	switch (type)
	{
	case RemoteControl::Sync:
		cmd = new RemoteControls::Sync(this);
		break;

	default:
		m_sock->close();
		return;
	}

	cmd->setManager(m_manager);

	m_header = true;
}

void RemoteClient::runCommand()
{
	m_buffer.remove(0, REMOTE_CONTROL_HEADER_SIZE);

	QDataStream ds(&m_buffer, QIODevice::ReadOnly);
	cmd->receiveCommand(ds);
}
