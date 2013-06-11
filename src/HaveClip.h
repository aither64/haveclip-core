#ifndef HAVECLIP_H
#define HAVECLIP_H

#define VERSION "0.2.0"

#include <QTcpServer>
#include <QClipboard>
#include <QSettings>
#include <QList>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QHash>
#include <QSignalMapper>
#include <QIcon>

#include "ClipboardContent.h"

class HaveClip : public QTcpServer
{
	Q_OBJECT
public:
	struct Node {
		QHostAddress addr;
		quint16 port;
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

	void addToHistory(ClipboardContent *content);
	void updateHistoryContextMenu();
	void updateToolTip();
	void loadNodes();
	QMimeData* copyMimeData(const QMimeData *mimeReference);

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
};

#endif // HAVECLIP_H
