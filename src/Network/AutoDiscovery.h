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
	void discover();

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
