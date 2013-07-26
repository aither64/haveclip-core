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

#ifndef HAVECLIP_H
#define HAVECLIP_H

#define VERSION "0.8.0-dev"

#include <QTcpServer>
#include <QClipboard>
#include <QSettings>
#include <QList>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QHash>
#include <QSignalMapper>
#include <QIcon>
#include <QHostInfo>
#include <QSslError>

#include "PasteServices/BasePasteService.h"
#include "ClipboardContent.h"

class HaveClip : public QTcpServer
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

	explicit HaveClip(QObject *parent = 0);
	~HaveClip();
	
signals:
	
public slots:

private:
	QSettings *settings;
	QClipboard *clipboard;
	QList<Node*> pool;
	QSystemTrayIcon *trayIcon;
	QMenu *menu;
	QMenu *historyMenu;
	QAction *historySeparator;
	QAction *menuSeparator;
	QAction *clipSndAction;
	QAction *clipRecvAction;
	QList<ClipboardContent*> history;
	QHash<QAction*, ClipboardContent*> historyHash;
	ClipboardContent *currentItem;
	QSignalMapper *historySignalMapper;
	QSignalMapper *pasteSignalMapper;
	QSignalMapper *pasteAdvSignalMapper;
	bool clipSync;
	bool clipSnd;
	bool clipRecv;
	bool histEnabled;
	int histSize;
	SelectionMode selectionMode;
	SynchronizeMode syncMode;
	QString host;
	Encryption encryption;
	QString certificate;
	QString privateKey;
	QString password;
	QList<QAction*> pasteActions;
	QList<BasePasteService*> pasteServices;
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
	void updateHistoryContextMenu();
	void updateToolTip();
	void loadNodes();
	QMimeData* copyMimeData(const QMimeData *mimeReference);
	void startListening(QHostAddress addr = QHostAddress::Null);
	void loadPasteServices();
	void clearPasteServices();

private slots:
	void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
	void clipboardChanged();
	void clipboardChanged(QClipboard::Mode m);
	void incomingConnection(int handle);
	void updateClipboard(ClipboardContent *content, bool fromHistory = false);
	void historyActionClicked(QObject *obj);
	void toggleSharedClipboard(bool enabled);
	void toggleClipboardSending(bool enabled);
	void toggleClipboardReceiving(bool enabled);
	void showSettings();
	void showAbout();
	void listenOnHost(const QHostInfo &host);
	void determineCertificateTrust(HaveClip::Node *node, const QList<QSslError> errors);
	void determineCertificateTrust(BasePasteService *service, const QList<QSslError> errors);
	void sslFatalError(const QList<QSslError> errors);
	void simplePaste(QObject *obj);
	void advancedPaste(QObject *obj);
//	void setPasteService(bool enabled, BasePasteService::PasteService type);
	void receivePasteUrl(QUrl url);
	void pasteServiceRequiresAuthentication(BasePasteService *service, QString username, bool failed, QString msg);
	void pasteServiceError(QString error);
	void checkSelection();
};

#endif // HAVECLIP_H
