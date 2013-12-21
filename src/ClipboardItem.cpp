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

#include <QTextDocument>
#include <QUrl>
#include <QStringList>
#include <QFile>
#include <QTemporaryFile>
#include <QDir>
#include <QImage>
#include <QPixmap>
#include <QDebug>

#include "ClipboardItem.h"

ClipboardItem::Preview::~Preview()
{
	QFile::remove(path);
}

ClipboardItem::ClipboardItem() :
	ClipboardContainer(),
	preview(0)
{
}

ClipboardItem::ClipboardItem(ClipboardItem::Mode m, QMimeData *data) :
    ClipboardContainer(),
    preview(0)
{
	mode = m;
	m_mimeData = data;

	foreach(QString f, m_mimeData->formats())
	{
		if(f.indexOf('/') != -1)
			formats << f;
	}
}

void ClipboardItem::init()
{
	QString tmp;

	if(m_mimeData->hasText()) {
		tmp = m_mimeData->text();

#ifdef Q_OS_LINUX
		excerpt = escape(tmp.left(200));

		if(tmp.count() > 200)
			excerpt += "<br>...";
#endif

		setTitle(tmp);

	} else if(m_mimeData->hasHtml()) {
		tmp = m_mimeData->html();

#ifdef Q_OS_LINUX
		excerpt = escape(tmp.left(200));

		if(tmp.count() > 200)
			excerpt += "<br>...";
#endif

		setTitle(tmp);

	} else if(m_mimeData->hasUrls()) {
		QList<QUrl> urls = m_mimeData->urls();

#ifdef Q_OS_LINUX
		foreach(QUrl u, urls.mid(0, 5))
			excerpt += u.toString() + "<br>";

		if(urls.size() > 5)
			excerpt += "<br>...";
#endif

		if(urls.size() > 0)
			tmp = urls.first().toString();
		else tmp = QObject::tr("URLs");

		setTitle(tmp);

	} else if(m_mimeData->hasImage()) {
		// For some reason, QMimeData::imageData().value<QImage>() does not work on copied QMimeData
		// Loaded image is null
		QImage img;
		img.loadFromData(m_mimeData->data("application/x-qt-image"));

		excerpt = QObject::tr("Image");
		setTitle(excerpt);
		icon = QIcon( QPixmap::fromImage(img) );

#ifdef Q_OS_LINUX
		preview = createItemPreview(img);

		if(preview)
		{
			QString prop;

			if(preview->width > 400)
				prop = QString("width=\"%1\"").arg(400);
			else if(preview->height > 400)
				prop = QString("height=\"%1\"").arg(400);

			excerpt += QString("<br><img src=\"%1\" %2>").arg(preview->path).arg(prop);
		}
#endif
	} else {
		tmp = QObject::tr("Unknown content");
		setTitle(tmp);
	}
}

ClipboardItem::~ClipboardItem()
{
	if(preview)
		delete preview;

	delete m_mimeData;
}

ClipboardContainer::ItemType ClipboardItem::type() const
{
	return BasicItem;
}

//ClipboardItem::Mode ClipboardItem::mode()
//{
//	return m_mode;
//}

//QString ClipboardItem::title()
//{
//	return m_title;
//}

//QString ClipboardItem::excerpt()
//{
//	return m_excerpt;
//}

//QIcon ClipboardItem::icon()
//{
//	return m_icon;
//}

//ClipboardItem::Preview* ClipboardItem::preview()
//{
//	return m_preview;
//}

//QStringList ClipboardItem::formats()
//{
//	return m_formats;
//}

ClipboardItem* ClipboardItem::item()
{
	return this;
}

bool ClipboardItem::hasNext() const
{
	return false;
}

ClipboardItem* ClipboardItem::nextItem()
{
	return 0;
}

bool ClipboardItem::hasPrevious() const
{
	return false;
}

ClipboardItem* ClipboardItem::previousItem()
{
	return 0;
}

void ClipboardItem::addItem(ClipboardItem *item, bool allowDuplicity)
{

}

void ClipboardItem::seal()
{

}

bool ClipboardItem::isSealed() const
{
	return false;
}

QList<ClipboardItem*> ClipboardItem::items()
{
	return QList<ClipboardItem*>();
}

QMimeData* ClipboardItem::mimeData()
{
	return m_mimeData;
}

QString ClipboardItem::toPlainText()
{
	QString ret;

	if(m_mimeData->hasText())
		ret = m_mimeData->text();

	else if(m_mimeData->hasHtml())
		ret = m_mimeData->html();

	else if(m_mimeData->hasUrls()) {
		foreach(QUrl u, m_mimeData->urls())
			ret += u.toString() + "\n";
	}

	return ret;
}

bool ClipboardItem::operator==(const ClipboardItem &other) const
{
	if(mode == ClipboardContainer::Selection || other.mode == ClipboardContainer::Selection)
	{
		return compareMimeData(m_mimeData, other.m_mimeData, true);

	} else {
		if(formats != other.formats)
			return false;

		foreach(QString f, formats)
		{
			if(m_mimeData->data(f) != other.m_mimeData->data(f))
				return false;
		}

		return true;
	}
}

bool ClipboardItem::operator!=(const ClipboardItem &other) const
{
	return !(*this == other);
}

void ClipboardItem::save(QDataStream &ds) const
{
	saveType(ds);

	ds << (qint32) mode;
	ds << formats;

	foreach(QString f, formats)
		ds << m_mimeData->data(f);
}

ClipboardItem::Preview* ClipboardItem::createItemPreview(QImage &img)
{
	Preview *preview = 0;
	QTemporaryFile tmp(QDir::tempPath() + "/haveclip-preview-XXXXXX");
	tmp.setAutoRemove(false);

	if(tmp.open())
	{
		if( img.save(&tmp, "PNG") )
		{
			preview = new Preview;
			preview->path = tmp.fileName();
			preview->width = img.width();
			preview->height = img.height();
		}

		tmp.close();
	}

	return preview;
}

void ClipboardItem::setTitle(QString &str)
{
	QString s = str.trimmed(), tmp;
	int start = 0, end;

	while((end = s.indexOf('\n', start)) != -1)
	{
		tmp = s.mid(start, end).trimmed().left(30);

		if(!tmp.isEmpty())
		{
			title = tmp;
			break;
		}

		start = end + 1;
	}

	if(title.isEmpty())
		title = s.left(30);

	title.replace("&", "&&");

	if(str.count() > 30)
		title += "...";
}

QString ClipboardItem::escape(QString str)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	return str.toHtmlEscaped();
#else
	return Qt::escape(str);
#endif
}
