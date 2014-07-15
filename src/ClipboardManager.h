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

#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include <QtGlobal>

#include <QClipboard>
#include <QSettings>
#include <QList>
#include <QHash>

#include "Network/ConnectionManager.h"
#include "ClipboardItem.h"
#include "History.h"

class History;
class Node;

class ClipboardManager : public QObject
{
	Q_OBJECT
public:
	enum SynchronizeMode {
		Selection,
		Clipboard,
		Both
	};

	explicit ClipboardManager(QObject *parent = 0);
	~ClipboardManager();
	static ClipboardManager* instance();
	QSettings *settings();
	ConnectionManager* connectionManager();
	History* history();
	ClipboardItem *currentItem();
	bool isSyncEnabled();
	bool isSendingEnabled();
	bool isReceivingEnabled();
	void setSyncMode(SynchronizeMode m);
	void distributeCurrentClipboard();
	static qint32 supportedModes();
	static void gracefullyExit(int sig);
	inline bool shouldDistribute() const;
	inline bool shouldListen() const;

public slots:
	void start();
	void delayedStart(int msecs);
	void jumpTo(ClipboardItem *content);
	void jumpToItemAt(int index);
	void saveSettings();
	void toggleSharedClipboard(bool enabled);
	void toggleClipboardSending(bool enabled, bool masterChange = false);
	void toggleClipboardReceiving(bool enabled, bool masterChange = false);

private:
	static ClipboardManager *m_instance;
	static QClipboard *clipboard;
	QSettings *m_settings;
	ConnectionManager *m_conman;
	QList<Node*> pool;
	History* m_history;
	bool m_clipSync;
	bool m_clipSnd;
	bool m_clipRecv;
	SynchronizeMode m_syncMode;
	QTimer *selectionTimer;
	QTimer *delayedEnsureTimer;
	ClipboardItem *delayedEnsureItem;
	bool clipboardChangedCalled;
	bool uniteCalled;

#ifdef Q_WS_X11
	bool isUserSelecting();
#endif
	void uniteClipboards(ClipboardItem *content);
	void ensureClipboardContent(ClipboardItem *content, QClipboard::Mode mode);
	void distributeClipboard(ClipboardItem *content);
	void updateToolTip();
	QMimeData* copyMimeData(const QMimeData *mimeReference);

private slots:
	void clipboardChanged();
	void clipboardChanged(QClipboard::Mode m, bool fromSelection = false);
	void updateClipboard(ClipboardContainer *content, bool fromHistory = false);
	void updateClipboardFromNetwork(ClipboardContainer *cont);
	void delayedClipboardEnsure();
#ifdef Q_WS_X11
	void checkSelection();
#endif
	
};

Q_DECLARE_METATYPE(Node*)

#endif // CLIPBOARDMANAGER_H
