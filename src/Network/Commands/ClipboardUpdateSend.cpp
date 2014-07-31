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

#include "ClipboardUpdateSend.h"

#include <QTextCodec>

using namespace Commands;

ClipboardUpdateSend::ClipboardUpdateSend(ClipboardContainer *cont, Communicator::Role r)
	: BaseCommand(cont, r)
{
}

BaseCommand::Type ClipboardUpdateSend::type() const
{
	return BaseCommand::ClipboardUpdateSend;
}

void ClipboardUpdateSend::receive(QDataStream &ds)
{
	qint32 mode;
	ds >> mode;

	QStringList formats;
	ds >> formats;

	initRx(m_recvFilters, &m_recvRx);

	QMimeData *mimedata = new QMimeData();

	foreach(const QString &f, formats)
	{
		QByteArray tmp;
		ds >> tmp;

		if(shouldReceive(f))
		{
			qDebug() << "Receive" << f;
			mimedata->setData(f, tmp);
		} else {
			qDebug() << "Drop" << f;
		}
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

	initRx(m_sendFilters, &m_sendRx);
	initRx(m_recvFilters, &m_recvRx);

	QStringList formatsToSend;

	foreach(const QString &f, item->mimeData()->formats())
	{
		if(shouldSend(f))
		{
			qDebug() << "Send" << f;
			formatsToSend << f;
		} else {
			qDebug() << "Drop" << f;
		}
	}

	ds << (qint32) item->mode;
	ds << formatsToSend;

	foreach(const QString &mimetype, formatsToSend)
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

void ClipboardUpdateSend::setSendFilters(Settings::MimeFilterMode mode, const QStringList &filters)
{
	m_sendMode = mode;
	m_sendFilters = filters;
}

void ClipboardUpdateSend::setRecvFilters(Settings::MimeFilterMode mode, const QStringList &filters)
{
	m_recvMode = mode;
	m_recvFilters = filters;
}

void ClipboardUpdateSend::initRx(const QStringList &what, QList<QRegExp> *where)
{
	where->clear();

	foreach(const QString &s, what)
		*where << QRegExp(s);
}

bool ClipboardUpdateSend::shouldSend(const QString &mimeType)
{
	bool ret = m_recvMode == Settings::Accept ? false : true;

	foreach(const QRegExp &rx, m_sendRx)
	{
		if(rx.exactMatch(mimeType))
		{
			ret = m_sendMode == Settings::Accept ? true : false;
			break;
		}
	}

	return ret && shouldReceive(mimeType);
}

bool ClipboardUpdateSend::shouldReceive(const QString &mimeType)
{
	bool ret = m_recvMode == Settings::Accept ? false : true;

	foreach(const QRegExp &rx, m_recvRx)
	{
		if(rx.exactMatch(mimeType))
		{
			ret = m_recvMode == Settings::Accept ? true : false;
			break;
		}
	}

	return ret;
}

