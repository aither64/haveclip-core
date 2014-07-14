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
	explicit Sender(ConnectionManager::Encryption enc, Node *node, ConnectionManager *parent = 0);
	Node *node();

signals:
	void introduceFinished(QString name, QSslCertificate cert);
	void verificationFinished(bool success);

public slots:
	void introduce(quint16 port);
	void verify(QString code);
	void distribute(ClipboardItem *content);

protected:
	virtual void conversationSignals();

protected slots:
	virtual void onError(QAbstractSocket::SocketError socketError);

private slots:
	void interceptIntroductionFinish(QString name);

private:
	Node *m_node;

	void connectToPeer();
	
};

#endif // DISTRIBUTOR_H
