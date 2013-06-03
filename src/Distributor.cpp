#include <QBuffer>
#include <QImage>

#include "Distributor.h"

Distributor::Distributor(HaveClip::Node *node, QObject *parent) :
	QTcpSocket(parent),
	node(node)
{
	connect(this, SIGNAL(connected()), this, SLOT(onConnect()));
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

void Distributor::distribute(HaveClip::MimeType type, QVariant data)
{
	/**
	  Protocol:
	    message_type:content_type:data

	  Where:
	    : - compulsory separator
	    message_type: 1 - sync clipboard
	    content_type: 0 - plain text
			  1 - HTML
			  2 - URLs, separated by \n
			  3 - image data
	    data: usually contents of clipboard, depends on message_type

	  For now only one message per connection is supported.
	  */

	this->type = type;
	this->data = data;

	connectToHost(node->addr, node->port);

	/**
	  We cannot write data immediately due to a bug in Qt which will cause application to end up in infinite loop
	  */
}

void Distributor::onError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "Unable to reach" << node->addr << ":" << socketError;
	this->deleteLater();
}

void Distributor::onConnect()
{
	QByteArray ba;
	QString msg("%1:%2:");

	ba.append(msg.arg(Distributor::CLIPBOARD_SYNC).arg(type).toUtf8());

	switch(type)
	{
	case HaveClip::Text:
	case HaveClip::Html:
		ba.append(data.toString().toUtf8());
		break;
	case HaveClip::Urls:
		ba.append(data.toStringList().join("\n").toUtf8());
		break;
	case HaveClip::ImageData: {
		QByteArray tmp;
		QBuffer buffer(&tmp);
		buffer.open(QIODevice::WriteOnly);
		data.value<QImage>().save(&buffer, "PNG");
		ba.append(tmp);
		break;
	}
	default:
		break;
	}

	qDebug() << "Distributing" << ba.size() << "bytes: " << ba;
	write(ba);

	disconnectFromHost();
}

void Distributor::onDisconnect()
{
	this->deleteLater();
}
