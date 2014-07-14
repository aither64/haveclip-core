#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QTcpServer>
#include <QHostInfo>
#include <QSslError>
#include <QSettings>

class Sender;
class Node;
class ClipboardItem;
class ClipboardContainer;

namespace Conversations {
	class Verification;
}

class ConnectionManager : public QTcpServer
{
	Q_OBJECT
public:
	enum Encryption {
		None=0,
		Ssl,
		Tls
	};

	enum AuthMode {
		NoAuth=0,
		Introduced,
		Verified
	};

	explicit ConnectionManager(QSettings *settings, QObject *parent = 0);
	void setNodes(QList<Node*> nodes);
	QString host();
	quint16 port();
	QString securityCode();
	Node* verifiedNode();
	void setListenHost(QString host, quint16 port);
	void setHost(QString host);
	void setPort(quint16 port);
	void setEncryption(Encryption encryption);
	void setCertificate(QString cert);
	void setPrivateKey(QString key);
	void setPassword(QString pass);
	void startReceiving();
	void stopReceiving();
	void syncClipboard(ClipboardItem *it);
	void saveSettings();
	bool isAuthenticated(AuthMode mode, QSslCertificate &cert);

signals:
	void listenFailed(QString error);
	void untrustedCertificateError(Node *node, const QList<QSslError> errors);
	void sslFatalError(const QList<QSslError> errors);
	void introductionFinished();
	void verificationRequested(Node *n);
	void verificationFinished(bool ok);
	void clipboardUpdated(ClipboardContainer *cont);

public slots:
	void verifyConnection(Node *n);
	void provideSecurityCode(QString code);
	void cancelVerification();

private:
	QSettings *m_settings;
	QString m_host;
	quint16 m_port;
	Encryption m_encryption;
	QString m_certificate;
	QString m_privateKey;
	QString m_password;
	QList<Node*> m_pool;

	Sender *m_verifySender;
	Node *m_verifiedNode;
	QString m_securityCode;

	void loadNodes();
	void saveNodes();
	void startListening(QHostAddress addr = QHostAddress::Null);
	QString generateSecurityCode(int len);

private slots:
	void incomingConnection(int handle);
	void listenOnHost(const QHostInfo &m_host);
	void introduceComplete(QString name, QSslCertificate cert);
	void verificationRequest(Node *n);
	void verifySecurityCode(Conversations::Verification *v, QString code);
	void verificationFinish(bool ok);
	void verifySenderDestroy();

};

#endif // CONNECTIONMANAGER_H
