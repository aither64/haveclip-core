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

	Q_ENUMS(CodeValidity)

	explicit ConnectionManager(QObject *parent = 0);

	Q_PROPERTY(QString securityCode READ securityCode NOTIFY securityCodeChanged)
	QString securityCode();

	Node& verifiedNode();

	int verifyTries();
	AutoDiscovery* autoDiscovery();
	void startReceiving();
	void stopReceiving();
	void syncClipboard(ClipboardItem *it);
	bool isAuthenticated(Communicator::Role role, AuthMode mode, QSslCertificate &cert, QHostAddress peerAddr);

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
	void securityCodeChanged(QString code);

public slots:
	Q_INVOKABLE void verifyConnection(unsigned int nodeId);
	Q_INVOKABLE void verifyConnection(QString host, quint16 port);
	Q_INVOKABLE void verifyConnection(const Node &n);
	Q_INVOKABLE void provideSecurityCode(QString code);
	Q_INVOKABLE void cancelVerification();

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
	void receiveEnabledChange(bool enabled);
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
