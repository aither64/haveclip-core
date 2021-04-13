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

#include "Node.h"

#include "Settings.h"

Node::Node() :
	m_id(0),
	m_send(true),
	m_recv(true)
{

}

Node::Node(const Node &other)
{
	m_id = other.m_id;
	m_name = other.m_name;
	m_host = other.m_host;
	m_port = other.m_port;
	m_send = other.m_send;
	m_recv = other.m_recv;
	m_certificate = other.m_certificate;
	m_compatible = other.m_compatible;
}

unsigned int Node::id() const
{
	return m_id;
}

QString Node::name() const
{
	return m_name;
}

QString Node::host() const
{
	return m_host;
}

quint16 Node::port() const
{
	return m_port;
}

bool Node::isSendEnabled() const
{
	return m_send;
}

bool Node::isReceiveEnabled() const
{
	return m_recv;
}

QSslCertificate Node::certificate() const
{
	return m_certificate;
}

bool Node::isCompatible() const
{
	return m_compatible;
}

bool Node::isValid() const
{
	return m_id > 0;
}

bool Node::hasId() const
{
	return m_id > 0;
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

void Node::setSendEnabled(bool send)
{
	m_send = send;
}

void Node::setReceiveEnabled(bool recv)
{
	m_recv = recv;
}

void Node::setCertificate(QSslCertificate cert)
{
	m_certificate = cert;
}

void Node::setCompatible(bool compat)
{
	m_compatible = compat;
}

void Node::setId()
{
	if(!m_id)
		m_id = Settings::get()->nextNodeId();
}

void Node::update(const Node &other)
{
	m_name = other.m_name;
	m_host = other.m_host;
	m_port = other.m_port;
	m_send = other.m_send;
	m_recv = other.m_recv;
	m_certificate = other.certificate();
}

Node Node::load(QSettings *settings, unsigned int id)
{
	Node n;
	n.m_id = id;
	n.m_name = settings->value("Name").toString();
	n.m_host = settings->value("Host").toString();
	n.m_port = settings->value("Port").toString().toUShort();
	n.m_send = settings->value("Send", true).toBool();
	n.m_recv = settings->value("Receive", true).toBool();

	QByteArray cert = settings->value("Certificate").toString().toUtf8();

	if(!cert.isEmpty())
	{
		QList<QSslCertificate> certs = QSslCertificate::fromData(cert);

		if(!certs.isEmpty())
			n.m_certificate = certs.first();
	}

	return n;
}

void Node::save(QSettings *settings)
{
	settings->setValue("Name", m_name);
	settings->setValue("Host", m_host);
	settings->setValue("Port", m_port);
	settings->setValue("Send", m_send);
	settings->setValue("Receive", m_recv);
	settings->setValue("Certificate", QString(m_certificate.toPem()));
}
