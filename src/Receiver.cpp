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

#include <QStringList>
#include <QImage>
#include <QDataStream>

#include "Receiver.h"
#include "Sender.h"

Receiver::Receiver(ClipboardManager::Encryption enc, QObject *parent) :
	QSslSocket(parent),
	len(0),
	dataRead(0),
	encryption(enc)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(onRead()));
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	connect(this, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslError(QList<QSslError>)));
}

void Receiver::communicate()
{
	if(encryption != ClipboardManager::None)
	{
		switch(encryption)
		{
		case ClipboardManager::Ssl:
			setProtocol(QSsl::SslV3);
			break;
		case ClipboardManager::Tls:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			setProtocol(QSsl::TlsV1_0);
#else
			setProtocol(QSsl::TlsV1);
#endif
			break;
		}

		setPeerVerifyMode(QSslSocket::VerifyNone);

		startServerEncryption();
	}
}

void Receiver::setCertificateAndKey(QString cert, QString key)
{
	setLocalCertificate(cert);
	setPrivateKey(key);
}

void Receiver::setAcceptPassword(QString password)
{
	m_password = password;
}

void Receiver::onRead()
{
	QByteArray data;

	while(bytesAvailable() > 0)
	{
		data = read(4096);
		dataRead += data.size();
		buffer.append(data);
	}
}

void Receiver::onDisconnect()
{
	QDataStream ds(&buffer, QIODevice::ReadOnly);

	quint32 magic;
	qint32 version, type, mode;
	quint64 length;
	QString password;

	if(buffer.size() < 20)
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

	ds >> type;

	if(type != Sender::ClipboardSync)
	{
		qDebug() << "Unsupported message type" << type;
		this->deleteLater();
		return;
	}

	ds >> length;

	if(length != buffer.size())
	{
		qDebug() << "Message size does not match! Message says" << length << ", received" << buffer.size();
		qDebug() << "Clipboard might be corrupted";
	}

	ds >> password;

	if(password != m_password)
	{
		qDebug() << "Password does not match!";
		this->deleteLater();
		return;
	}

	ds >> mode;

	QStringList formats;
	ds >> formats;

	QMimeData *mimedata = new QMimeData();

	foreach(QString f, formats)
	{
		QByteArray tmp;
		ds >> tmp;

		mimedata->setData(f, tmp);
	}

	ClipboardItem *content = new ClipboardItem(
		(ClipboardItem::Mode) mode,
		mimedata
	);
	content->init();

	emit clipboardUpdated(content);
	this->deleteLater();
}

void Receiver::onSslError(const QList<QSslError> &errors)
{
	qDebug() << "RECEIVER SSL error" << errors;

	ignoreSslErrors();
}
