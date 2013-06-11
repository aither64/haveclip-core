#include <QStringList>
#include <QImage>
#include <QDomDocument>

#include "Receiver.h"
#include "Sender.h"

Receiver::Receiver(QObject *parent) :
	QTcpSocket(parent),
	len(0),
	dataRead(0)
{
	connect(this, SIGNAL(readyRead()), this, SLOT(onRead()));
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
}

void Receiver::communicate()
{

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
