#include "HaveClip.h"

#include <QApplication>
#include <QMimeData>
#include <QStringList>
#include <QUrl>
#include <QImage>
#include <QColor>
#include <QMenu>
#include <QLabel>
#include <QTemporaryFile>
#include <QDir>
#include <QTimer>

#include "Client.h"
#include "Distributor.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"

HaveClip::ItemPreview::~ItemPreview()
{
	QFile::remove(path);
}

HaveClip::HistoryItem::~HistoryItem()
{
	if(preview)
		delete preview;
}

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
	trayIcon = new QSystemTrayIcon(QIcon(":/gfx/trayicon.png"), this);
	trayIcon->setToolTip(tr("HaveClip"));

	menu = new QMenu;
	menuSeparator = menu->addSeparator();

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
	HaveClip::ItemPreview *preview = 0;

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

		QImage img = data.value<QImage>();

		preview = createItemPreview(img);
	} else {
		qDebug() << "Uknown MIME type, ignoring";
		return;
	}

	// Clipboards did not change
	if(lastClipboard == data)
		return;

	addToHistory(type, data, preview);
	updateToolTip();
	updateHistoryContextMenu();

	lastClipboard = data;

	if(clipSnd)
	{
		foreach(Node *n, pool)
		{
			Distributor *d = new Distributor(n, this);
			d->distribute(type, data);
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

	connect(c, SIGNAL(clipboardUpdated(HaveClip::MimeType,QVariant)), this, SLOT(updateClipboard(HaveClip::MimeType,QVariant)));

	c->communicate();
}

/**
  Called when new clipboard is received via network
  */
void HaveClip::updateClipboard(HaveClip::MimeType t, QVariant data, bool fromHistory)
{
	qDebug() << "Update clipboard" << t << data;

	if(!fromHistory)
		lastClipboard = data;

	HaveClip::ItemPreview *preview = 0;

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
		QImage img = data.value<QImage>();
		clipboard->setImage(img);

		preview = createItemPreview(img);
		break;
	}
	default:break;
	}

	if(fromHistory)
	{
		updateToolTip();
	} else {
		addToHistory(t, data, preview);
		updateToolTip();
		updateHistoryContextMenu();
	}
}

HaveClip::ItemPreview* HaveClip::createItemPreview(QImage &img)
{
	ItemPreview *preview = 0;
	QTemporaryFile tmp(QDir::tempPath() + "/haveclip-preview-XXXXXX");
	tmp.setAutoRemove(false);

	if(tmp.open())
	{
		if( img.save(&tmp, "PNG") )
		{
			preview = new ItemPreview;
			preview->path = tmp.fileName();
			preview->width = img.width();
			preview->height = img.height();
		}

		tmp.close();
	}

	return preview;
}

void HaveClip::addToHistory(HaveClip::MimeType type, QVariant data, ItemPreview *preview)
{
	if(!histEnabled)
	{
		if(currentItem == 0)
		{
			currentItem = new HistoryItem;
			history << currentItem;
		}

		currentItem->type = type;
		currentItem->data = data;
		currentItem->preview = preview;

		return;
	}

	foreach(HistoryItem *it, history)
	{
		if(it->data == data)
			return;
	}

	HistoryItem *item = new HistoryItem;
	item->type = type;
	item->data = data;
	item->preview = preview;

	if(history.size() >= histSize)
		delete history.takeFirst();

	history << item;
	currentItem = item;
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

	if(!histEnabled)
		return;

	QAction *lastAction = 0;

	foreach(HistoryItem *it, history)
	{
		QString text;

		switch(it->type)
		{
		case HaveClip::Text:
		case HaveClip::Html:
			text = it->data.toString().trimmed().left(30);
			break;
		case HaveClip::Urls:
			text = it->data.toStringList().first().left(30);
			break;
		case HaveClip::ImageData:
			text = tr("Image");

			if(it->icon.isNull())
				it->icon = QIcon( QPixmap::fromImage(it->data.value<QImage>()) );

			break;
		}

		QAction *act = new QAction(text, this);

		if(!it->icon.isNull())
			act->setIcon(it->icon);

		connect(act, SIGNAL(triggered()), signalMapper, SLOT(map()));
		signalMapper->setMapping(act, act);

		menu->insertAction(lastAction ? lastAction : menuSeparator, act);

		historyHash.insert(act, it);

		lastAction = act;
	}
}

void HaveClip::updateToolTip()
{
	QString tip = "<p>%1</p>";

	switch(currentItem->type)
	{
	case HaveClip::Text:
	case HaveClip::Html: {
		QString s = currentItem->data.toString();
		tip += "<pre>" + s.mid(0, 200) + "</pre>";

		if(s.size() > 200)
			tip += "<br>...";
		break;
	}
	case HaveClip::Urls: {
		QStringList l = currentItem->data.toStringList();
		tip += QStringList(l.mid(0, 10)).join("\n");

		if(l.size() > 10)
			tip += "<br>...";
		break;
	}
	case HaveClip::ImageData: {
		QString prop;

		if(currentItem->preview->width > 400)
			prop = QString("width=\"%1\"").arg(400);
		else if(currentItem->preview->height > 400)
			prop = QString("height=\"%1\"").arg(400);

		tip += QString("<p><img src=\"%1\" %2></p>").arg(currentItem->preview->path).arg(prop);
		break;
	}
	}

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
		HistoryItem *it = historyHash[act];

		currentItem = it;
		updateClipboard(it->type, it->data, true);
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
