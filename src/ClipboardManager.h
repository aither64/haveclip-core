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

#define VERSION "0.10.0-dev"
#define HISTORY_MAGIC_NUMBER 0x84D3C117
#define HISTORY_VERSION 2

#define PROTO_MAGIC_NUMBER 0x84D3C117
#define PROTO_VERSION 2

#include <QtGlobal>

#ifdef Q_WS_X11
#define INCLUDE_SERIAL_MODE 1
#endif

#include <QTcpServer>
#include <QClipboard>
#include <QSettings>
#include <QList>
#include <QHash>
#include <QHostInfo>
#include <QSslError>
#include <QAbstractEventDispatcher>

#include "PasteServices/BasePasteService.h"
#include "ClipboardItem.h"
#include "History.h"

class History;

#ifdef INCLUDE_SERIAL_MODE
class ClipboardSerialBatch;
#endif

class ClipboardManager : public QTcpServer
{
	Q_OBJECT
public:
	struct Node {
		QString host;
		quint16 port;
		QSslCertificate certificate;

		QString toString();
	};

	enum Encryption {
		None=0,
		Ssl,
		Tls
	};

	enum SelectionMode {
		Separate,
		United
	};

	enum SynchronizeMode {
		Selection,
		Clipboard,
		Both
	};

	explicit ClipboardManager(QObject *parent = 0);
	~ClipboardManager();
	void start();
	QSettings *settings();
	QList<BasePasteService*> pasteServices();
	History* history();
	ClipboardItem *currentItem();
	bool isSyncEnabled();
	bool isSendingEnabled();
	bool isReceivingEnabled();
#ifdef INCLUDE_SERIAL_MODE
	bool isSerialModeEnabled() const;
#endif
	void setNodes(QStringList nodes);
	void setSelectionMode(SelectionMode m);
	void setSyncMode(SynchronizeMode m);
	void setListenHost(QString host, quint16 port);
	void setEncryption(Encryption encryption);
	void setCertificate(QString cert);
	void setPrivateKey(QString key);
	void setPassword(QString pass);
	void setPasteServices(QList<BasePasteService*> services);
	void distributeCurrentClipboard();
	static void gracefullyExit(int sig);
#ifdef INCLUDE_SERIAL_MODE
	static bool eventFilter(void *message);
#endif
	inline bool shouldDistribute() const;
	inline bool shouldListen() const;

signals:
	void listenFailed(QString error);
	void untrustedCertificateError(ClipboardManager::Node *node, const QList<QSslError> errors);
	void sslFatalError(const QList<QSslError> errors);
#ifdef INCLUDE_SERIAL_MODE
	void serialModeChanged(bool enabled);
#endif

public slots:
	void jumpTo(ClipboardItem *content);
	void saveSettings();
	void toggleSharedClipboard(bool enabled);
	void toggleClipboardSending(bool enabled, bool masterChange = false);
	void toggleClipboardReceiving(bool enabled, bool masterChange = false);
#ifdef INCLUDE_SERIAL_MODE
	void toggleSerialMode();
	void toggleSerialModeFromNetwork(bool enable, qint64 id);
	void serialModeNewBatch(ClipboardSerialBatch *batch);
	void serialModeAppend(ClipboardItem *item);
	void serialModeNext();
	void serialModeRestart(ClipboardContainer *cont);
	void serialModeRestartFromNetwork(ClipboardSerialBatch *cont);
#endif

private:
	static ClipboardManager *m_instance;
	static QStringList serialExceptions;
	QSettings *m_settings;
	QClipboard *clipboard;
	QList<Node*> pool;
	History* m_history;
	bool m_clipSync;
	bool m_clipSnd;
	bool m_clipRecv;
	SelectionMode m_selectionMode;
	SynchronizeMode m_syncMode;
	QString m_host;
	quint16 m_port;
	Encryption m_encryption;
	QString m_certificate;
	QString m_privateKey;
	QString m_password;
	QList<BasePasteService*> m_pasteServices;
	QTimer *selectionTimer;
	QTimer *delayedEnsureTimer;
	ClipboardItem *delayedEnsureItem;
	bool clipboardChangedCalled;
	bool uniteCalled;
#ifdef INCLUDE_SERIAL_MODE
	static QAbstractEventDispatcher::EventFilter prevEventFilter;
	QTimer *serialTimer;
	bool m_serialMode;
#endif

#ifdef Q_WS_X11
	bool isUserSelecting();
#endif
	void uniteClipboards(ClipboardItem *content);
	void ensureClipboardContent(ClipboardItem *content, QClipboard::Mode mode);
	void distributeClipboard(ClipboardItem *content);
	void updateToolTip();
	void loadNodes();
	QMimeData* copyMimeData(const QMimeData *mimeReference);
	void startListening(QHostAddress addr = QHostAddress::Null);
	void loadPasteServices();
	void clearPasteServices();

private slots:
	void clipboardChanged();
	void clipboardChanged(QClipboard::Mode m, bool fromSelection = false);
	void incomingConnection(int handle);
	void updateClipboard(ClipboardContainer *content, bool fromHistory = false);
	void updateClipboardFromNetwork(ClipboardContainer *cont);
	void listenOnHost(const QHostInfo &m_host);
	void delayedClipboardEnsure();
#ifdef Q_WS_X11
	void checkSelection();
#endif
	void receivePasteUrl(QUrl url);
#ifdef INCLUDE_SERIAL_MODE
	void nextSerialClipboard(bool fromNetwork = false);
	void propagateSerialMode();
#endif
	
};

#endif // CLIPBOARDMANAGER_H
