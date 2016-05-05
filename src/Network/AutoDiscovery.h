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

#ifndef AUTODISCOVERY_H
#define AUTODISCOVERY_H

#define DISCOVERY_ATTEMPTS 3
#define DISCOVERY_INTERVAL 200

#include <QUdpSocket>
#include <QTimer>

#include "../Node.h"

class AutoDiscovery : public QUdpSocket
{
	Q_OBJECT
public:
	enum MessageType {
		DiscoverRequest = 0,
		DiscoverReply
	};

	explicit AutoDiscovery(QObject *parent = 0);

signals:
	void aboutToDiscover();
	void peerDiscovered(const Node &node);

public slots:
	Q_INVOKABLE void discover();

private slots:
	void allowAutoDiscoveryChange(bool allow);
	void sendRequest();
	void sendReply();
	void readPendingDatagrams();

private:
	QTimer *m_requestTimer;
	QTimer *m_replyTimer;
	int m_requestCounter;
	int m_replyCounter;
	QHostAddress m_replyAddress;
	quint16 m_replyPort;
	QList<Node> m_discoveredNodes;
	QByteArray m_requestDatagram;
	QByteArray m_replyDatagram;
	int m_identifier;

	void createDatagram(QByteArray &where, MessageType type);
	void parseDatagram(QByteArray &datagram, QHostAddress &sender);
	void reply(QHostAddress &host, quint16 port);
	bool isAlreadyDiscovered(Node &n);
	void resetDiscovery();
};

#endif // AUTODISCOVERY_H
