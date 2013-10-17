#include "ClipboardUpdateSend.h"

#include <QTextCodec>

ClipboardUpdateSend::ClipboardUpdateSend(ClipboardContainer *cont, Communicator::Role r)
	: NetworkCommand(cont, r)
{
}

NetworkCommand::Type ClipboardUpdateSend::type() const
{
	return NetworkCommand::ClipboardUpdateSend;
}

void ClipboardUpdateSend::receive(QDataStream &ds)
{
	qint32 mode;
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

	ClipboardItem *item = new ClipboardItem(
		(ClipboardItem::Mode) mode,
		mimedata
	);
	item->init();

	m_cont = item;

	finish();
}

void ClipboardUpdateSend::send(QDataStream &ds)
{
	ClipboardItem *item = static_cast<ClipboardItem*>(m_cont);

	ds << (qint32) item->mode;
	ds << item->mimeData()->formats();

	foreach(QString mimetype, item->mimeData()->formats())
	{
		if(mimetype == "text/html")
		{
			QByteArray tmp = item->mimeData()->data("text/html");

			QTextCodec *codec = QTextCodec::codecForHtml(tmp, QTextCodec::codecForName("utf-8"));
			ds << codec->toUnicode(tmp).toUtf8();

		} else
			ds << item->mimeData()->data(mimetype);
	}

	finish();
}

