#include "ConnectionManager.h"

#include "../Node.h"
#include "Sender.h"
#include "Receiver.h"

ConnectionManager::ConnectionManager(QSettings *settings, QObject *parent) :
	QTcpServer(parent),
	m_settings(settings)
{
	m_host = m_settings->value("Connection/Host", "0.0.0.0").toString();
	m_port = m_settings->value("Connection/Port", 9999).toInt();

	m_encryption = (ConnectionManager::Encryption) m_settings->value("Connection/Encryption", 0).toInt();
	m_certificate = m_settings->value("Connection/Certificate", "certs/haveclip.crt").toString();
	m_privateKey = m_settings->value("Connection/PrivateKey", "certs/haveclip.key").toString();

	m_password = m_settings->value("AccessPolicy/Password").toString();
}

void ConnectionManager::setNodes(QList<Node*> nodes)
{
	m_pool = nodes;

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

QString ConnectionManager::host()
{
	return m_host;
}

quint16 ConnectionManager::port()
{
	return m_port;
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

void ConnectionManager::setPassword(QString pass)
{
	m_password = pass;
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

}

void ConnectionManager::syncClipboard(ClipboardItem *it)
{
	foreach(Node *n, m_pool)
	{
		Sender *d = new Sender(m_encryption, n, this);
		d->setPassword(m_password);

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

	m_settings->setValue("AccessPolicy/Password", m_password);
}

void ConnectionManager::incomingConnection(int handle)
{
	Receiver *c = new Receiver(m_encryption, this);
	c->setSocketDescriptor(handle);

	connect(c, SIGNAL(clipboardUpdated(ClipboardContainer*)), this, SLOT(updateClipboardFromNetwork(ClipboardContainer*)));

	c->setCertificateAndKey(m_certificate, m_privateKey);
	c->setPassword(m_password);
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
