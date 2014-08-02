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

#include "Communicator.h"

#include <QTimer>

#include "../Version.h"
#include "../Settings.h"
#include "Conversations/Introduction.h"
#include "Conversations/Verification.h"
#include "Conversations/ClipboardUpdate.h"

Communicator::Communicator(ConnectionManager *parent) :
	QSslSocket(parent),
	m_conman(parent),
	m_conversation(0),
	dataRead(0),
	haveHeader(false),
	msgLen(0),
	m_runPostDone(false)
{
	encryption = Settings::get()->encryption();

	setLocalCertificate(Settings::get()->certificate());
	setPrivateKey(Settings::get()->privateKey());

	connect(this, SIGNAL(encrypted()), this, SLOT(onEncrypted()));
	connect(this, SIGNAL(readyRead()), this, SLOT(onRead()));
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(this, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslError(QList<QSslError>)));
}

Communicator::~Communicator()
{
	if(m_conversation)
		delete m_conversation;
}

QString Communicator::statusToString(CommunicationStatus status)
{
	switch(status)
	{
	case Ok:
		return tr("Ok");

	case ConnectionFailed:
		return tr("Connection failed");

	case UnrecoverableSslError:
		return tr("Unrecoverable SSL error");

	case IncompleteHeader:
	case MagicNumberNotMatches:
		return tr("Communication error");

	case ProtocolVersionMismatch:
	case UnknownConversation:
	case InvalidConversation:
	case UnexpectedMessageType:
		return tr("Incompatible HaveClip version");

	case NotAuthenticated:
		return tr("Authentication failure");

	case MessageTooLarge:
		return tr("Message is too large");

	default:
		return tr("Unknown error");
	}
}

void Communicator::sendMessage()
{
	QByteArray buf;
	QDataStream ds(&buf, QIODevice::WriteOnly);
	quint64 bufSize;

	ds << (quint32) PROTO_MAGIC_NUMBER;
	ds << (qint32) PROTO_VERSION;
	ds << (qint32) m_conversation->type();
	ds << (qint32) m_conversation->currentCommandType();
	ds << (quint64) 0; // Filled later

	m_conversation->send(ds);

	ds.device()->seek(16); // seek to message length field

	bufSize = buf.size();

	if(bufSize > Settings::get()->maxSendSize())
	{
		qDebug() << "Message too large:" << bufSize << "bytes";
		emit finished(MessageTooLarge);
		this->deleteLater();
	}

	ds << (quint64) bufSize;

	qDebug() << "Send message" << m_conversation->currentCommandType() << bufSize << "bytes";

	write(buf);

	continueConversation();
}

void Communicator::receiveMessage()
{
	QDataStream ds(&buffer, QIODevice::ReadOnly);
	ds.device()->seek(HEADER_SIZE); // skip header

	if(m_conversation->currentRole() != Communicator::Receive)
	{
		qDebug() << "Fuckup";
		emit finished(UnknownError);
		this->deleteLater();
		return;
	}

	m_conversation->receive(ds);

	buffer.remove(0, msgLen);

	qDebug() << "Received message" << msgLen;

	haveHeader = false;
	dataRead -= msgLen;
	msgLen = 0;

	continueConversation();
}

void Communicator::readHeader()
{
	QDataStream ds(&buffer, QIODevice::ReadOnly);

	quint32 magic;
	qint32 version, convType, cmdType;

	if(buffer.size() < HEADER_SIZE)
	{
		qDebug() << "Invalid message - incomplete header";
		emit finished(IncompleteHeader);
		this->deleteLater();
		return;
	}

	ds >> magic;

	if(magic != PROTO_MAGIC_NUMBER)
	{
		qDebug() << "Invalid message - magic number does not match";
		emit finished(MagicNumberNotMatches);
		this->deleteLater();
		return;
	}

	ds >> version;

	if(version != PROTO_VERSION)
	{
		qDebug() << "Protocol version does not match. Supported is" << PROTO_VERSION << ", received" << version;
		emit finished(ProtocolVersionMismatch);
		this->deleteLater();
		return;
	}

	ds >> convType;

	if(!m_conversation) // Receiver must wait for sender to specify conversation type in first msg
	{
		switch(convType)
		{
		case Conversation::Introduction: {
			qDebug() << "Initiating conversation Introduction";
			Conversations::Introduction *c = new Conversations::Introduction(Communicator::Receive, 0, this);
			c->setName(Settings::get()->networkName());
			c->setPort(Settings::get()->port());
			m_conversation = c;

			break;
		}

		case Conversation::Verification:
			qDebug() << "Initiating conversation Verification";
			m_conversation = new Conversations::Verification(Communicator::Receive, 0, this);
			break;

		case Conversation::ClipboardUpdate: {
			qDebug() << "Initiating conversation ClipboardUpdate";
			Conversations::ClipboardUpdate *conv = new Conversations::ClipboardUpdate(Communicator::Receive, 0, this);
			conv->setFilters(Settings::get()->receiveFilterMode(), Settings::get()->receiveFilters());

			m_conversation = conv;
			break;
		}

		default:
			qDebug() << "Unknown conversation" << convType;
			emit finished(UnknownConversation);
			this->deleteLater();
			return;
		}

		if(!m_conman->isAuthenticated(Communicator::Receive, m_conversation->authenticate(), m_peerCertificate, peerAddress()))
		{
			qDebug() << "Authentication failed";
			emit finished(NotAuthenticated);
			this->deleteLater();
			return;
		}

		conversationSignals();

	} else if(convType != m_conversation->type()) {
		qDebug() << "Invalid conversation: expected" << m_conversation->type() << ", received" << convType;
		emit finished(InvalidConversation);
		this->deleteLater();
		return;
	}

	ds >> cmdType;

	if(cmdType != m_conversation->currentCommandType())
	{
		qDebug() << "Unexpected message type: expected" << m_conversation->currentCommandType() << ", received" << cmdType;
		emit finished(UnexpectedMessageType);
		this->deleteLater();
		return;
	}

	ds >> msgLen;

	if(msgLen > Settings::get()->maxReceiveSize())
	{
		qDebug() << "Message too large:" << msgLen << "bytes";
		emit finished(MessageTooLarge);
		this->deleteLater();
	}
}

void Communicator::conversationSignals()
{
	connect(m_conversation, SIGNAL(done()), this, SLOT(conversationDone()));
	connect(m_conversation, SIGNAL(morphed(Conversation*)), this, SLOT(morphConversation(Conversation*)));
}

void Communicator::continueConversation()
{
	if(m_conversation->isDone())
	{
		m_runPostDone = true;
		disconnectFromHost();

#if defined(MER_SAILFISH)
		if(encryption == Communicator::None && state() != QAbstractSocket::ClosingState)
		{
			QTimer::singleShot(500, this, SLOT(onDisconnect()));
			return;
		}
#endif

	} else if(m_conversation->currentRole() == Communicator::Send) {
		sendMessage();

	} else {
		onRead();
	}
}

void Communicator::onError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "Connection error" << socketError;

	if(!m_conversation || (m_conversation && !m_conversation->isDone()))
	{
		emit finished(ConnectionFailed);
		this->deleteLater();
	}
}

void Communicator::onConnect()
{
	if(encryption == Communicator::None
		&& !m_conman->isAuthenticated(Communicator::Send, m_conversation->authenticate(), m_peerCertificate, peerAddress()))
	{
		qDebug() << "Authentication failed" << m_conversation->type() << m_conversation->authenticate();
		emit finished(NotAuthenticated);
		this->deleteLater();
	}

	if(m_conversation->currentRole() == Communicator::Send)
		sendMessage();
}

void Communicator::onEncrypted()
{
	m_peerCertificate = peerCertificate();

	if(m_conversation && !m_conman->isAuthenticated(Communicator::Send, m_conversation->authenticate(), m_peerCertificate, peerAddress()))
	{
		qDebug() << "Authentication failed" << m_conversation->type() << m_conversation->authenticate();
		emit finished(NotAuthenticated);
		this->deleteLater();
		return;
	}
}

void Communicator::onRead()
{
	QByteArray data;

	while(bytesAvailable() > 0)
	{
		data = read(4096);
		dataRead += data.size();
		buffer.append(data);
	}

	if(!haveHeader && dataRead >= HEADER_SIZE)
	{
		readHeader();
		haveHeader = true;
	}

	if(dataRead >= msgLen && msgLen != 0)
		receiveMessage();
}

void Communicator::onDisconnect()
{
	qDebug() << "Communicator::onDisconnect";

	if(m_runPostDone)
		m_conversation->postDone();

	emit finished(Ok);
	this->deleteLater();
}

void Communicator::conversationDone()
{

}

void Communicator::morphConversation(Conversation *c)
{
	m_conversation->deleteLater();

	m_conversation = c;
	m_conversation->setParent(this);

	conversationSignals();
}

void Communicator::onSslError(const QList<QSslError> &errors)
{
	QList<QSslError::SslError> recoverable;
	recoverable << QSslError::SelfSignedCertificate
		<< QSslError::CertificateUntrusted
		<< QSslError::HostNameMismatch
		<< QSslError::NoError;

	foreach(QSslError e, errors)
	{
		if(!recoverable.contains(e.error()))
		{
			qDebug() << "Unrecoverable SSL error" << e;
			emit sslFatalError(errors);
			emit finished(UnrecoverableSslError);
			this->deleteLater();
			return;
		}
	}

	ignoreSslErrors();
}
