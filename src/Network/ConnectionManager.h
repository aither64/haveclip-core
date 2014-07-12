#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QTcpServer>
#include <QHostInfo>
#include <QSslError>
#include <QSettings>

class Node;
class ClipboardItem;

class ConnectionManager : public QTcpServer
{
	Q_OBJECT
public:
	enum Encryption {
		None=0,
		Ssl,
		Tls
	};

	explicit ConnectionManager(QSettings *settings, QObject *parent = 0);
	void setNodes(QList<Node*> nodes);
	QString host();
	quint16 port();
	void setListenHost(QString host, quint16 port);
	void setHost(QString host);
	void setPort(quint16 port);
	void setEncryption(Encryption encryption);
	void setCertificate(QString cert);
	void setPrivateKey(QString key);
	void setPassword(QString pass);
	void startReceiving();
	void stopReceiving();
	void verifyConnection(Node *n);
	void syncClipboard(ClipboardItem *it);
	void saveSettings();

signals:
	void listenFailed(QString error);
	void untrustedCertificateError(Node *node, const QList<QSslError> errors);
	void sslFatalError(const QList<QSslError> errors);

public slots:

private:
	QSettings *m_settings;
	QString m_host;
	quint16 m_port;
	Encryption m_encryption;
	QString m_certificate;
	QString m_privateKey;
	QString m_password;
	QList<Node*> m_pool;

	void loadNodes();
	void startListening(QHostAddress addr = QHostAddress::Null);

private slots:
	void incomingConnection(int handle);
	void listenOnHost(const QHostInfo &m_host);

};

#endif // CONNECTIONMANAGER_H
