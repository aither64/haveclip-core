#include "ConnectionManager.h"

#include <QDateTime>

#include "../Settings.h"
#include "../Node.h"
#include "Sender.h"
#include "Receiver.h"
#include "AutoDiscovery.h"

#include "Conversations/Verification.h"

ConnectionManager::ConnectionManager(QObject *parent) :
	QTcpServer(parent),
	m_verifySender(0)
{
	m_autoDiscovery = new AutoDiscovery(this);

	Settings *s = Settings::get();

	connect(s, SIGNAL(hostChanged(QString)), this, SLOT(hostAndPortChanged()));
	connect(s, SIGNAL(portChanged(quint16)), this, SLOT(hostAndPortChanged()));
	connect(s, SIGNAL(hostAndPortChanged(QString,quint16)), this, SLOT(hostAndPortChanged()));
}

QString ConnectionManager::securityCode()
{
	return m_securityCode;
}

Node& ConnectionManager::verifiedNode()
{
	return m_verifiedNode;
}

AutoDiscovery* ConnectionManager::autoDiscovery()
{
	return m_autoDiscovery;
}

void ConnectionManager::startReceiving()
{
	startListening();
}

void ConnectionManager::stopReceiving()
{
	close();
}

void ConnectionManager::verifyConnection(const Node &n)
{
	m_verifiedNode = n;

	m_verifySender = new Sender(n, this);

	connect(m_verifySender, SIGNAL(destroyed()), this, SLOT(verifySenderDestroy()));
	connect(m_verifySender, SIGNAL(finished(Communicator::CommunicationStatus)), this, SLOT(introduceFinish(Communicator::CommunicationStatus)));
	connect(m_verifySender, SIGNAL(introduceFinished(QString,QSslCertificate)), this, SLOT(introduceComplete(QString,QSslCertificate)));

	m_verifySender->introduce(Settings::get()->networkName(), Settings::get()->port());
}

void ConnectionManager::provideSecurityCode(QString code)
{
	Sender *s = new Sender(m_verifiedNode, this);

	connect(s, SIGNAL(verificationFinished(bool)), this, SLOT(verificationFinish(bool)));

	s->verify(code);
}

void ConnectionManager::cancelVerification()
{
	if(m_verifySender)
		m_verifySender->abort();

	m_verifiedNode = Node();
	m_securityCode = "";
}

void ConnectionManager::syncClipboard(ClipboardItem *it)
{
	foreach(const Node &n, Settings::get()->nodes())
	{
		Sender *d = new Sender(n, this);

		connect(d, SIGNAL(untrustedCertificateError(Node,QList<QSslError>)), this, SIGNAL(untrustedCertificateError(Node,QList<QSslError>)));
		connect(d, SIGNAL(sslFatalError(QList<QSslError>)), this, SIGNAL(sslFatalError(QList<QSslError>)));

		d->distribute(it);
	}
}

bool ConnectionManager::isAuthenticated(ConnectionManager::AuthMode mode, QSslCertificate &cert)
{
	switch(mode)
	{
	case NoAuth:
		return true;

	case Introduced:
		return m_verifiedNode.certificate() == cert;

	case Verified:
		foreach(const Node &n, Settings::get()->nodes())
		{
			if(n.certificate() == cert)
				return true;
		}
	}

	return false;
}

void ConnectionManager::hostAndPortChanged()
{
	if(isListening())
		close();

	startListening();
}

void ConnectionManager::incomingConnection(int handle)
{
	Receiver *c = new Receiver(this);
	c->setSocketDescriptor(handle);

	connect(c, SIGNAL(verificationRequested(Node)), this, SLOT(verificationRequest(Node)));
	connect(c, SIGNAL(verificationCodeReceived(Conversations::Verification*,QString)), this, SLOT(verifySecurityCode(Conversations::Verification*,QString)));
	connect(c, SIGNAL(clipboardUpdated(ClipboardContainer*)), this, SIGNAL(clipboardUpdated(ClipboardContainer*)));

	c->communicate();
}

void ConnectionManager::startListening(QHostAddress addr)
{
	if(addr.isNull())
		addr.setAddress(Settings::get()->host());

	if(!addr.isNull())
	{
		if(!listen(addr, Settings::get()->port()))
		{
			//QMessageBox::warning(0, tr("Unable to start listening"), tr("Listening failed, clipboard receiving is not active!\n\n") + errorString());
			emit listenFailed(tr("Listen failed: %1").arg(errorString()));
			return;
		}

		qDebug() << "Listening on" << serverAddress() << serverPort();

	} else {
		QHostInfo::lookupHost(Settings::get()->host(), this, SLOT(listenOnHost(QHostInfo)));
	}
}

QString ConnectionManager::generateSecurityCode(int len)
{
	QString code;
	qsrand(QDateTime::currentDateTime().toTime_t());

	for(int i = 0; i < len; i++)
	{
		code += (char) (48 + (rand() % 9 ));
	}

	return code;
}

void ConnectionManager::listenOnHost(const QHostInfo &host)
{
	if(host.error() != QHostInfo::NoError) {
//		QMessageBox::warning(0, tr("Unable to resolve hostname"), tr("Resolving failed: ") + host.errorString());
		emit listenFailed(tr("Resolving failed: %1").arg(host.errorString()));
		return;
	}

	QList<QHostAddress> addrs = host.addresses();

	if(addrs.size() == 0)
	{
//		QMessageBox::warning(0, tr("Hostname has no IP address"), tr("Hostname has no IP addresses. Clipboard receiving is not active."));
		emit listenFailed(tr("Hostname has no IP addresses. Clipboard receiving is not active."));
		return;
	}

	startListening(addrs.first());
}

void ConnectionManager::introduceComplete(QString name, QSslCertificate cert)
{
	qDebug() << "Introduce complete, generate security code";

	m_verifiedNode.setName(name);
	m_verifiedNode.setCertificate(cert);

	m_securityCode = generateSecurityCode(6);

	emit introductionFinished();
}

void ConnectionManager::introduceFinish(Communicator::CommunicationStatus status)
{
	if(status != Communicator::Ok)
		emit introductionFailed(status);
}

void ConnectionManager::verificationRequest(const Node &node)
{
	qDebug() << "ConnectionManager::verificationRequest" << node.name();

	m_verifiedNode = node;

	emit verificationRequested(m_verifiedNode);
}

void ConnectionManager::verifySecurityCode(Conversations::Verification *v, QString code)
{
	qDebug() << "Verifying security code";

	bool valid = m_securityCode == code;

	v->setValid(valid);

	if(valid)
	{
		Settings::get()->addOrUpdateNode(m_verifiedNode);
		Settings::get()->save();

		emit verificationFinished(valid);

		m_verifiedNode = Node();
		m_securityCode = "";

	} else {
		emit verificationFinished(valid);
	}
}

void ConnectionManager::verificationFinish(bool ok)
{
	qDebug() << "Verification finished" << ok;

	if(ok)
	{
		Settings::get()->addOrUpdateNode(m_verifiedNode);
		Settings::get()->save();

		emit verificationFinished(ok);

		m_verifiedNode = Node();
		m_securityCode = "";

	} else {
		emit verificationFinished(ok);
	}

//	qDebug() << "Still here :)";
}

void ConnectionManager::verifySenderDestroy()
{
	m_verifySender = 0;
}
