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

#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <QSslSocket>
#include <QMimeData>

#include "Communicator.h"

class Sender : public Communicator
{
	Q_OBJECT
public:
	explicit Sender(History *history, ClipboardManager::Encryption enc, ClipboardManager::Node *node, QObject *parent = 0);
	ClipboardManager::Node *node();

public slots:
	void distribute(ClipboardItem *content);

protected slots:
	virtual void onError(QAbstractSocket::SocketError socketError);
	virtual void onSslError(const QList<QSslError> &errors);

private:
	ClipboardManager::Node *m_node;

	void connectToPeer();
	
};

#endif // DISTRIBUTOR_H
