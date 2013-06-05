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
#include <QIcon>

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

	struct ItemPreview {
		QString path;
		int width;
		int height;

		~ItemPreview();
	};

	struct HistoryItem {
		MimeType type;
		QVariant data;
		QIcon icon;
		ItemPreview *preview;

		~HistoryItem();
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
	QAction *clipSndAction;
	QAction *clipRecvAction;
	QList<HistoryItem*> history;
	QHash<QAction*, HistoryItem*> historyHash;
	HistoryItem *currentItem;
	QSignalMapper *signalMapper;
	bool clipSync;
	bool clipSnd;
	bool clipRecv;
	bool histEnabled;
	int histSize;

	ItemPreview* createItemPreview(QImage &img);
	void addToHistory(MimeType type, QVariant data, ItemPreview *preview);
	void updateHistoryContextMenu();
	void updateToolTip();
	void loadNodes();

private slots:
	void clipboardChanged();
	void clipboardChanged(QClipboard::Mode m);
	void incomingConnection(int handle);
	void updateClipboard(HaveClip::MimeType t, QVariant data, bool fromHistory = false);
	void historyActionClicked(QObject *obj);
	void toggleSharedClipboard(bool enabled);
	void toggleClipboardSending(bool enabled);
	void toggleClipboardReceiving(bool enabled);
	void showSettings();
	void showAbout();
};

#endif // HAVECLIP_H
