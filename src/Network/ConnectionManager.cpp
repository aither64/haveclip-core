#include "ConnectionManager.h"

#include <QDateTime>

#include "../Node.h"
#include "Sender.h"
#include "Receiver.h"

#include "Conversations/Verification.h"

ConnectionManager::ConnectionManager(QSettings *settings, QObject *parent) :
	QTcpServer(parent),
	m_settings(settings),
	m_verifySender(0),
	m_verifiedNode(0)
{
	m_host = m_settings->value("Connection/Host", "0.0.0.0").toString();
	m_port = m_settings->value("Connection/Port", 9999).toInt();

	m_encryption = (ConnectionManager::Encryption) m_settings->value("Connection/Encryption", 0).toInt();
	m_certificate = m_settings->value("Connection/Certificate", "certs/haveclip.crt").toString();
	m_privateKey = m_settings->value("Connection/PrivateKey", "certs/haveclip.key").toString();
}

void ConnectionManager::setNodes(QList<Node*> nodes)
{
	m_pool = nodes;

	saveNodes();
}

QString ConnectionManager::host()
{
	return m_host;
}

quint16 ConnectionManager::port()
{
	return m_port;
}

QString ConnectionManager::securityCode()
{
	return m_securityCode;
}

Node* ConnectionManager::verifiedNode()
{
	return m_verifiedNode;
}

void ConnectionManager::setListenHost(QString host, quint16 port)
{
	if(host != m_host || port != m_port)
	{
		m_host = host;
		m_port = port;

		if(isListening())
			close();

		startListening();
	}
}

void ConnectionManager::setHost(QString host)
{
	setListenHost(host, m_port);
}

void ConnectionManager::setPort(quint16 port)
{
	setListenHost(m_host, port);
}

void ConnectionManager::setEncryption(Encryption encryption)
{
	m_encryption = encryption;
}

void ConnectionManager::setCertificate(QString cert)
{
	m_certificate = cert;
}

void ConnectionManager::setPrivateKey(QString key)
{
	m_privateKey = key;
}

void ConnectionManager::startReceiving()
{
	startListening();
}

void ConnectionManager::stopReceiving()
{
	close();
}

void ConnectionManager::verifyConnection(Node *n)
{
	m_verifiedNode = n;

	m_verifySender = new Sender(m_encryption, n, this);
	m_verifySender->setCertificateAndKey(m_certificate, m_privateKey);

	connect(m_verifySender, SIGNAL(destroyed()), this, SLOT(verifySenderDestroy()));
	connect(m_verifySender, SIGNAL(introduceFinished(QString,QSslCertificate)), this, SLOT(introduceComplete(QString,QSslCertificate)));

	m_verifySender->introduce(m_port);
}

void ConnectionManager::provideSecurityCode(QString code)
{
	Sender *s = new Sender(m_encryption, m_verifiedNode, this);
	s->setCertificateAndKey(m_certificate, m_privateKey);

	connect(s, SIGNAL(verificationFinished(bool)), this, SLOT(verificationFinish(bool)));

	s->verify(code);
}

void ConnectionManager::cancelVerification()
{
	if(m_verifySender)
		m_verifySender->abort();

	delete m_verifiedNode;
	m_verifiedNode = 0;
	m_securityCode = "";
}

void ConnectionManager::syncClipboard(ClipboardItem *it)
{
	foreach(Node *n, m_pool)
	{
		Sender *d = new Sender(m_encryption, n, this);
		d->setCertificateAndKey(m_certificate, m_privateKey);

		connect(d, SIGNAL(untrustedCertificateError(Node*,QList<QSslError>)), this, SIGNAL(untrustedCertificateError(Node*,QList<QSslError>)));
		connect(d, SIGNAL(sslFatalError(QList<QSslError>)), this, SIGNAL(sslFatalError(QList<QSslError>)));

		d->distribute(it);
	}
}

void ConnectionManager::saveSettings()
{
	m_settings->setValue("Connection/Host", m_host);
	m_settings->setValue("Connection/Port", m_port);
	m_settings->setValue("Connection/Encryption", m_encryption);
	m_settings->setValue("Connection/Certificate", m_certificate);
	m_settings->setValue("Connection/PrivateKey", m_privateKey);
}

bool ConnectionManager::isAuthenticated(ConnectionManager::AuthMode mode, QSslCertificate &cert)
{
	switch(mode)
	{
	case NoAuth:
		return true;

	case Introduced:
		return m_verifiedNode->certificate() == cert;

	case Verified:
		foreach(Node *n, m_pool)
		{
			if(n->certificate() == cert)
				return true;
		}
	}

	return false;
}

void ConnectionManager::incomingConnection(int handle)
{
	Receiver *c = new Receiver(m_encryption, this);
	c->setSocketDescriptor(handle);

	connect(c, SIGNAL(verificationRequested(Node*)), this, SLOT(verificationRequest(Node*)));
	connect(c, SIGNAL(verificationCodeReceived(Conversations::Verification*,QString)), this, SLOT(verifySecurityCode(Conversations::Verification*,QString)));
	connect(c, SIGNAL(clipboardUpdated(ClipboardContainer*)), this, SIGNAL(clipboardUpdated(ClipboardContainer*)));

	c->setCertificateAndKey(m_certificate, m_privateKey);
	c->communicate();
}

void ConnectionManager::startListening(QHostAddress addr)
{
	if(addr.isNull())
		addr.setAddress(m_host);

	if(!addr.isNull())
	{
		if(!listen(addr, m_port))
		{
			//QMessageBox::warning(0, tr("Unable to start listening"), tr("Listening failed, clipboard receiving is not active!\n\n") + errorString());
			emit listenFailed(tr("Listen failed: %1").arg(errorString()));
			return;
		}

		qDebug() << "Listening on" << serverAddress() << serverPort();

	} else {
		QHostInfo::lookupHost(m_host, this, SLOT(listenOnHost(QHostInfo)));
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

	m_verifiedNode->setName(name);
	m_verifiedNode->setCertificate(cert);

	m_securityCode = generateSecurityCode(6);

	emit introductionFinished();
}

void ConnectionManager::verificationRequest(Node *node)
{
	qDebug() << "ConnectionManager::verificationRequest" << node->name();

	if(m_verifiedNode)
		delete m_verifiedNode;

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
		m_pool << m_verifiedNode;
		saveNodes();

		emit verificationFinished(valid);

		m_verifiedNode = 0;
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
		m_pool << m_verifiedNode;
		saveNodes();

		emit verificationFinished(ok);

		m_verifiedNode = 0;
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

void ConnectionManager::loadNodes()
{
	Node *n;
	qDeleteAll(m_pool);

	m_settings->beginGroup("Pool/Nodes");

	foreach(QString grp, m_settings->childGroups())
	{
		m_settings->beginGroup(grp);

		n = Node::load(m_settings);

		if(n) m_pool << n;

		m_settings->endGroup();
	}

	m_settings->endGroup();

}

void ConnectionManager::saveNodes()
{
	m_settings->beginGroup("Pool/Nodes");
	m_settings->remove("");

	int cnt = m_pool.count();

	for(int i = 0; i < cnt; i++)
	{
		m_settings->beginGroup(QString::number(i));

		m_pool[i]->save(m_settings);

		m_settings->endGroup();
	}

	m_settings->endGroup();
}
