/*
  HaveClip

  Copyright (C) 2013-2016 Jakub Skokan <aither@havefun.cz>

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

#include "RemoteControl.h"
#include "Version.h"
#include "ClipboardManager.h"
#include "RemoteClient.h"

#include <QDesktopServices>
#include <QDir>

RemoteControl::RemoteControl(ClipboardManager *manager)
	: QObject(manager),
	  m_manager(manager)
{
	QLocalServer::removeServer(socketPath());
	server = new QLocalServer(this);

	if (!server->listen(socketPath(true)))
	{
		qWarning() << "Unable to listen on local server, CLI will not work";
		return;
	}

	qDebug() << "Remote control socket" << server->fullServerName();

	connect(server, SIGNAL(newConnection()), this, SLOT(localClientConnected()));
}

RemoteControl::~RemoteControl()
{
	server->close();
}

QString RemoteControl::socketPath(bool mkpath)
{
	QString path;
	QDir d;

#if defined(Q_OS_UNIX)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	path = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
	path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif

	if (mkpath)
		d.mkpath(path);

	path += "/" SOCKET_NAME;

#else
	path = SOCKET_NAME;
#endif

	return QDir::cleanPath(path);
}

void RemoteControl::localClientConnected()
{
	QLocalSocket *sock = server->nextPendingConnection();
	new RemoteClient(sock, m_manager, this);
}

