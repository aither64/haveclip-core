/*
  HaveClip

  Copyright (C) 2013-2016 Jakub Skokan <aither@havefun.cz>

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

#include "qmlclipboardmanager.h"
#include "../Version.pri.h"
#include "../git_version.h"

QmlClipboardManager::QmlClipboardManager(QObject *parent) :
    QObject(parent)
{
    m_manager = ClipboardManager::instance();
    m_history = m_manager->history();

	connect(m_manager, SIGNAL(initialized()), this, SLOT(managerInitialize()));
    connect(m_history, SIGNAL(historyChanged()), this, SLOT(historyChange()));
}

QString QmlClipboardManager::version()
{
	return VERSION;
}

QString QmlClipboardManager::commitSha1()
{
	return GIT_CURRENT_SHA1;
}

QString QmlClipboardManager::content()
{
	return m_content;
}

void QmlClipboardManager::checkClipboard()
{
	m_manager->clipboardChanged();
}

void QmlClipboardManager::doSync()
{
	m_manager->distributeCurrentClipboard();
}

void QmlClipboardManager::jumpToItemAt(int index)
{
	m_manager->jumpToItemAt(m_history->count() - index - 1);

	m_content = m_history->currentItem()->toPlainText();
	emit contentChanged(m_content);
}

void QmlClipboardManager::managerInitialize()
{
	ClipboardItem *it = m_history->currentItem();

	if (!it)
		return;

	m_content = it->toPlainText();
	emit contentChanged(m_content);
}

void QmlClipboardManager::historyChange()
{
    QList<ClipboardContainer*> items = m_history->items();

    if(!items.size())
        m_content = "<empty>";
    else
        m_content = items.last()->item()->toPlainText();

    emit contentChanged(m_content);
}
