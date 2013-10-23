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

#ifndef CLIPBOARDSERIALBATCH_H
#define CLIPBOARDSERIALBATCH_H

#include "ClipboardManager.h"

#ifdef INCLUDE_SERIAL_MODE

class ClipboardSerialBatch : public ClipboardContainer
{
public:
	ClipboardSerialBatch(qint64 id);
	ClipboardSerialBatch(qint64 id, ClipboardItem *item);
	~ClipboardSerialBatch();
	virtual void init();
	virtual ItemType type() const;
	virtual ClipboardItem* item();
	virtual bool hasNext() const;
	virtual ClipboardItem* nextItem();
	virtual bool hasPrevious() const;
	virtual ClipboardItem* previousItem();
	virtual void addItem(ClipboardItem *item, bool allowDuplicity);
	virtual void seal();
	virtual bool isSealed() const;
	virtual QList<ClipboardItem*> items();
	virtual void save(QDataStream &ds) const;
	qint64 id() const;
	int currentIndex() const;
	int count() const;
	static qint64 createId();

private:
	QList<ClipboardItem*> m_items;
	int m_index;
	bool m_sealed;
	qint64 m_id;

};

#endif // INCLUDE_SERIAL_MODE

#endif // CLIPBOARDSERIALBATCH_H
