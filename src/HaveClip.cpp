#include "HaveClip.h"

#include <QApplication>
#include <QMimeData>
#include <QStringList>
#include <QUrl>
#include <QImage>
#include <QColor>
#include <QMenu>

#include "Client.h"
#include "Distributor.h"
#include "AboutDialog.h"

HaveClip::HaveClip(QObject *parent) :
	QTcpServer(parent)
{
	clipboard = QApplication::clipboard();
	signalMapper = new QSignalMapper(this);

	connect(signalMapper, SIGNAL(mapped(QObject*)), this, SLOT(historyActionClicked(QObject*)));

#if defined Q_OS_LINUX
	connect(clipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(clipboardChanged(QClipboard::Mode)));
#elif defined Q_OS_WIN32
	// Signal change(QClipboard::Mode) is not sent on Windows
	connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));
#endif

	// Load settings
	settings = new QSettings(this);

	foreach(QString node, settings->value("Pool/Nodes").toStringList())
	{
		Node *n = new Node;
		n->addr = node.section(':', 0, 0);
		n->port = node.section(':', 1, 1).toUShort();
		pool << n;
	}

	// Start server
	listen(QHostAddress::Any, 9999);

	// Tray
	trayIcon = new QSystemTrayIcon(QIcon(":/gfx/trayicon.png"), this);
	trayIcon->setToolTip(tr("HaveClip"));

	menu = new QMenu;
	menuSeparator = menu->addSeparator();
	menu->addAction(tr("&Settings"));
	menu->addAction(tr("&About..."), this, SLOT(showAbout()));
	menu->addAction(tr("&Quit"), qApp, SLOT(quit()));

	trayIcon->setContextMenu(menu);
	trayIcon->show();

	qApp->setQuitOnLastWindowClosed(false);
}

HaveClip::~HaveClip()
{
	qDeleteAll(pool);
	qDeleteAll(history);
	delete menu;
}

/**
  Called when system clipboard is changed
  */
void HaveClip::clipboardChanged()
{
	HaveClip::MimeType type;
	const QMimeData *mimeData = clipboard->mimeData(QClipboard::Clipboard);
	QVariant data;

	if(mimeData->hasText()) {
		type = HaveClip::Text;
		data = mimeData->text();

	} else if(mimeData->hasHtml()) {
		type = HaveClip::Html;
		data = mimeData->html();

	} else if(mimeData->hasUrls()) {
		type = HaveClip::Urls;

		// Convert to string list
		QStringList urls;

		foreach(QUrl u, mimeData->urls())
			urls << u.toString();

		data = urls;

	} else if(mimeData->hasImage()) {
		type = HaveClip::ImageData;
		data = mimeData->imageData();

	} else {
		qDebug() << "Uknown MIME type, ignoring";
		return;
	}

	if(lastClipboard == data)
	{
		qDebug() << "Clipboard is the same, no need to propagate";
		return;
	}

	HistoryItem *item = new HistoryItem;
	item->type = type;
	item->data = data;

	addToHistory(item);

	updateHistoryContextMenu();

	lastClipboard = data;

	foreach(Node *n, pool)
	{
		Distributor *d = new Distributor(n, this);
		d->distribute(type, data);
	}
}

void HaveClip::clipboardChanged(QClipboard::Mode m)
{
//	qDebug() << "Clipboard changed";

	if(m != QClipboard::Clipboard)
	{
//		qDebug() << "Not a clipboard, ignoring";
		return;
	}

	clipboardChanged();
}


void HaveClip::incomingConnection(int handle)
{
	Client *c = new Client(this);
	c->setSocketDescriptor(handle);

	connect(c, SIGNAL(clipboardUpdated(HaveClip::MimeType,QVariant)), this, SLOT(updateClipboard(HaveClip::MimeType,QVariant)));

	c->communicate();
}

/**
  Called when we receive new clipboard by network
  */
void HaveClip::updateClipboard(HaveClip::MimeType t, QVariant data)
{
	qDebug() << "Update clipboard" << t << data;

	lastClipboard = data;

	switch(t)
	{
	case HaveClip::Text:
	case HaveClip::Html:
		qDebug() << "Set clipboard" << data.toString();
		clipboard->setText(data.toString(), QClipboard::Clipboard);
		break;

	case HaveClip::Urls:
		qDebug() << "Set clipboard urls";
		clipboard->setText(data.toStringList().join("\n"));
		break;

	case HaveClip::ImageData: {
		qDebug() << "Set clipboard image";
		clipboard->setImage( qvariant_cast<QImage>(data) );
		break;
	}
	default:break;
	}

	HistoryItem *item = new HistoryItem;
	item->type = t;
	item->data = data;

	addToHistory(item);

	updateHistoryContextMenu();
}

void HaveClip::addToHistory(HistoryItem *it)
{
	if(history.size() >= 10)
		delete history.takeFirst();

	history << it;
}

void HaveClip::updateHistoryContextMenu()
{
	QHashIterator<QAction*, HistoryItem*> i(historyHash);

	while(i.hasNext())
	{
		i.next();

		signalMapper->removeMappings(i.key());
		menu->removeAction(i.key());
		historyHash.remove(i.key());
		i.key()->deleteLater();
	}

	QAction *lastAction = 0;

	foreach(HistoryItem *it, history)
	{
		QString text = it->data.toString().trimmed().left(30);
		QAction *act = new QAction(text, this);

		connect(act, SIGNAL(triggered()), signalMapper, SLOT(map()));
		signalMapper->setMapping(act, act);

		menu->insertAction(lastAction ? lastAction : menuSeparator, act);

		historyHash.insert(act, it);

		lastAction = act;
	}
}

void HaveClip::historyActionClicked(QObject *obj)
{
	QAction *act = static_cast<QAction*>(obj);

	if(historyHash.contains(act))
	{
		HistoryItem *it = historyHash[act];

		updateClipboard(it->type, it->data);
	}
}

void HaveClip::showAbout()
{
	AboutDialog *dlg = new AboutDialog;
	dlg->exec();
	dlg->deleteLater();
}
