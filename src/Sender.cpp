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

#include "Sender.h"

Sender::Sender(ClipboardManager::Encryption enc, ClipboardManager::Node *node, QObject *parent) :
	QSslSocket(parent),
	m_node(node),
	encryption(enc),
	deleteContent(false)
{
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(this, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslError(QList<QSslError>)));
}

void Sender::distribute(const ClipboardContent *content, QString password)
{
	/**
	  Binary protocol

	  length  type                       meaning
	   4      uint32                     magic number, identifies correct data format
	   4      int32                      version
	   4      int32                      message type
	   8      quint64                    message length
	   -      QString                    password

	  for message type = ClipboardUpdate:
	   4      int32                      mode (clipboard, selection, both)
	   -      QStringList                formats
	   -      QByteArray * formats.size  mime data
	   -      QStringList                file URLs

	  for message type = FileRead:
	   -      QString                    file name
	   8      quint64                    offset
	   8      quint64                    number of bytes to read starting from offset
	  */

	this->content = content;
	this->password = password;

	if(encryption != ClipboardManager::None)
	{
		setPeerVerifyMode(QSslSocket::VerifyNone);

		connect(this, SIGNAL(encrypted()), this, SLOT(onConnect()));

		switch(encryption)
		{
		case ClipboardManager::Ssl:
			setProtocol(QSsl::SslV3);
			break;
		case ClipboardManager::Tls:
			setProtocol(QSsl::TlsV1);
			break;
		}

		connectToHostEncrypted(m_node->host, m_node->port);

	} else {
		connect(this, SIGNAL(connected()), this, SLOT(onConnect()));

		connectToHost(m_node->host, m_node->port);
	}

	/**
	  We cannot write data immediately due to a bug in Qt which will cause application to end up
	  in infinite loop when connection fails.
	  */
}

void Sender::setDeleteContentOnSent(bool del)
{
	deleteContent = del;
}

void Sender::onError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "Unable to reach" << m_node->host << ":" << socketError;
	this->deleteLater();
}

void Sender::onConnect()
{
	QByteArray buf;
	QDataStream ds(&buf, QIODevice::WriteOnly);

	ds << (quint32) PROTO_MAGIC_NUMBER;
	ds << (qint32) PROTO_VERSION;
	ds << (qint32) ClipboardSync;
	ds << (quint64) 0; // Filled later

	ds << password;

	ds << (qint32) content->mode;
	ds << content->mimeData->formats();

	foreach(QString mimetype, content->mimeData->formats())
	{
		if(mimetype == "text/html")
		{
			QByteArray tmp = content->mimeData->data("text/html");

			QTextCodec *codec = QTextCodec::codecForHtml(tmp, QTextCodec::codecForName("utf-8"));
			ds << codec->toUnicode(tmp).toUtf8();

		} else
			ds << content->mimeData->data(mimetype);
	}

	ds.device()->seek(12); // seek to message length field

	qDebug() << "Buf size before setting size" << buf.size();

	ds << (quint64) buf.size();

	qDebug() << "Distributing" << buf.size() << "bytes";

	write(buf);

	disconnectFromHost();
}

void Sender::onDisconnect()
{
	if(deleteContent)
		delete content;

	this->deleteLater();
}

void Sender::onSslError(const QList<QSslError> &errors)
{
	QList<QSslError::SslError> recoverable;
	recoverable << QSslError::SelfSignedCertificate
		<< QSslError::CertificateUntrusted
		<< QSslError::HostNameMismatch
		<< QSslError::CertificateExpired;

	bool exception = true;

	foreach(QSslError e, errors)
	{
		if(!recoverable.contains(e.error()))
		{
			qDebug() << "Unrecoverable SSL error" << e;
			emit sslFatalError(errors);
			return;
		}

		if(e.certificate() != m_node->certificate)
		{
			exception = false;
			break;
		}
	}

	if(exception)
	{
		qDebug() << "SSL errors ignored because of exception";
		ignoreSslErrors();

	} else {
		emit untrustedCertificateError(m_node, errors);
	}
}

ClipboardManager::Node* Sender::node()
{
	return m_node;
}
