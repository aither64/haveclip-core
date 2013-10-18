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

#include "ClipboardContainer.h"

#include <QStringList>
#include <QByteArray>
#include <QDebug>

#include "ClipboardManager.h"
#include "ClipboardItem.h"
#include "ClipboardSerialBatch.h"

ClipboardContainer::ClipboardContainer()
{
}

ClipboardContainer* ClipboardContainer::load(QDataStream &ds)
{
	qint32 ctype;

	ds >> ctype;

	switch(ctype)
	{
	case ClipboardContainer::BasicItem: {
		qint32 mode;
		QStringList formats;
		QMimeData *md = new QMimeData();
		QByteArray data;

		ds >> mode;
		ds >> formats;

		foreach(QString f, formats)
		{
			ds >> data;
			md->setData(f, data);
		}

		ClipboardItem *it = new ClipboardItem((ClipboardItem::Mode) mode, md);
		it->init();

		return it;
	}

#ifdef INCLUDE_SERIAL_MODE
	case ClipboardContainer::SerialBatch: {
		ClipboardContainer *cnt;
		quint32 count;
		qint64 id;

		ds >> id;

		cnt = new ClipboardSerialBatch(id);

		ds >> count;

		for(int i = 0; i < count; i++)
		{
			ClipboardContainer *it = load(ds);

			if(it)
				cnt->addItem(it->item(), true);
		}

		return cnt;
	}
#endif // INCLUDE_SERIAL_MODE

	default:
		qDebug() << "Failed to load container from history: unknown type" << ctype;
		return 0;
	}
}

bool ClipboardContainer::compareMimeData(const QMimeData *data1, const QMimeData *data2, bool isSelection)
{
	if(isSelection)
	{
		if(data1->hasText() != data2->hasText())
			return false;

		if(data1->hasText() && data1->text() != data2->text())
			return false;

		return true;

	} else {
		QStringList formats1, formats2;

		foreach(QString f, data1->formats())
			if(f.indexOf('/') != -1)
				formats1 << f;

		foreach(QString f, data2->formats())
			if(f.indexOf('/') != -1)
				formats2 << f;

		if(formats1 != formats2)
			return false;

		foreach(QString f, formats1)
			if(data1->data(f) != data2->data(f))
				return false;

		return true;
	}
}

ClipboardContainer::Mode ClipboardContainer::qtModeToOwn(QClipboard::Mode m)
{
	switch(m)
	{
	case QClipboard::Selection:
		return Selection;
	case QClipboard::Clipboard:
		return Clipboard;
	case QClipboard::FindBuffer:
		return FindBuffer;
	}
}

QClipboard::Mode ClipboardContainer::ownModeToQt(Mode m)
{
	switch(m)
	{
	case Selection:
		return QClipboard::Selection;
	case Clipboard:
		return QClipboard::Clipboard;
	case FindBuffer:
		return QClipboard::FindBuffer;
	default:
		return QClipboard::Clipboard;
	}
}

void ClipboardContainer::saveType(QDataStream &ds) const
{
	ds << (qint32) type();
}
