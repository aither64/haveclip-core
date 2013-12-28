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

#ifndef HISTORY_H
#define HISTORY_H

#include <QAbstractListModel>

#include "ClipboardManager.h"
#include "ClipboardItem.h"

#ifdef INCLUDE_SERIAL_MODE
class ClipboardSerialBatch;
#endif

class History : public QAbstractListModel
{
	Q_OBJECT
public:
	enum HistoryRoles {
		PlainTextRole = Qt::UserRole + 1,
		ClipboardItemPointerRole
	};

	explicit History(QObject *parent = 0);
	void init();
	QHash<int, QByteArray> roleNames() const;
	int rowCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role) const;
	Q_INVOKABLE void remove(QVariant v);
	bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
	Q_INVOKABLE int count() const;
	ClipboardContainer* containerAt(int index);
	QList<ClipboardContainer*> items();
	ClipboardItem* currentItem();
	ClipboardItem* lastItem();
	ClipboardContainer* currentContainer();
	bool isEnabled() const;
	void setEnabled(bool enabled);
	bool isSaving() const;
	void setSave(bool save);
	int stackSize() const;
	void setStackSize(int size);
	ClipboardItem* add(ClipboardItem *item, bool allowDuplicity);
	bool isNew(ClipboardItem *item) const;
#ifdef INCLUDE_SERIAL_MODE
	void addBatch(ClipboardSerialBatch *batch);
	void beginSerialMode(qint64 id = 0);
	void endSerialMode();
	qint64 preparedSerialbatchId() const;
	ClipboardSerialBatch* searchBatchById(qint64 id);
#endif
	
signals:
	void historyChanged();
	
public slots:
	void load();
	void save();
	void clear();
	void deleteFile();
	void jumpTo(ClipboardItem* item);
#ifdef INCLUDE_SERIAL_MODE
	void restartSerialBatch(ClipboardSerialBatch *batch);
#endif

private:
	ClipboardContainer *m_currentContainer;
	QList<ClipboardContainer*> m_items;
	bool m_track;
	bool m_save;
	int m_size;
	bool m_serialInit;
	qint64 m_serialBatchId;

	QString filePath();
	void popToFront(ClipboardContainer *item);
	
};

#endif // HISTORY_H
