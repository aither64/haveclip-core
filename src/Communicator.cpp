#include "Communicator.h"
#include "Conversations/ClipboardUpdate.h"

Communicator::Communicator(QObject *parent) :
	QSslSocket(parent),
	m_conversation(0),
	haveHeader(false),
	msgLen(0)
{
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

void Communicator::setCertificateAndKey(QString cert, QString key)
{
	setLocalCertificate(cert);
	setPrivateKey(key);
}

void Communicator::setPassword(QString password)
{
	m_password = password;
}

void Communicator::sendMessage()
{
	QByteArray buf;
	QDataStream ds(&buf, QIODevice::WriteOnly);

	ds << (quint32) PROTO_MAGIC_NUMBER;
	ds << (qint32) PROTO_VERSION;
	ds << (qint32) m_conversation->type();
	ds << (qint32) m_conversation->currentCommandType();
	ds << (quint64) 0; // Filled later

	ds << m_password;

	m_conversation->send(ds);

	ds.device()->seek(16); // seek to message length field

	ds << (quint64) buf.size();

	qDebug() << "Send message" << m_conversation->currentCommandType() << buf.size() << "bytes";

	write(buf);
}

void Communicator::receiveMessage()
{
	qDebug() << "receiveMessage: buffer" << buffer.size() << "msgLen" << msgLen;

	QDataStream ds(&buffer, QIODevice::ReadOnly);
	ds.device()->seek(HEADER_SIZE); // skip header

	QString msgPassword;

	ds >> msgPassword;

	if(msgPassword != m_password)
	{
		qDebug() << "Password does not match!";
		this->deleteLater();
		return;
	}

	if(m_conversation->currentRole() != Communicator::Receive)
	{
		qDebug() << "Fuckup";
		this->deleteLater();
		return;
	}

	m_conversation->receive(ds);

	buffer.remove(0, msgLen);

	qDebug() << "Deleting message from buffer, size = " << buffer.size();

	haveHeader = false;
	msgLen = 0;
	dataRead = 0;

	continueConversation();
}

void Communicator::readHeader()
{
	if(m_conversation)
		qDebug() << "Read header start" << m_conversation->currentCommandType() << m_conversation->currentRole();

	QDataStream ds(&buffer, QIODevice::ReadOnly);

	quint32 magic;
	qint32 version, convType, cmdType;

	if(buffer.size() < HEADER_SIZE)
	{
		qDebug() << "Invalid message - incomplete header";
		this->deleteLater();
		return;
	}

	ds >> magic;

	if(magic != PROTO_MAGIC_NUMBER)
	{
		qDebug() << "Invalid message - magic number does not match";
		this->deleteLater();
		return;
	}

	ds >> version;

	if(version != PROTO_VERSION)
	{
		qDebug() << "Protocol version does not match. Supported is" << PROTO_VERSION << ", received" << version;
		this->deleteLater();
		return;
	}

	ds >> convType;

	if(!m_conversation) // Receiver must wait for sender to specify conversation type in first msg
	{
		switch(convType)
		{
		case Conversation::ClipboardUpdate:
			qDebug() << "Initiating conversation ClipboardUpdate";
			m_conversation = new ClipboardUpdate(Communicator::Receive, 0);
			break;

		default:
			qDebug() << "Unknown conversation" << convType;
			this->deleteLater();
			return;
		}

		conversationSignals();

	} else if(convType != m_conversation->type()) {
		qDebug() << "Invalid conversation: expected" << m_conversation->type() << ", received" << convType;
		this->deleteLater();
		return;
	}

	ds >> cmdType;

	if(cmdType != m_conversation->currentCommandType())
	{
		qDebug() << "Unexpected message type: expected" << m_conversation->currentCommandType() << ", received" << cmdType;
		this->deleteLater();
		return;
	}

	ds >> msgLen;

	qDebug() << "Header read, msg len" << msgLen;
}

void Communicator::conversationSignals()
{
	qDebug() << "Communicator::conversationSignals() called";

	connect(m_conversation, SIGNAL(done()), this, SLOT(conversationDone()));
}

void Communicator::continueConversation()
{
	if(m_conversation->isDone())
		disconnectFromHost();

	else if(m_conversation->currentRole() == Communicator::Send)
		sendMessage();
}

void Communicator::onError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "Connection error" << socketError;
	this->deleteLater();
}

void Communicator::onConnect()
{
	if(m_conversation->currentRole() == Communicator::Send)
		sendMessage();
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
	this->deleteLater();
}

void Communicator::conversationDone()
{

}

void Communicator::onSslError(const QList<QSslError> &errors)
{
	ignoreSslErrors();
}
