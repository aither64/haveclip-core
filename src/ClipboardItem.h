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

#ifndef CLIPBOARDCONTENT_H
#define CLIPBOARDCONTENT_H

#include <QClipboard>
#include <QMimeData>
#include <QIcon>

#include "ClipboardContainer.h"

class ClipboardItem : public ClipboardContainer
{
public:
	struct Preview
	{
		QString path;
		int width;
		int height;

		~Preview();
	};

	QString excerpt;
	QIcon icon;
	Preview *preview;
	QStringList formats;

	ClipboardItem(Mode m, QMimeData *data);
	virtual ~ClipboardItem();
	virtual void init();
	virtual ItemType type() const;
//	virtual Mode mode();
//	virtual QString title();
//	virtual QString excerpt();
//	virtual QIcon icon();
//	virtual Preview* preview();
//	virtual QStringList formats();
	virtual ClipboardItem* item();
	virtual bool hasNext() const;
	virtual ClipboardItem* nextItem();
	virtual bool hasPrevious() const;
	virtual ClipboardItem* previousItem();
	virtual void addItem(ClipboardItem *item, bool allowDuplicity);
	virtual void seal();
	virtual bool isSealed() const;
	virtual QList<ClipboardItem*> items();
	QMimeData* mimeData();
	QString toPlainText();
	bool operator==(const ClipboardItem &other) const;
	bool operator!=(const ClipboardItem &other) const;
	virtual void save(QDataStream &ds) const;

protected:
	ClipboardItem();

	QMimeData *m_mimeData;

private:
//	Mode m_mode;
//	QString m_title;
//	QString m_excerpt;
//	QIcon m_icon;
//	Preview *m_preview;
//	QStringList m_formats;

	Preview* createItemPreview(QImage &img);
	void setTitle(QString &str);
	QString escape(QString str);
};

#endif // CLIPBOARDCONTENT_H
