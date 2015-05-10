/*
  HaveClip

  Copyright (C) 2013-2015 Jakub Skokan <aither@havefun.cz>

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

#include <QStringList>
#include <QImage>
#include <QDataStream>

#include "Receiver.h"
#include "Conversation.h"

Receiver::Receiver(ConnectionManager *parent) :
	Communicator(parent)
{
}

void Receiver::communicate()
{
	if(encryption != Communicator::None)
	{
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

		setPeerVerifyMode(QSslSocket::QueryPeer);

		startServerEncryption();
	}
}

void Receiver::conversationSignals()
{
	Communicator::conversationSignals();

	connect(m_conversation, SIGNAL(verificationRequested(QString,quint16)), this, SLOT(interceptVerificationRequest(QString,quint16)));
	connect(m_conversation, SIGNAL(verificationCodeReceived(Conversations::Verification*,QString)), this, SIGNAL(verificationCodeReceived(Conversations::Verification*,QString)));
	connect(m_conversation, SIGNAL(verificationFinished(int)), this, SIGNAL(verificationFinished(int)));
	connect(m_conversation, SIGNAL(clipboardSync(ClipboardContainer*)), this, SIGNAL(clipboardUpdated(ClipboardContainer*)));
}

void Receiver::interceptVerificationRequest(QString name, quint16 port)
{
	Node n;
	n.setId();
	n.setName(name);
	n.setHost(peerAddress().toString());
	n.setPort(port);
	n.setCertificate(m_peerCertificate);

	emit verificationRequested(n);
}
