#include "HaveClip.h"

#include <QApplication>
#include <QMimeData>
#include <QStringList>
#include <QUrl>
#include <QImage>
#include <QColor>
#include <QMenu>
#include <QLabel>
#include <QTimer>
#include <QTextDocument>

#include "Client.h"
#include "Distributor.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"

HaveClip::HaveClip(QObject *parent) :
	QTcpServer(parent),
	currentItem(0)
{
	clipboard = QApplication::clipboard();
	signalMapper = new QSignalMapper(this);

	connect(signalMapper, SIGNAL(mapped(QObject*)), this, SLOT(historyActionClicked(QObject*)));

#if defined Q_OS_LINUX
	connect(clipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(clipboardChanged(QClipboard::Mode)));
#elif defined Q_OS_WIN32
	// Signal change(QClipboard::Mode) is not sent on Windows
	connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));
#elif defined Q_OS_MAC
	// There's no notification about clipboard changes on OS X, active checking is needed
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(clipboardChanged()));
	timer->start(300);
#endif

	// Load settings
	settings = new QSettings(this);

	loadNodes();

	clipSync = settings->value("Sync/Enable", true).toBool();
	clipSnd = settings->value("Sync/Send", true).toBool();
	clipRecv = settings->value("Sync/Receive", true).toBool();

	histEnabled = settings->value("History/Enable", true).toBool();
	histSize = settings->value("History/Size", 10).toInt();

	// Start server
	listen(QHostAddress::Any, 9999);

	// Tray
	trayIcon = new QSystemTrayIcon(QIcon(":/gfx/icon.png"), this);
	trayIcon->setToolTip(tr("HaveClip"));

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

	historyMenu = new QMenu;
	menuSeparator = historyMenu->addSeparator();

	menu = new QMenu;

	QAction *a = menu->addAction(tr("&Enable clipboard synchronization"));
	a->setCheckable(true);
	a->setChecked(clipSync);
	connect(a, SIGNAL(toggled(bool)), this, SLOT(toggleSharedClipboard(bool)));

	clipSndAction = menu->addAction(tr("Enable clipboard se&nding"));
	clipSndAction->setCheckable(true);
	clipSndAction->setChecked(clipSnd);
	clipSndAction->setEnabled(clipSync);
	connect(clipSndAction, SIGNAL(toggled(bool)), this, SLOT(toggleClipboardSending(bool)));

	clipRecvAction = menu->addAction(tr("Enable clipboard &receiving"));
	clipRecvAction->setCheckable(true);
	clipRecvAction->setChecked(clipRecv);
	clipRecvAction->setEnabled(clipSync);
	connect(clipRecvAction, SIGNAL(toggled(bool)), this, SLOT(toggleClipboardReceiving(bool)));

	menu->addSeparator();
	menu->addAction(tr("&Settings"), this, SLOT(showSettings()));
	menu->addAction(tr("&About..."), this, SLOT(showAbout()));
	menu->addAction(tr("&Quit"), qApp, SLOT(quit()));

	trayIcon->setContextMenu(menu);
	trayIcon->show();

	qApp->setQuitOnLastWindowClosed(false);

	// Load contents of clipboard
	clipboardChanged();
}

HaveClip::~HaveClip()
{
	qDeleteAll(pool);
	qDeleteAll(history);
	delete menu;
	delete historyMenu;
}

void HaveClip::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if(reason != QSystemTrayIcon::Trigger)
		return;

	historyMenu->exec(trayIcon->geometry().bottomLeft());
}

/**
  Called when system clipboard is changed
  */
void HaveClip::clipboardChanged()
{
	QMimeData *mimeData = copyMimeData(clipboard->mimeData(QClipboard::Clipboard));

	ClipboardContent *cnt = new ClipboardContent(mimeData);

	if((currentItem && *currentItem == *cnt) || cnt->formats.isEmpty())
	{
		delete cnt;
		return;
	}

	cnt->init();

	addToHistory(cnt);
	updateToolTip();
	updateHistoryContextMenu();

	if(clipSnd)
	{
		foreach(Node *n, pool)
		{
			Distributor *d = new Distributor(n, this);
			d->distribute(cnt);
		}
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

	connect(c, SIGNAL(clipboardUpdated(ClipboardContent*)), this, SLOT(updateClipboard(ClipboardContent*)));

	c->communicate();
}

/**
  Called when new clipboard is received via network
  */
void HaveClip::updateClipboard(ClipboardContent *content, bool fromHistory)
{
	qDebug() << "Update clipboard";

	currentItem = content;
	clipboard->setMimeData(copyMimeData(content->mimeData), QClipboard::Clipboard);

	if(fromHistory)
	{
		updateToolTip();
	} else {
		addToHistory(content);
		updateToolTip();
		updateHistoryContextMenu();
	}
}



void HaveClip::addToHistory(ClipboardContent *content)
{
	if(!histEnabled)
	{
		if(currentItem)
			delete currentItem;

		currentItem = content;

		return;
	}

	foreach(ClipboardContent *c, history)
	{
		if(*c == *content)
			return;
	}

	if(history.size() >= histSize)
		delete history.takeFirst();

	history << content;
	currentItem = content;
}

void HaveClip::updateHistoryContextMenu()
{
	QHashIterator<QAction*, ClipboardContent*> i(historyHash);

	while(i.hasNext())
	{
		i.next();

		signalMapper->removeMappings(i.key());
		historyMenu->removeAction(i.key());
		historyHash.remove(i.key());
		i.key()->deleteLater();
	}

	if(!histEnabled)
		return;

	QAction *lastAction = 0;

	foreach(ClipboardContent *c, history)
	{
		QAction *act = new QAction(c->title, this);

		if(!c->icon.isNull())
			act->setIcon(c->icon);

		connect(act, SIGNAL(triggered()), signalMapper, SLOT(map()));
		signalMapper->setMapping(act, act);

		historyMenu->insertAction(lastAction ? lastAction : menuSeparator, act);

		historyHash.insert(act, c);

		lastAction = act;
	}
}

void HaveClip::updateToolTip()
{
#if defined Q_OS_LINUX
	QString tip = "<p>%1</p>";
	tip += "<pre>" + currentItem->excerpt + "</pre>";
#else
	QString tip = "%1";
#endif

	trayIcon->setToolTip(tip.arg(tr("HaveClip")));
}

void HaveClip::loadNodes()
{
	pool.clear();

	foreach(QString node, settings->value("Pool/Nodes").toStringList())
	{
		Node *n = new Node;
		n->addr = node.section(':', 0, 0);
		n->port = node.section(':', 1, 1).toUShort();
		pool << n;
	}
}

void HaveClip::historyActionClicked(QObject *obj)
{
	QAction *act = static_cast<QAction*>(obj);

	if(historyHash.contains(act))
	{
		ClipboardContent *c = historyHash[act];

		currentItem = c;
		updateClipboard(c, true);
	}
}

void HaveClip::toggleSharedClipboard(bool enabled)
{
	clipSync = enabled;

	toggleClipboardSending(enabled);
	toggleClipboardReceiving(enabled);

	clipSndAction->setEnabled(enabled);
	clipRecvAction->setEnabled(enabled);

	settings->setValue("Sync/Enable", clipSync);
}

void HaveClip::toggleClipboardSending(bool enabled)
{
	clipSnd = enabled;

	settings->setValue("Sync/Send", clipSnd);
}

void HaveClip::toggleClipboardReceiving(bool enabled)
{
	if(enabled && !clipRecv)
		listen(QHostAddress::Any, 9999);
	else if(!enabled && clipRecv)
		close();

	clipRecv = enabled;
	settings->setValue("Sync/Receive", clipRecv);
}

void HaveClip::showSettings()
{
	SettingsDialog *dlg = new SettingsDialog(settings);

	if(dlg->exec() == QDialog::Accepted)
	{
		settings->setValue("Pool/Nodes", dlg->nodes());

		loadNodes();

		histEnabled = dlg->historyEnabled();
		histSize = dlg->historySize();

		settings->setValue("History/Enable", histEnabled);
		settings->setValue("History/Size", histSize);

		if(!histEnabled)
			updateHistoryContextMenu();
	}

	dlg->deleteLater();
}

void HaveClip::showAbout()
{
	AboutDialog *dlg = new AboutDialog;
	dlg->exec();
	dlg->deleteLater();
}

QMimeData* HaveClip::copyMimeData(const QMimeData *mimeReference)
{
	QMimeData *mimeCopy = new QMimeData();

	foreach(QString format, mimeReference->formats())
	{
		// Retrieving data
		QByteArray data = mimeReference->data(format);

		// Checking for custom MIME types
		if(format.startsWith("application/x-qt"))
		{
			// Retrieving true format name
			int indexBegin = format.indexOf('"') + 1;
			int indexEnd = format.indexOf('"', indexBegin);
			format = format.mid(indexBegin, indexEnd - indexBegin);
		}

		mimeCopy->setData(format, data);
	}

	return mimeCopy;
}
