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

#include "ClipboardSerialBatch.h"

#ifdef INCLUDE_SERIAL_MODE
#include <QDebug>
#include <QDateTime>

ClipboardSerialBatch::ClipboardSerialBatch(qint64 id) :
	ClipboardContainer(),
	m_id(id),
	m_index(0),
	m_sealed(false)
{
	title = QObject::tr("< Serial batch >");
}

ClipboardSerialBatch::ClipboardSerialBatch(qint64 id, ClipboardItem *item) :
	ClipboardSerialBatch(id)
{
	m_items << item;
}

void ClipboardSerialBatch::init()
{
	if(m_items.size())
		m_items[m_index]->init();
}

ClipboardSerialBatch::~ClipboardSerialBatch()
{
	qDeleteAll(m_items);
}

ClipboardContainer::ItemType ClipboardSerialBatch::type() const
{
	return SerialBatch;
}

ClipboardItem* ClipboardSerialBatch::item()
{
	if(!m_items.size())
		return 0;

	return m_items[m_index];
}

bool ClipboardSerialBatch::hasNext() const
{
	return m_index < (m_items.count() - 1);
}

ClipboardItem* ClipboardSerialBatch::nextItem()
{
	return m_items[++m_index];
}

bool ClipboardSerialBatch::hasPrevious() const
{
	return m_index > 0;
}

ClipboardItem* ClipboardSerialBatch::previousItem()
{
	return m_items[--m_index];
}

void ClipboardSerialBatch::addItem(ClipboardItem *item, bool allowDuplicity)
{
	if(m_items.size())
	{
		ClipboardItem *last = m_items.last();

		if(*last == *item)
		{
			if(!allowDuplicity)
			{
				delete item;
				return;

			} else if(last->mode != ClipboardItem::ClipboardAndSelection && last->mode != item->mode) {
				last->mode = ClipboardItem::ClipboardAndSelection;
				delete item;
				return;
			}
		}
	}

	m_items << item;
}

void ClipboardSerialBatch::seal()
{
	m_sealed = true;
}

bool ClipboardSerialBatch::isSealed() const
{
	return m_sealed;
}

QList<ClipboardItem*> ClipboardSerialBatch::items()
{
	return m_items;
}

void ClipboardSerialBatch::save(QDataStream &ds) const
{
	saveType(ds);

	ds << m_id;
	ds << (quint32) m_items.count();

	foreach(ClipboardItem* it, m_items)
		ds << *it;
}

qint64 ClipboardSerialBatch::id() const
{
	return m_id;
}

int ClipboardSerialBatch::currentIndex() const
{
	return m_index;
}

int ClipboardSerialBatch::count() const
{
	return m_items.count();
}

qint64 ClipboardSerialBatch::createId()
{
	return QDateTime::currentMSecsSinceEpoch();
}

#endif // INCLUDE_SERIAL_MODE
