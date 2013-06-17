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

#define VERSION "0.5.0"

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
	QSignalMapper *signalMapper;
	bool clipSync;
	bool clipSnd;
	bool clipRecv;
	bool histEnabled;
	int histSize;
	QString host;
	Encryption encryption;
	QString certificate;
	QString privateKey;
	QString password;
	BasePasteService *pasteService;
	QAction *pasteAction;
	QAction *pasteAdvancedAction;
	QAction *pasteSeparator;

	void addToHistory(ClipboardContent *content);
	void updateHistoryContextMenu();
	void updateToolTip();
	void loadNodes();
	QMimeData* copyMimeData(const QMimeData *mimeReference);
	void startListening(QHostAddress addr = QHostAddress::Null);
	void createPasteService(BasePasteService::PasteService type);
	void removePasteService();

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
	void sslFatalError(const QList<QSslError> errors);
	void simplePaste();
	void advancedPaste();
	void setPasteService(bool enabled, BasePasteService::PasteService type);
	void receivePasteUrl(QUrl url);
	void pasteServiceRequiresAuthentication(QString username, bool failed, QString msg);
	void pasteServiceError(QString error);
};

#endif // HAVECLIP_H
