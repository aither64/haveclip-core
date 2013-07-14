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
#include <QDomDocument>

#include "Receiver.h"
#include "Sender.h"

Receiver::Receiver(HaveClip::Encryption enc, QObject *parent) :
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
	if(encryption != HaveClip::None)
	{
		switch(encryption)
		{
		case HaveClip::Ssl:
			setProtocol(QSsl::SslV3);
			break;
		case HaveClip::Tls:
			setProtocol(QSsl::TlsV1);
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
	this->password = password;
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
	QDomDocument doc;

	if(!doc.setContent(buffer))
	{
		qDebug() << "Invalid message";
		return;
	}

	QDomElement root = doc.documentElement();

	if(!password.isEmpty())
	{
		QDomElement passwd = root.firstChildElement("password");

		if(passwd.text() != password)
		{
			qDebug() << "Password does not match! Ignore";
			this->deleteLater();
			return;
		}
	}

	QDomElement clipboard = root.firstChildElement("clipboard");
	QDomNode n = clipboard.firstChild();
	QMimeData *mimedata = new QMimeData();

	while(!n.isNull())
	{
		QDomElement e = n.toElement();

		if(!e.isNull())
			mimedata->setData(e.attribute("mimetype"), QByteArray::fromBase64( e.text().toAscii() ));

		n = n.nextSibling();
	}

	qDebug() << "clipboard.mode" << clipboard.attribute("mode");

	ClipboardContent *content = new ClipboardContent(
		clipboard.attribute("mode") == "selection" ? QClipboard::Selection : QClipboard::Clipboard,
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
