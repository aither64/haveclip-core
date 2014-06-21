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

#define VERSION "0.13.0"
#define HISTORY_MAGIC_NUMBER 0x84D3C117
#define HISTORY_VERSION 3

#define PROTO_MAGIC_NUMBER 0x84D3C117
#define PROTO_VERSION 3

#include <QtGlobal>

#include <QTcpServer>
#include <QClipboard>
#include <QSettings>
#include <QList>
#include <QHash>
#include <QHostInfo>
#include <QSslError>
#include <QAbstractEventDispatcher>

#include "ClipboardItem.h"
#include "History.h"

class History;

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
	static ClipboardManager* instance();
	QSettings *settings();
	History* history();
	ClipboardItem *currentItem();
	bool isSyncEnabled();
	bool isSendingEnabled();
	bool isReceivingEnabled();
	QString host();
	quint16 port();
	QString password();
	QList<Node*> nodes();
	void setNodes(QStringList nodes);
	void setNodes(QList<Node*> nodes);
	void setSelectionMode(SelectionMode m);
	void setSyncMode(SynchronizeMode m);
	void setListenHost(QString host, quint16 port);
	void setHost(QString host);
	void setPort(quint16 port);
	void setEncryption(Encryption encryption);
	void setCertificate(QString cert);
	void setPrivateKey(QString key);
	void setPassword(QString pass);
	void distributeCurrentClipboard();
	static qint32 supportedModes();
	static void gracefullyExit(int sig);
	inline bool shouldDistribute() const;
	inline bool shouldListen() const;

signals:
	void listenFailed(QString error);
	void untrustedCertificateError(ClipboardManager::Node *node, const QList<QSslError> errors);
	void sslFatalError(const QList<QSslError> errors);

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
	void loadNodes();
	QMimeData* copyMimeData(const QMimeData *mimeReference);
	void startListening(QHostAddress addr = QHostAddress::Null);

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
	
};

Q_DECLARE_METATYPE(ClipboardManager::Node*)

#endif // CLIPBOARDMANAGER_H
