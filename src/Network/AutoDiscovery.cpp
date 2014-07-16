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
	m_name = QHostInfo::localHostName();
	m_port = Settings::get()->port();

	m_requestTimer = new QTimer(this);
	m_requestTimer->setInterval(DISCOVERY_INTERVAL);

	m_replyTimer = new QTimer(this);
	m_replyTimer->setInterval(DISCOVERY_INTERVAL);

	connect(m_requestTimer, SIGNAL(timeout()), this, SLOT(sendRequest()));
	connect(m_replyTimer, SIGNAL(timeout()), this, SLOT(sendReply()));

	connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));

	bind(DISCOVERY_PORT, QUdpSocket::ShareAddress);
}

void AutoDiscovery::setName(QString name)
{
	m_name = name;
}

void AutoDiscovery::setPort(quint16 port)
{
	m_port = port;
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
	ds << m_name;
	ds << m_port;
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
