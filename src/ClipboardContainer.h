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

#ifndef CLIPBOARDCONTAINER_H
#define CLIPBOARDCONTAINER_H

#include <QList>
#include <QMimeData>
#include <QClipboard>

class ClipboardItem;

class ClipboardContainer
{
public:
	enum ItemType {
		BasicItem=0,
		SerialBatch
	};

	enum Mode {
		Selection=1,
		Clipboard=2,
		FindBuffer=4,
		ClipboardAndSelection=8
	};

	Mode mode;
	QString title;

	ClipboardContainer();
	virtual ItemType type() const = 0;
	virtual ClipboardItem* item() = 0;
	virtual bool hasNext() const = 0;
	virtual ClipboardItem* nextItem() = 0;
	virtual bool hasPrevious() const = 0;
	virtual ClipboardItem* previousItem() = 0;
	virtual void addItem(ClipboardItem *item, bool allowDuplicity) = 0;
	virtual void seal() = 0;
	virtual bool isSealed() const = 0;
	virtual QList<ClipboardItem*> items() = 0;
	virtual void save(QDataStream &ds) const = 0;
	static ClipboardContainer* load(QDataStream &ds);
	static bool compareMimeData(const QMimeData *data1, const QMimeData *data2, bool isSelection);
	static Mode qtModeToOwn(QClipboard::Mode m);
	static QClipboard::Mode ownModeToQt(Mode m);

protected:
	void saveType(QDataStream &ds) const;
};

inline QDataStream& operator<<(QDataStream &ds, const ClipboardContainer &cnt)
{
	cnt.save(ds);

	return ds;
}

Q_DECLARE_METATYPE(ClipboardContainer*)

#endif // CLIPBOARDCONTAINER_H
