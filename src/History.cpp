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

#include "Version.h"
#include "Settings.h"

History::History(QObject *parent) :
	QAbstractListModel(parent),
	m_currentContainer(0)
{
	connect(Settings::get(), SIGNAL(saveHistoryChanged(bool)), this, SLOT(saveChange(bool)));
}

void History::init()
{
	if(Settings::get()->isHistoryEnabled())
		load();
	else
		deleteFile();
}

QHash<int, QByteArray> History::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[PlainTextRole] = "plaintext";
	roles[ClipboardItemPointerRole] = "pointer";

	return roles;
}

int History::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);

	return m_items.count();
}

QVariant History::data(const QModelIndex &index, int role) const
{
	if(!index.isValid())
		return QVariant();

	switch(role)
	{
	case Qt::DisplayRole:
	case PlainTextRole:
		return m_items[ m_items.count() - index.row() - 1 ]->item()->toPlainText();

	case ClipboardItemPointerRole:
		return QVariant::fromValue<ClipboardContainer*>(m_items[ m_items.count() - index.row() - 1 ]);

	default:
		break;
	}

	return QVariant();
}

void History::remove(QVariant v)
{
	int index = m_items.indexOf(v.value<ClipboardContainer*>());

	if(index == -1)
		return;

	removeRows(m_items.count() - index - 1, 1);
}

bool History::removeRows(int row, int count, const QModelIndex &parent)
{
	beginRemoveRows(parent, row, row+count-1);

	int cnt = m_items.count();

	for(int i = row, j = row; i < row+count; i++)
		delete m_items.takeAt(cnt-j-1)->item();

	endRemoveRows();

	if(!m_items.count())
		m_currentContainer = 0;

	return true;
}

int History::count() const
{
	return m_items.count();
}

ClipboardContainer* History::containerAt(int index)
{
	return m_items[index];
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
	return Settings::get()->isHistoryEnabled();
}

bool History::isSaving() const
{
	return Settings::get()->saveHistory();
}

int History::stackSize() const
{
	return Settings::get()->historySize();
}

ClipboardItem* History::add(ClipboardItem *item, bool allowDuplicity)
{
	Q_UNUSED(allowDuplicity);

	if( ! Settings::get()->isHistoryEnabled() )
	{
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

		case ClipboardItem::BasicItem:
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

			if(m_items.size() >= Settings::get()->historySize())
			{
				beginRemoveRows(QModelIndex(), m_items.size()-1, m_items.size()-1);
				delete m_items.takeFirst();
				endRemoveRows();
			}

			beginInsertRows(QModelIndex(), 0, 0);
			m_items << item;
			endInsertRows();

			break;
		}
	} else {
		beginInsertRows(QModelIndex(), 0, 0);
		m_items << item;
		endInsertRows();
	}

	m_currentContainer = item;

	emit historyChanged();

	return m_currentContainer->item();
}

void History::load()
{
	if( ! Settings::get()->isHistoryEnabled() )
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

	int index = 0;

	while(!ds.atEnd())
	{
		ClipboardContainer *cnt = ClipboardContainer::load(ds);

		if(cnt)
		{
			beginInsertRows(QModelIndex(), index, index);
			m_items << cnt;
			endInsertRows();

			index++;
		}
	}

	if(!m_items.isEmpty())
		m_currentContainer = m_items.last();

	file.close();
}

void History::save()
{
	if(!Settings::get()->saveHistory())
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
	int cnt = m_items.count();

	if(!cnt)
		return;

	beginRemoveRows(QModelIndex(), 0, cnt - 1);

	qDeleteAll(m_items);
	m_items.clear();
	m_currentContainer = 0;

	endRemoveRows();
}

void History::deleteFile()
{
	QFile::remove(filePath());
}

void History::jumpTo(ClipboardItem* item)
{
	popToFront(item);
}

void History::saveChange(bool save)
{
	if(!save)
		deleteFile();
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
	int i = m_items.indexOf(item);

	m_items.removeAt(i);
	m_items << item;

	emit dataChanged(index(0, 0), index(i, 0));
}
