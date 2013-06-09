#include <QBuffer>
#include <QImage>
#include <QDomDocument>
#include <QTextCodec>

#include "Distributor.h"

Distributor::Distributor(HaveClip::Node *node, QObject *parent) :
	QTcpSocket(parent),
	node(node)
{
	connect(this, SIGNAL(connected()), this, SLOT(onConnect()));
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

void Distributor::distribute(const ClipboardContent *content)
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

	connectToHost(node->addr, node->port);

	/**
	  We cannot write data immediately due to a bug in Qt which will cause application to end up
	  in infinite loop when connection fails.
	  */
}

void Distributor::onError(QAbstractSocket::SocketError socketError)
{
	qDebug() << "Unable to reach" << node->addr << ":" << socketError;
	this->deleteLater();
}

void Distributor::onConnect()
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

void Distributor::onDisconnect()
{
	this->deleteLater();
}
