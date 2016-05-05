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

#include "AutoDiscovery.h"

#include <QDateTime>

#include "../Settings.h"
#include "../Node.h"
#include "../Version.h"

AutoDiscovery::AutoDiscovery(QObject *parent) :
	QUdpSocket(parent),
	m_requestCounter(0),
	m_replyCounter(0)
{
	m_requestTimer = new QTimer(this);
	m_requestTimer->setInterval(DISCOVERY_INTERVAL);

	m_replyTimer = new QTimer(this);
	m_replyTimer->setInterval(DISCOVERY_INTERVAL);

	connect(m_requestTimer, SIGNAL(timeout()), this, SLOT(sendRequest()));
	connect(m_replyTimer, SIGNAL(timeout()), this, SLOT(sendReply()));

	connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

	allowAutoDiscoveryChange( Settings::get()->allowAutoDiscovery() );

	connect(Settings::get(), SIGNAL(allowAutoDiscoveryChanged(bool)), this, SLOT(allowAutoDiscoveryChange(bool)));
}

void AutoDiscovery::discover()
{
	emit aboutToDiscover();
	resetDiscovery();

	qsrand(QDateTime::currentDateTime().toTime_t());

	m_identifier = rand();

	createDatagram(m_requestDatagram, DiscoverRequest);
	sendRequest();

	m_requestTimer->start();
}

void AutoDiscovery::allowAutoDiscoveryChange(bool allow)
{
	if(allow && state() != QAbstractSocket::BoundState)
	{
		if(!bind(DISCOVERY_PORT, QUdpSocket::ShareAddress))
			qDebug() << "Discovery: failed to bind";

	} else if(!allow && state() == QAbstractSocket::BoundState)
		close();
}

void AutoDiscovery::sendRequest()
{
	writeDatagram(m_requestDatagram, QHostAddress::Broadcast, DISCOVERY_PORT);

	if(++m_requestCounter == DISCOVERY_ATTEMPTS)
		m_requestTimer->stop();
}

void AutoDiscovery::sendReply()
{
	writeDatagram(m_replyDatagram, m_replyAddress, m_replyPort);

	if(++m_replyCounter == DISCOVERY_ATTEMPTS)
	{
		m_replyCounter = 0;
		m_replyTimer->stop();
	}
}

void AutoDiscovery::readPendingDatagrams()
{
	while(hasPendingDatagrams())
	{
		QByteArray datagram;
		datagram.resize(pendingDatagramSize());

		QHostAddress sender;
		quint16 senderPort;

		readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

		parseDatagram(datagram, sender);
	}
}

void AutoDiscovery::createDatagram(QByteArray &where, MessageType type)
{
	QDataStream ds(&where, QIODevice::WriteOnly);

	ds << (quint32) PROTO_MAGIC_NUMBER;
	ds << (qint32) PROTO_VERSION;
	ds << (qint32) type;
	ds << (qint32) m_identifier;
	ds << Settings::get()->networkName();
	ds << Settings::get()->port();
}

void AutoDiscovery::parseDatagram(QByteArray &datagram, QHostAddress &sender)
{
	QDataStream ds(&datagram, QIODevice::ReadOnly);

	quint32 magic;
	qint32 version;
	qint32 type;
	qint32 identifier;
	QString name;
	quint16 port;

	ds >> magic;

	if(magic != PROTO_MAGIC_NUMBER)
	{
		qDebug() << "Discovery: magic number does not match";
		return;
	}

	ds >> version;
	ds >> type;
	ds >> identifier;

	if(identifier == m_identifier)
	{
		qDebug() << "Discovery: ignoring request from self";
		return;
	}

	ds >> name;
	ds >> port;

	switch(type)
	{
	case DiscoverRequest:
		reply(sender, port);
		return;

	case DiscoverReply:
		break;
	}

	Node n;

	n.setCompatible(version == PROTO_VERSION);
	n.setName(name);
	n.setHost(sender.toString());
	n.setPort(port);

	if(isAlreadyDiscovered(n))
	{
		qDebug() << "Discovery:" << name << "already discovered";
		return;
	}

	m_discoveredNodes << n;

	emit peerDiscovered(n);
}

void AutoDiscovery::reply(QHostAddress &host, quint16 port)
{
	if(m_replyTimer->isActive())
	{
		qDebug() << "Discovery: Already replying to someone";
		return;
	}

	qDebug() << "Discovery: reply to" << host << port;

	createDatagram(m_replyDatagram, DiscoverReply);

	m_replyAddress = host;
	m_replyPort = port;

	sendReply();

	m_replyTimer->start();
}

bool AutoDiscovery::isAlreadyDiscovered(Node &n)
{
	foreach(const Node &d, m_discoveredNodes)
	{
		if(d.host() == n.host() && d.port() == n.port())
			return true;
	}

	return false;
}

void AutoDiscovery::resetDiscovery()
{
	m_discoveredNodes.clear();
	m_requestDatagram.clear();
	m_requestCounter = 0;
	m_requestTimer->stop();
}
