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

#include "History.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>

#ifdef INCLUDE_SERIAL_MODE
#include "ClipboardSerialBatch.h"
#endif

History::History(QObject *parent) :
	QObject(parent),
	m_track(true),
	m_save(false),
	m_size(30),
	m_currentContainer(0),
	m_serialInit(false)
{
}

void History::init()
{
	if(m_track)
		load();
	else
		deleteFile();
}

QList<ClipboardContainer*> History::items()
{
	return m_items;
}

ClipboardItem* History::currentItem()
{
	if(!m_currentContainer)
		return 0;

	return m_currentContainer->item();
}

ClipboardItem* History::lastItem()
{
	if(!m_currentContainer)
		return 0;

	QList<ClipboardItem*> items = m_currentContainer->items();

	if(!items.count())
		return currentItem();

	return items.last();
}

ClipboardContainer* History::currentContainer()
{
	return m_currentContainer;
}

bool History::isEnabled() const
{
	return m_track;
}

void History::setEnabled(bool enabled)
{
	m_track = enabled;
}

bool History::isSaving() const
{
	return m_save;
}

void History::setSave(bool save)
{
	m_save = save;
}

int History::stackSize() const
{
	return m_size;
}

void History::setStackSize(int size)
{
	m_size = size;
}

ClipboardItem* History::add(ClipboardItem *item, bool allowDuplicity)
{
	if(!m_track)
	{
		// FIXME: serial mode
		if(m_currentContainer)
			delete m_currentContainer;

		m_currentContainer = item;

		emit historyChanged();

		return m_currentContainer->item();
	}

	if(m_currentContainer)
	{
		switch(m_currentContainer->type())
		{
#ifdef INCLUDE_SERIAL_MODE
		case ClipboardItem::SerialBatch:
			if(!m_currentContainer->isSealed())
			{
				m_currentContainer->addItem(item, allowDuplicity);

				emit historyChanged();

				return m_currentContainer->item();
			}

			// do not add break
			// if sealed, add as BasicItem
#endif // INCLUDE_SERIAL_MODE

		case ClipboardItem::BasicItem:
#ifdef INCLUDE_SERIAL_MODE
			if(m_serialInit)
			{
				m_currentContainer = new ClipboardSerialBatch(m_serialBatchId, item);
				m_items << m_currentContainer;

				m_serialInit = false;
				m_serialBatchId = 0;

				if(m_items.size() >= m_size)
					delete m_items.takeFirst();

				emit historyChanged();

				return m_currentContainer->item();
			}
#endif // INCLUDE_SERIAL_MODE

			foreach(ClipboardContainer *cont, m_items)
			{
				ClipboardItem *it = cont->item();

				// Item with the same content is already in history
				if(*it == *item)
				{
					m_currentContainer = it;

					if(m_currentContainer->mode != item->mode)
						m_currentContainer->mode = ClipboardItem::ClipboardAndSelection;

					// These items are same but have different instances, remove duplicity
					if(it != item)
						delete item;

					popToFront(m_currentContainer);
					emit historyChanged();

					return m_currentContainer->item();
				}
			}

			if(m_items.size() >= m_size)
				delete m_items.takeFirst();

			m_items << item;

			break;
		}
	} else
		m_items << item;

	m_currentContainer = item;

	emit historyChanged();

	return m_currentContainer->item();
}

bool History::isNew(ClipboardItem *item) const
{

}

#ifdef INCLUDE_SERIAL_MODE
void History::beginSerialMode(qint64 id)
{
	m_serialInit = true;

	if(!id)
		id = ClipboardSerialBatch::createId();

	m_serialBatchId = id;

	qDebug() << "History: begun serial mode id = " << m_serialBatchId;
}

void History::endSerialMode()
{
	if(m_currentContainer->type() == ClipboardItem::SerialBatch)
	{
		m_currentContainer->seal();
		qDebug() << "History: end serial mode id = " << static_cast<ClipboardSerialBatch*>(m_currentContainer)->id();
	}

	m_serialInit = false;
}

qint64 History::preparedSerialbatchId() const
{
	return m_serialBatchId;
}

ClipboardSerialBatch* History::searchBatchById(qint64 id)
{
	foreach(ClipboardContainer *cont, m_items)
	{
		if(cont->type() != ClipboardContainer::SerialBatch)
			continue;

		ClipboardSerialBatch *s = static_cast<ClipboardSerialBatch*>(cont);

		if(s->id() == id)
			return s;
	}

	return 0;
}
#endif // INCLUDE_SERIAL_MODE

void History::load()
{
	if(!m_track)
		return;

	QFile file(filePath());

	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Unable to open history file for reading";
		return;
	}

	QDataStream ds(&file);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	ds.setVersion(QDataStream::Qt_5_1);
#else
	ds.setVersion(QDataStream::Qt_4_6);
#endif

	quint32 magic;
	qint32 version;

	ds >> magic;

	if(magic != HISTORY_MAGIC_NUMBER)
	{
		qDebug() << "Bad file format: magic number does not match";
		return;
	}

	ds >> version;

	if(version != HISTORY_VERSION)
	{
		qDebug() << "History file uses older format" << "(" << version << ")" << "than this version of HaveClip" << "(" << HISTORY_VERSION << ")";
		qDebug() << "Skipping history load";
		return;
	}

	while(!ds.atEnd())
	{
		ClipboardContainer *cnt = ClipboardContainer::load(ds);

		if(cnt)
			m_items << cnt;
	}

	file.close();
}

void History::save()
{
	if(!m_save)
		return;

	QFileInfo fi(filePath());
	QDir d;
	d.mkpath(fi.absolutePath());

	qDebug() << "Save history to" << fi.absoluteFilePath();

	QFile file(fi.absoluteFilePath());

	if(!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Unable to open history file for writing";
		return;
	}

	QDataStream ds(&file);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	ds.setVersion(QDataStream::Qt_5_1);
#else
	ds.setVersion(QDataStream::Qt_4_6);
#endif

	ds << (quint32) HISTORY_MAGIC_NUMBER;
	ds << (qint32) HISTORY_VERSION;

	// Saved from oldest to newest
	foreach(ClipboardContainer *cnt, m_items)
		ds << *cnt;

	file.close();
}

void History::clear()
{
	m_items.clear();
}

void History::deleteFile()
{
	QFile::remove(filePath());
}

void History::jumpTo(ClipboardItem* item)
{
	popToFront(item);
}

QString History::filePath()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	return QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/history.dat";
#else
	return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/history.dat";
#endif
}

void History::popToFront(ClipboardContainer *item)
{
	m_items.removeOne(item);
	m_items << item;
}
