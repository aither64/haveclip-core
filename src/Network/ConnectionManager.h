#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QTcpServer>
#include <QHostInfo>
#include <QSslError>

#include "../Node.h"
#include "Communicator.h"

class Sender;
class ClipboardItem;
class ClipboardContainer;
class AutoDiscovery;

namespace Conversations {
	class Verification;
}

class ConnectionManager : public QTcpServer
{
	Q_OBJECT
public:
	enum AuthMode {
		NoAuth=0,
		Introduced,
		Verified
	};

	enum CodeValidity {
		Valid,
		NotValid,
		Refused
	};

	explicit ConnectionManager(QObject *parent = 0);
	QString securityCode();
	Node& verifiedNode();
	int verifyTries();
	AutoDiscovery* autoDiscovery();
	void startReceiving();
	void stopReceiving();
	void syncClipboard(ClipboardItem *it);
	bool isAuthenticated(Communicator::Role role, AuthMode mode, QSslCertificate &cert);

signals:
	void listenFailed(QString error);
	void untrustedCertificateError(const Node &node, const QList<QSslError> errors);
	void sslFatalError(const QList<QSslError> errors);
	void introductionFinished();
	void introductionFailed(Communicator::CommunicationStatus status);
	void verificationRequested(const Node &n);
	void verificationFinished(ConnectionManager::CodeValidity validity);
	void verificationFailed(Communicator::CommunicationStatus status);
	void clipboardUpdated(ClipboardContainer *cont);

public slots:
	void verifyConnection(const Node &n);
	void provideSecurityCode(QString code);
	void cancelVerification();

private:
	AutoDiscovery *m_autoDiscovery;

	Sender *m_verifySender;
	Node m_verifiedNode;
	QString m_securityCode;
	int m_verifyTries;
	bool m_verifiedNodeAdded;

	void startListening(QHostAddress addr = QHostAddress::Null);
	QString generateSecurityCode(int len);

private slots:
	void hostAndPortChanged();
	void incomingConnection(int handle);
	void listenOnHost(const QHostInfo &m_host);
	void introduceComplete(QString name, QSslCertificate cert);
	void introduceFinish(Communicator::CommunicationStatus status);
	void verificationRequest(const Node &n);
	void verifySecurityCode(Conversations::Verification *v, QString code);
	void verificationComplete(int validity);
	void verificationFinish(Communicator::CommunicationStatus status);
	void verifySenderDestroy();

};

#endif // CONNECTIONMANAGER_H
