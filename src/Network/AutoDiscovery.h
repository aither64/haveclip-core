#ifndef AUTODISCOVERY_H
#define AUTODISCOVERY_H

#define DISCOVERY_ATTEMPTS 3
#define DISCOVERY_INTERVAL 200

#include <QUdpSocket>
#include <QTimer>

class Node;

class AutoDiscovery : public QUdpSocket
{
	Q_OBJECT
public:
	enum MessageType {
		DiscoverRequest = 0,
		DiscoverReply
	};

	explicit AutoDiscovery(QObject *parent = 0);
	void setName(QString name);
	void setPort(quint16 port);

signals:
	void aboutToDiscover();
	void peerDiscovered(Node *node);

public slots:
	void discover();

private slots:
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
	QList<Node*> m_discoveredNodes;
	QString m_name;
	quint16 m_port;
	QByteArray m_requestDatagram;
	QByteArray m_replyDatagram;
	int m_identifier;

	void createDatagram(QByteArray &where, MessageType type);
	void parseDatagram(QByteArray &datagram, QHostAddress &sender, quint16 port);
	void reply(QHostAddress &host, quint16 port);
	bool isAlreadyDiscovered(Node *n);
	void resetDiscovery();
};

#endif // AUTODISCOVERY_H
