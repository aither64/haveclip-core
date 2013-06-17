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
#include <QMessageBox>
#include <QFileInfo>

#include "Receiver.h"
#include "Sender.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"
#include "CertificateTrustDialog.h"
#include "LoginDialog.h"

#include "PasteServices/PasteDialog.h"
#include "PasteServices/Stikked/Stikked.h"
#include "PasteServices/Pastebin/Pastebin.h"

QString HaveClip::Node::toString()
{
	return host + ":" + QString::number(port);
}

HaveClip::HaveClip(QObject *parent) :
	QTcpServer(parent),
	currentItem(0),
	pasteService(0)
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

	encryption = (HaveClip::Encryption) settings->value("Connection/Encryption", HaveClip::None).toInt();
	certificate = settings->value("Connection/Certificate", "certs/haveclip.crt").toString();
	privateKey = settings->value("Connection/PrivateKey", "certs/haveclip.key").toString();

	password = settings->value("AccessPolicy/Password").toString();

	// Start server
	startListening();

	// Tray
	trayIcon = new QSystemTrayIcon(QIcon(":/gfx/icon.png"), this);
	trayIcon->setToolTip(tr("HaveClip"));

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

	historyMenu = new QMenu;
	historySeparator = historyMenu->addSeparator();

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
	menuSeparator = menu->addSeparator();

	setPasteService(
		settings->value("PasteServices/Enable", false).toBool(),
		(BasePasteService::PasteService) settings->value("PasteServices/Service", BasePasteService::Stikked).toInt()
	);

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

	if(currentItem && *currentItem == *cnt)
	{
		delete cnt;
		return;

	} else if(currentItem && cnt->formats.isEmpty()) { // empty clipboard, restore last content
		updateClipboard(currentItem, true);
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
			Sender *d = new Sender(encryption, n, this);

			connect(d, SIGNAL(untrustedCertificateError(HaveClip::Node*,QList<QSslError>)), this, SLOT(determineCertificateTrust(HaveClip::Node*,QList<QSslError>)));
			connect(d, SIGNAL(sslFatalError(QList<QSslError>)), this, SLOT(sslFatalError(QList<QSslError>)));

			d->distribute(cnt, password);
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
	Receiver *c = new Receiver(encryption, this);
	c->setSocketDescriptor(handle);

	connect(c, SIGNAL(clipboardUpdated(ClipboardContent*)), this, SLOT(updateClipboard(ClipboardContent*)));

	c->setCertificateAndKey(certificate, privateKey);
	c->setAcceptPassword(password);
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
		{
			currentItem = content;
			return;
		}
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

		historyMenu->insertAction(lastAction ? lastAction : historySeparator, act);

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
		n->host = node.section(':', 0, 0);
		n->port = node.section(':', 1, 1).toUShort();

		QByteArray cert = settings->value("Node:" + n->toString() + "/Certificate").toString().toAscii();

		if(!cert.isEmpty())
			n->certificate = QSslCertificate::fromData(cert).first();

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
		startListening();
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

		QString oldHost = host;
		host = dlg->host();
		int port = dlg->port();

		settings->setValue("Connection/Host", host);
		settings->setValue("Connection/Port", port);

		password = dlg->password();
		settings->setValue("AccessPolicy/Password", password);

		if(host != oldHost || port != serverPort())
		{
			if(isListening())
				close();

			startListening();
		}

		encryption = dlg->encryption();
		certificate = dlg->certificate();
		privateKey = dlg->privateKey();

		settings->setValue("Connection/Encryption", encryption);
		settings->setValue("Connection/Certificate", certificate);
		settings->setValue("Connection/PrivateKey", privateKey);

		// Paste services
		settings->setValue("PasteServices/Enable", dlg->pasteServiceEnabled());
		settings->setValue("PasteServices/Service", dlg->pasteServiceType());

		setPasteService(dlg->pasteServiceEnabled(), dlg->pasteServiceType());

		if(pasteService)
			pasteService->applySettings(dlg->pasteServiceSettings());
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

void HaveClip::startListening(QHostAddress addr)
{
	QString host = settings->value("Connection/Host", "0.0.0.0").toString();
	this->host.clear();

	if(addr.isNull())
		addr.setAddress(host);

	if(!addr.isNull())
	{
		if(!listen(addr, settings->value("Connection/Port", 9999).toInt()))
		{
			QMessageBox::warning(0, tr("Unable to start listening"), tr("Listening failed, clipboard receiving is not active!\n\n") + errorString());
			return;
		}

		this->host = host;

		qDebug() << "Listening on" << serverAddress() << serverPort();

	} else {
		QHostInfo::lookupHost(host, this, SLOT(listenOnHost(QHostInfo)));
	}
}

void HaveClip::listenOnHost(const QHostInfo &host)
{
	if(host.error() != QHostInfo::NoError) {
		QMessageBox::warning(0, tr("Unable to resolve hostname"), tr("Resolving failed: ") + host.errorString());
		return;
	}

	QList<QHostAddress> addrs = host.addresses();

	if(addrs.size() == 0)
	{
		QMessageBox::warning(0, tr("Hostname has no IP address"), tr("Hostname has no IP addresses. Clipboard receiving is not active."));
		return;
	}

	startListening(addrs.first());
}

void HaveClip::determineCertificateTrust(HaveClip::Node *node, const QList<QSslError> errors)
{
	CertificateTrustDialog *dlg = new CertificateTrustDialog(node, errors);

	if(dlg->exec() == QDialog::Accepted)
	{
		QSslCertificate cert = errors.first().certificate();
		node->certificate = cert;

		if(dlg->remember())
			settings->setValue("Node:" + node->toString() + "/Certificate", QString(cert.toPem()));

		// It is easier to just create new instance. We would have to wait for the current one to fail.
		Sender *d = new Sender(encryption, node, this);

		connect(d, SIGNAL(untrustedCertificateError(HaveClip::Node*,QList<QSslError>)), this, SLOT(determineCertificateTrust(HaveClip::Node*,QList<QSslError>)));
		connect(d, SIGNAL(sslFatalError(QList<QSslError>)), this, SLOT(sslFatalError(QList<QSslError>)));

		d->distribute(currentItem, password);
	}

	dlg->deleteLater();
}

void HaveClip::sslFatalError(const QList<QSslError> errors)
{
	QString errs;

	foreach(QSslError e, errors)
		errs += e.errorString() + "\n";

	QMessageBox::warning(0, tr("SSL fatal error"), tr("Unable to establish secure connection:\n\n") + errs);
}

void HaveClip::setPasteService(bool enabled, BasePasteService::PasteService type)
{
	if(enabled && !pasteService)
	{
		createPasteService( (BasePasteService::PasteService) settings->value("PasteServices/Service", BasePasteService::Stikked).toInt() );

	} else if(!enabled && pasteService) {
		removePasteService();

	} else if(enabled && pasteService && pasteService->type() != type) {
		qDebug() << "Replacing paste service";
		removePasteService();
		createPasteService(type);
	}
}

void HaveClip::createPasteService(BasePasteService::PasteService type)
{
	switch(type)
	{
	case BasePasteService::Stikked:
		pasteService = new Stikked(settings, this);
		break;
	case BasePasteService::Pastebin:
		pasteService = new Pastebin(settings, this);
		break;
	default:
		return;
	}

	connect(pasteService, SIGNAL(authenticationRequired(QString,bool,QString)), this, SLOT(pasteServiceRequiresAuthentication(QString,bool,QString)));
	connect(pasteService, SIGNAL(pasted(QUrl)), this, SLOT(receivePasteUrl(QUrl)));

	pasteAction = new QAction(tr("Paste to %1").arg(pasteService->label()), this);
	connect(pasteAction, SIGNAL(triggered()), this, SLOT(simplePaste()));

	pasteAdvancedAction = new QAction(tr("Advanced paste to %1").arg(pasteService->label()), this);
	connect(pasteAdvancedAction, SIGNAL(triggered()), this, SLOT(advancedPaste()));

	menu->insertAction(menuSeparator, pasteAction);
	menu->insertAction(menuSeparator, pasteAdvancedAction);
}

void HaveClip::removePasteService()
{
	menu->removeAction(pasteAction);
	menu->removeAction(pasteAdvancedAction);

	pasteAction->deleteLater();
	pasteAdvancedAction->deleteLater();

	pasteService->deleteLater();
	pasteService = 0;
}

void HaveClip::simplePaste()
{
	switch(pasteService->type())
	{
	case BasePasteService::Stikked:

		break;
	}

	pasteService->paste(currentItem->toPlainText());
}

void HaveClip::advancedPaste()
{
	PasteDialog *dlg = new PasteDialog(currentItem->mimeData->text(), pasteService);

	if(dlg->exec() == QDialog::Accepted)
	{
		pasteService->paste(dlg->pasteServiceSettings(), currentItem->toPlainText());
	}

	dlg->deleteLater();
}

void HaveClip::receivePasteUrl(QUrl url)
{
	QMimeData *mime = new QMimeData;

	QList<QUrl> urls;
	urls << url;

	QString html = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
			"<html><head>"
			"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"</head><body>"
			"<a href=\"%1\">%1</a>"
			"</body></html>";

	mime->setUrls(urls);
	mime->setText(url.toString());
	mime->setHtml(html.arg(url.toString()));

	clipboard->setMimeData(mime);
}

void HaveClip::pasteServiceRequiresAuthentication(QString username, bool failed, QString msg)
{
	LoginDialog *dlg = new LoginDialog(username);

	if(failed)
		dlg->setError(tr("Login failed: %1").arg(msg));

	if(dlg->exec() == QDialog::Accepted)
	{
		pasteService->provideAuthentication(dlg->username(), dlg->password());
	}

	dlg->deleteLater();
}
