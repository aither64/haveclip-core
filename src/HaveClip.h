#ifndef HAVECLIP_H
#define HAVECLIP_H

#include <QTcpServer>
#include <QClipboard>
#include <QSettings>
#include <QList>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QHash>
#include <QSignalMapper>

class HaveClip : public QTcpServer
{
	Q_OBJECT
public:
	struct Node {
		QHostAddress addr;
		quint16 port;
	};

	enum MimeType {
		Text=0,
		Html,
		Urls,
		ImageData,
		ColorData,
		Unknown
	};

	struct HistoryItem {
		MimeType type;
		QVariant data;
	};

	explicit HaveClip(QObject *parent = 0);
	~HaveClip();
	
signals:
	
public slots:

private:
	QSettings *settings;
	QClipboard *clipboard;
	QList<Node*> pool;
	QVariant lastClipboard;
	QSystemTrayIcon *trayIcon;
	QMenu *menu;
	QAction *menuSeparator;
	QList<HistoryItem*> history;
	QHash<QAction*, HistoryItem*> historyHash;
	QSignalMapper *signalMapper;
	bool synchronize;

	void addToHistory(MimeType type, QVariant data);
	void updateHistoryContextMenu();

private slots:
	void clipboardChanged();
	void clipboardChanged(QClipboard::Mode m);
	void incomingConnection(int handle);
	void updateClipboard(HaveClip::MimeType t, QVariant data, bool fromHistory = false);
	void historyActionClicked(QObject *obj);
	void toggleSharedClipboard(bool enabled);
	void showSettings();
	void showAbout();
};

#endif // HAVECLIP_H
