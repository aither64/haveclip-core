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

#define VERSION "0.9.0-dev"
#define HISTORY_MAGIC_NUMBER 0x84D3C117
#define HISTORY_VERSION 1

#define PROTO_MAGIC_NUMBER 0x84D3C117
#define PROTO_VERSION 1

#include <QTcpServer>
#include <QClipboard>
#include <QSettings>
#include <QList>
#include <QHash>
#include <QHostInfo>
#include <QSslError>

#include "PasteServices/BasePasteService.h"
#include "ClipboardContent.h"

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
	QList<ClipboardContent*> history();
	ClipboardContent *currentItem();
	bool isHistoryEnabled();
	bool isSyncEnabled();
	bool isSendingEnabled();
	bool isReceivingEnabled();
	void setNodes(QStringList nodes);
	void setHistoryEnabled(bool enable);
	void setHistorySize(int size);
	void setHistorySave(bool save);
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

signals:
	void listenFailed(QString error);
	void historyChanged();
	void untrustedCertificateError(ClipboardManager::Node *node, const QList<QSslError> errors);
	void sslFatalError(const QList<QSslError> errors);

public slots:
	void jumpTo(ClipboardContent *content);
	void saveSettings();
	void toggleSharedClipboard(bool enabled);
	void toggleClipboardSending(bool enabled);
	void toggleClipboardReceiving(bool enabled);

private:
	QSettings *m_settings;
	QClipboard *clipboard;
	QList<Node*> pool;
	QList<ClipboardContent*> m_history;
	ClipboardContent *m_currentItem;
	bool m_clipSync;
	bool m_clipSnd;
	bool m_clipRecv;
	bool m_histEnabled;
	int m_histSize;
	bool m_histSave;
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
	bool clipboardChangedCalled;
	bool uniteCalled;

#ifdef Q_WS_X11
	bool isUserSelecting();
#endif
	void uniteClipboards(ClipboardContent *content);
	void ensureClipboardContent(ClipboardContent *content, QClipboard::Mode mode);
	void distributeClipboard(ClipboardContent *content, bool deleteLater = false);
	void addToHistory(ClipboardContent *content);
	void popToFront(ClipboardContent *content);
	QString historyFilePath();
	void deleteHistoryFile();
	void updateToolTip();
	void loadNodes();
	QMimeData* copyMimeData(const QMimeData *mimeReference);
	void startListening(QHostAddress addr = QHostAddress::Null);
	void loadPasteServices();
	void clearPasteServices();

private slots:
	void clipboardChanged();
	void clipboardChanged(QClipboard::Mode m);
	void incomingConnection(int handle);
	void updateClipboard(ClipboardContent *content, bool fromHistory = false);
	void listenOnHost(const QHostInfo &m_host);
#ifdef Q_WS_X11
	void checkSelection();
#endif
	void loadHistory();
	void saveHistory();
	void receivePasteUrl(QUrl url);
	
};

#endif // CLIPBOARDMANAGER_H
