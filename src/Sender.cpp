#include <QBuffer>
#include <QImage>
#include <QDomDocument>
#include <QTextCodec>

#include "Sender.h"

Sender::Sender(HaveClip::Encryption enc, HaveClip::Node *node, QObject *parent) :
	QSslSocket(parent),
	node(node),
	encryption(enc)
{
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(this, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslError(QList<QSslError>)));
}

void Sender::distribute(const ClipboardContent *content)
{
	/**
	  XML protocol
	  <haveclip>
		<password>1234</password>
		<clipboard>
			<mimedata mimetype="text/plain">base64 encoded data</mimedata>
			...
		</clipboard>
	  </haveclip>
	  */

	this->content = content;

	if(encryption != HaveClip::None)
	{
		connect(this, SIGNAL(encrypted()), this, SLOT(onConnect()));

		setPeerVerifyMode(QSslSocket::VerifyNone);

		switch(encryption)
		{
		case HaveClip::Ssl:
			setProtocol(QSsl::SslV3);
			break;
		case HaveClip::Tls:
			setProtocol(QSsl::TlsV1);
			break;
		}

		connectToHostEncrypted(node->host, node->port);
	} else {
		connect(this, SIGNAL(connected()), this, SLOT(onConnect()));

		connectToHost(node->host, node->port);
	}

	/**
	  We cannot write data immediately due to a bug in Qt which will cause application to end up
	  in infinite loop when connection fails.
	  */
}

void Sender::onError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "Unable to reach" << node->host << ":" << socketError;
	this->deleteLater();
}

void Sender::onConnect()
{
	QDomDocument doc;
	QDomElement root = doc.createElement("haveclip");
	doc.appendChild(root);

	QDomElement clip = doc.createElement("clipboard");
	root.appendChild(clip);

	foreach(QString mimetype, content->mimeData->formats())
	{
		QDomElement mimedata = doc.createElement("mimedata");
		mimedata.setAttribute("mimetype", mimetype);

		QDomText text;
		QByteArray data;

		if(mimetype == "text/html")
		{
			QByteArray tmp = content->mimeData->data("text/html");

			QTextCodec *codec = QTextCodec::codecForHtml(tmp, QTextCodec::codecForName("utf-8"));
			data = codec->toUnicode(tmp).toUtf8();
		} else
			data = content->mimeData->data(mimetype);

//		qDebug() << mimetype << data;

		text = doc.createTextNode(data.toBase64());
		mimedata.appendChild(text);

		clip.appendChild(mimedata);
	}

	QByteArray ba = doc.toByteArray();

	qDebug() << "Distributing" << ba.size() << "bytes";
	write(ba);

	disconnectFromHost();
}

void Sender::onDisconnect()
{
	this->deleteLater();
}

void Sender::onSslError(const QList<QSslError> &errors)
{
	qDebug() << "SENDER SSL error" << errors;

	ignoreSslErrors();
}
