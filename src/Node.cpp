#include "Node.h"

Node::Node(Node *other)
{
	if(other)
	{
		m_name = other->m_name;
		m_host = other->m_host;
		m_port = other->m_port;
		m_certificate = other->m_certificate;
	}
}

QString Node::name()
{
	return m_name;
}

QString Node::host()
{
	return m_host;
}

quint16 Node::port()
{
	return m_port;
}

QSslCertificate Node::certificate()
{
	return m_certificate;
}

void Node::setName(QString name)
{
	m_name = name;
}

void Node::setHost(QString host)
{
	m_host = host;
}

void Node::setPort(quint16 port)
{
	m_port = port;
}

void Node::setCertificate(QSslCertificate cert)
{
	m_certificate = cert;
}

Node* Node::load(QSettings *settings)
{
	Node *n = new Node();
	n->m_name = settings->value("Name").toString();
	n->m_host = settings->value("Host").toString();
	n->m_port = settings->value("Port").toString().toUShort();

	QByteArray cert = settings->value("Certificate").toString().toUtf8();

	if(!cert.isEmpty())
		n->m_certificate = QSslCertificate::fromData(cert).first();

	return n;
}

void Node::save(QSettings *settings)
{
	settings->setValue("Name", m_name);
	settings->setValue("Host", m_host);
	settings->setValue("Port", m_port);
	settings->setValue("Certificate", m_certificate.toPem());
}
