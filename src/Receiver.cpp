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

	ClipboardContent *content = new ClipboardContent(mimedata);
	content->init();

	emit clipboardUpdated(content);
	this->deleteLater();
}

void Receiver::onSslError(const QList<QSslError> &errors)
{
	qDebug() << "RECEIVER SSL error" << errors;

	ignoreSslErrors();
}
