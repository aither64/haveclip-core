/*
  HaveClip

  Copyright (C) 2013 Jakub Skokan <aither@havefun.cz>

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

#include <QImage>
#include <QTextCodec>
#include <QDateTime>
#include <QDataStream>

#include "Communicator.h"
#include "Sender.h"
#include "Conversations/Introduction.h"
#include "Conversations/Verification.h"
#include "Conversations/ClipboardUpdate.h"

Sender::Sender(const Node &node, ConnectionManager *parent) :
	Communicator(parent),
	m_node(node)
{
}

void Sender::introduce(QString name, quint16 port)
{
	Conversations::Introduction *conv = new Conversations::Introduction(Communicator::Send, 0, this);
	conv->setName(name);
	conv->setPort(port);

	m_conversation = conv;

	connectToPeer();
}

void Sender::verify(QString code)
{
	Conversations::Verification *conv = new Conversations::Verification(Communicator::Send, 0, this);
	conv->setSecurityCode(code);

	m_conversation = conv;

	connectToPeer();
}

void Sender::distribute(ClipboardItem *content)
{
	m_conversation = new Conversations::ClipboardUpdate(Communicator::Send, content, this);

	connectToPeer();
}

void Sender::conversationSignals()
{
	connect(m_conversation, SIGNAL(introductionFinished(QString)), this, SLOT(interceptIntroductionFinish(QString)));
	connect(m_conversation, SIGNAL(verificationFinished(int)), this, SIGNAL(verificationFinished(int)));
}

void Sender::onError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "Unable to reach" << m_node.host() << ":" << socketError;

	Communicator::onError(socketError);
}

void Sender::interceptIntroductionFinish(QString name)
{
	emit introduceFinished(name, m_peerCertificate);
}

Node Sender::node()
{
	return m_node;
}

void Sender::connectToPeer()
{
	conversationSignals();

	if(encryption != Communicator::None)
	{
		setPeerVerifyMode(QSslSocket::QueryPeer);

		connect(this, SIGNAL(encrypted()), this, SLOT(onConnect()));

		switch(encryption)
		{
		case Communicator::Ssl:
			setProtocol(QSsl::SslV3);
			break;
		case Communicator::Tls:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			setProtocol(QSsl::TlsV1_0);
#else
			setProtocol(QSsl::TlsV1);
#endif
			break;

		default:
			break;
		}

		connectToHostEncrypted(m_node.host(), m_node.port());

	} else {
		connect(this, SIGNAL(connected()), this, SLOT(onConnect()));

		connectToHost(m_node.host(), m_node.port());
	}

	/**
	  We cannot write data immediately due to a bug in Qt which will cause application to end up
	  in infinite loop when connection fails.
	  */
}
