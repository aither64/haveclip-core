#include <QStringList>
#include <QImage>

#include "Client.h"
#include "Distributor.h"

Client::Client(QObject *parent) :
	QTcpSocket(parent),
	len(0),
	dataRead(0),
	type(HaveClip::Unknown)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(onRead()));
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
}

void Client::communicate()
{

}

void Client::onRead()
{
	QByteArray data;

	while(bytesAvailable() > 0)
	{
		data = read(4096);
		dataRead += data.size();
		buffer.append(data);
	}
}

void Client::onDisconnect()
{
	QVariant data;
	QString str;
	Distributor::Protocol msg_type;
	HaveClip::MimeType mime_type;

	int pos, startPos, rawType;
	bool ok;

	// Message type
	if((pos = buffer.indexOf(':')) == -1) {
		qDebug() << "Bad format - missing message type";
		return;
	}

	rawType = buffer.left(pos).toInt(&ok);

	if(!ok)
	{
		qDebug() << "Bad format - wrong number format";
		return;
	}

	msg_type = (Distributor::Protocol) rawType;
	startPos = pos + 1;

	// Mime type
	if((pos = buffer.indexOf(':', startPos)) == -1) {
		qDebug() << "Bad format - missing content type";
		return;
	}

	rawType = buffer.mid(startPos, pos - startPos).toInt(&ok);

	if(!ok)
	{
		qDebug() << "Bad format - wrong number format";
		return;
	}

	mime_type = (HaveClip::MimeType) rawType;
	startPos = pos + 1;

	// Contents
	QByteArray content = buffer.mid(startPos);

	switch(mime_type)
	{
	case HaveClip::Text:
	case HaveClip::Html: {
		const char *s = content.data();
		data = QString::fromUtf8(s);
		break;
	}
	case HaveClip::Urls: {
		const char *s = content.data();
		str = QString::fromUtf8(s);
		data = str.split("\n");
		break;
	}
	case HaveClip::ImageData:
		data = QImage::fromData(content);
		break;
	default:break;
	}

	emit clipboardUpdated(mime_type, data);
	this->deleteLater();
}
