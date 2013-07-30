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
#include <QDesktopServices>
#include <QDir>

#include "Receiver.h"
#include "Sender.h"
#include "SettingsDialog.h"
#include "AboutDialog.h"
#include "CertificateTrustDialog.h"
#include "LoginDialog.h"

#include "PasteServices/PasteDialog.h"
#include "PasteServices/Stikked/Stikked.h"
#include "PasteServices/Pastebin/Pastebin.h"

#ifdef Q_WS_X11
#include <QX11Info>
extern "C" {
	#include <X11/Xlib.h>
}
#endif

QString HaveClip::Node::toString()
{
	return host + ":" + QString::number(port);
}

HaveClip::HaveClip(QObject *parent) :
	QTcpServer(parent),
	currentItem(0),
	clipboardChangedCalled(false),
	uniteCalled(false)
{
	clipboard = QApplication::clipboard();
	historySignalMapper = new QSignalMapper(this);
	pasteSignalMapper = new QSignalMapper(this);
	pasteAdvSignalMapper = new QSignalMapper(this);

	connect(historySignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(historyActionClicked(QObject*)));
	connect(pasteSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(simplePaste(QObject*)));
	connect(pasteAdvSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(advancedPaste(QObject*)));

	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(saveHistory()));

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

#ifdef Q_WS_X11
	selectionTimer = new QTimer(this);
	selectionTimer->setSingleShot(true);

	connect(selectionTimer, SIGNAL(timeout()), this, SLOT(checkSelection()));
#endif

	// Load settings
	settings = new QSettings(this);

	loadNodes();

	clipSync = settings->value("Sync/Enable", true).toBool();
	clipSnd = settings->value("Sync/Send", true).toBool();
	clipRecv = settings->value("Sync/Receive", true).toBool();

	histEnabled = settings->value("History/Enable", true).toBool();
	histSize = settings->value("History/Size", 10).toInt();
	histSave = settings->value("History/Save", true).toBool();

	selectionMode = (HaveClip::SelectionMode) settings->value("Selection/Mode", HaveClip::Separate).toInt();
	syncMode = (HaveClip::SynchronizeMode) settings->value("Sync/Synchronize", HaveClip::Both).toInt();

	encryption = (HaveClip::Encryption) settings->value("Connection/Encryption", 0).toInt();
	certificate = settings->value("Connection/Certificate", "certs/haveclip.crt").toString();
	privateKey = settings->value("Connection/PrivateKey", "certs/haveclip.key").toString();

	password = settings->value("AccessPolicy/Password").toString();

	// Start server
	startListening();

	// Tray
	trayIcon = new QSystemTrayIcon(QIcon(":/gfx/HaveClip_128.png"), this);
	trayIcon->setToolTip(tr("HaveClip"));

#ifndef Q_OS_MAC
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
#endif

	historyMenu = new QMenu(tr("History"));
	historySeparator = historyMenu->addSeparator();

	menu = new QMenu;

#if defined Q_OS_MAC
	menu->addMenu(historyMenu);
	menu->addSeparator();
#endif

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

	loadPasteServices();

	menu->addAction(tr("&Settings"), this, SLOT(showSettings()));
	menu->addAction(tr("&About..."), this, SLOT(showAbout()));
	menu->addAction(tr("&Quit"), qApp, SLOT(quit()));

	trayIcon->setContextMenu(menu);
	trayIcon->show();

	qApp->setQuitOnLastWindowClosed(false);
	qApp->setWindowIcon(QIcon(":/gfx/HaveClip_128.png"));

	// Load history
	if(histSave)
		loadHistory();
	else
		deleteHistoryFile();

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
	clipboardChanged(QClipboard::Clipboard);
}

void HaveClip::clipboardChanged(QClipboard::Mode m)
{
	if(clipboardChangedCalled)
	{
		qDebug() << "ClipboardChanged already called, end";
		return;
	}

	clipboardChangedCalled = true;

	if((m != QClipboard::Clipboard && m != QClipboard::Selection)
			|| (syncMode != HaveClip::Both
			    && ((m == QClipboard::Selection && syncMode == HaveClip::Clipboard) || (m == QClipboard::Clipboard && syncMode == HaveClip::Selection)))
	)
	{
		qDebug() << "Ignoring this clipboard";
		clipboardChangedCalled = false;
		return;
	}

#ifdef Q_WS_X11
	if(m == QClipboard::Selection && isUserSelecting())
	{
		clipboardChangedCalled = false;
		return;
	}
#endif

	ClipboardContent::Mode mode = ClipboardContent::qtModeToOwn(m);
	const QMimeData *mimeData = clipboard->mimeData(m);
	QMimeData *copiedMimeData;

	if(m == QClipboard::Selection) // Selection has only text and html
	{
		copiedMimeData = new QMimeData();

		if(mimeData->hasText())
			copiedMimeData->setText(mimeData->text());

		if(mimeData->hasHtml())
			copiedMimeData->setHtml(mimeData->html());
	} else
		copiedMimeData = copyMimeData(mimeData);

	ClipboardContent *cnt = new ClipboardContent(mode, copiedMimeData);

	if(currentItem && *currentItem == *cnt)
	{
		if(currentItem->mode != ClipboardContent::ClipboardAndSelection && currentItem->mode != cnt->mode)
		{
			currentItem->mode = ClipboardContent::ClipboardAndSelection;
			distributeClipboard(currentItem);
		}

		delete cnt;
		clipboardChangedCalled = false;
		return;

	} else if(currentItem && cnt->formats.isEmpty()) { // empty clipboard, restore last content
		qDebug() << "Clipboard is empty, reset";
		updateClipboard(currentItem, true);
		delete cnt;
		clipboardChangedCalled = false;
		return;
	}

	cnt->init();

	addToHistory(cnt);
	updateToolTip();
	updateHistoryContextMenu();

	if(selectionMode == HaveClip::United)
		uniteClipboards(cnt);

	if(clipSnd)
		distributeClipboard(cnt);

	clipboardChangedCalled = false;
}

void HaveClip::distributeClipboard(ClipboardContent *content, bool deleteLater)
{
	foreach(Node *n, pool)
	{
		Sender *d = new Sender(encryption, n, this);
		d->setDeleteContentOnSent(deleteLater);

		connect(d, SIGNAL(untrustedCertificateError(HaveClip::Node*,QList<QSslError>)), this, SLOT(determineCertificateTrust(HaveClip::Node*,QList<QSslError>)));
		connect(d, SIGNAL(sslFatalError(QList<QSslError>)), this, SLOT(sslFatalError(QList<QSslError>)));

		d->distribute(content, password);
	}
}

#ifdef Q_WS_X11
bool HaveClip::isUserSelecting()
{
	Window root, child;
	int root_x, root_y, win_x, win_y;
	unsigned int state;

	XQueryPointer(QX11Info::display(), QX11Info::appRootWindow(), &root, &child, &root_x, &root_y, &win_x, &win_y, &state);

	if((state & Button1Mask) == Button1Mask || (state & ShiftMask) == ShiftMask)
	{
		if(!selectionTimer->isActive())
			selectionTimer->start(100);

		return true;
	}

	return false;
}
#endif

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

	if(selectionMode == HaveClip::United || content->mode == ClipboardContent::ClipboardAndSelection)
	{
		uniteClipboards(content);
	} else
		clipboard->setMimeData(copyMimeData(content->mimeData), ClipboardContent::ownModeToQt(content->mode));

	if(fromHistory)
	{
		updateToolTip();
	} else {
		addToHistory(content);
		updateToolTip();
		updateHistoryContextMenu();
	}

	qDebug() << "Update clipboard end";
}

void HaveClip::uniteClipboards(ClipboardContent *content)
{
	if(uniteCalled)
	{
		qDebug() << "Unite has already been called, end";
		return;
	}

	uniteCalled = true;

	ensureClipboardContent(content, QClipboard::Selection);
	ensureClipboardContent(content, QClipboard::Clipboard);

	uniteCalled = false;
}

void HaveClip::ensureClipboardContent(ClipboardContent *content, QClipboard::Mode mode)
{
	if(!ClipboardContent::compareMimeData(content->mimeData, clipboard->mimeData(mode), mode == QClipboard::Selection))
	{
		qDebug() << "Update" << mode;
		clipboard->setMimeData(copyMimeData(content->mimeData), mode);
	} else {
		qDebug() << "No need to update" << mode;
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

		historySignalMapper->removeMappings(i.key());
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

		connect(act, SIGNAL(triggered()), historySignalMapper, SLOT(map()));
		historySignalMapper->setMapping(act, act);

		historyMenu->insertAction(lastAction ? lastAction : historySeparator, act);

		historyHash.insert(act, c);

		lastAction = act;
	}
}

QString HaveClip::historyFilePath()
{
	return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/history.dat";
}

void HaveClip::deleteHistoryFile()
{
	QFile::remove(historyFilePath());
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
		histSave = dlg->saveHistory();

		settings->setValue("History/Enable", histEnabled);
		settings->setValue("History/Size", histSize);
		settings->setValue("History/Save", histSave);

		if(!histSave)
			deleteHistoryFile();

		selectionMode = dlg->selectionMode();
		syncMode = dlg->synchronizationMode();

		settings->setValue("Selection/Mode", selectionMode);
		settings->setValue("Sync/Synchronize", syncMode);

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
		settings->beginGroup("PasteServices");
		settings->remove("");

		int i = 0;

		foreach(BasePasteService *s, dlg->pasteServices())
		{
			settings->beginGroup(QString::number(i++));

			s->saveSettings();

			settings->endGroup();
		}

		settings->endGroup();

		clearPasteServices();
		loadPasteServices();
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
		if(format.indexOf('/') == -1)
			continue;

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

void HaveClip::loadPasteServices()
{
	settings->beginGroup("PasteServices");

	foreach(QString i, settings->childGroups())
	{
		settings->beginGroup(i);

		BasePasteService *s;

		switch(settings->value("Type").toInt())
		{
		case BasePasteService::Stikked:
			s = new Stikked(settings, this);
			break;
		case BasePasteService::Pastebin:
			s = new Pastebin(settings, this);
			break;
		default:
			settings->endGroup();
			continue;
		}

		connect(s, SIGNAL(authenticationRequired(BasePasteService*,QString,bool,QString)), this, SLOT(pasteServiceRequiresAuthentication(BasePasteService*,QString,bool,QString)));
		connect(s, SIGNAL(pasted(QUrl)), this, SLOT(receivePasteUrl(QUrl)));
		connect(s, SIGNAL(errorOccured(QString)), this, SLOT(pasteServiceError(QString)));
		connect(s, SIGNAL(untrustedCertificateError(BasePasteService*,QList<QSslError>)), this, SLOT(determineCertificateTrust(BasePasteService*,QList<QSslError>)));

		// Simple paste
		QAction *a = new QAction(tr("Paste to %1").arg(s->label()), this);

		pasteSignalMapper->setMapping(a, s);
		connect(a, SIGNAL(triggered()), pasteSignalMapper, SLOT(map()));

		menu->insertAction(menuSeparator, a);
		pasteActions << a;

		// Advanced paste
		a = new QAction(tr("Advanced paste to %1").arg(s->label()), this);

		pasteAdvSignalMapper->setMapping(a, s);
		connect(a, SIGNAL(triggered()), pasteAdvSignalMapper, SLOT(map()));

		menu->insertAction(menuSeparator, a);
		pasteActions << a;

		a = new QAction(this);
		a->setSeparator(true);
		menu->insertAction(menuSeparator, a);
		pasteActions << a;

		pasteServices << s;

		settings->endGroup();
	}

	settings->endGroup();
}

void HaveClip::clearPasteServices()
{
	foreach(QAction *a, pasteActions)
	{
		menu->removeAction(a);
		pasteSignalMapper->removeMappings(a);
		pasteAdvSignalMapper->removeMappings(a);
		a->deleteLater();
	}

	pasteActions.clear();

	qDeleteAll(pasteServices);
	pasteServices.clear();
}

void HaveClip::simplePaste(QObject *obj)
{
	BasePasteService *service = static_cast<BasePasteService*>(obj);

	switch(service->type())
	{
	case BasePasteService::Stikked:

		break;
	}

	service->paste(currentItem->toPlainText());
}

void HaveClip::advancedPaste(QObject *obj)
{
	BasePasteService *service = static_cast<BasePasteService*>(obj);

	PasteDialog *dlg = new PasteDialog(currentItem->mimeData->text(), service);

	if(dlg->exec() == QDialog::Accepted)
	{
		service->paste(dlg->pasteServiceSettings(), dlg->dataToPaste());
	}

	dlg->deleteLater();
}

void HaveClip::receivePasteUrl(QUrl url)
{
	QMimeData *mime = new QMimeData;

	QString html = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
			"<html><head>"
			"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"</head><body>"
			"<a href=\"%1\">%1</a>"
			"</body></html>";

	mime->setText(url.toString());
	mime->setHtml(html.arg(url.toString()));

	clipboard->setMimeData(mime);
}

void HaveClip::pasteServiceRequiresAuthentication(BasePasteService *service, QString username, bool failed, QString msg)
{
	LoginDialog *dlg = new LoginDialog(username);

	if(failed)
		dlg->setError(tr("Login failed: %1").arg(msg));

	if(dlg->exec() == QDialog::Accepted)
	{
		service->provideAuthentication(dlg->username(), dlg->password());
	}

	dlg->deleteLater();
}

void HaveClip::pasteServiceError(QString error)
{
	QMessageBox::warning(0, tr("Unable to paste"), tr("Paste failed.\n\nError occured: %1").arg(error));
}

void HaveClip::determineCertificateTrust(BasePasteService *service, const QList<QSslError> errors)
{
	CertificateTrustDialog *dlg = new CertificateTrustDialog(service, errors);

	if(dlg->exec() == QDialog::Accepted)
	{
		service->setCertificate(errors.first().certificate());

		if(dlg->remember())
		{
			int i = pasteServices.indexOf(service);

			settings->beginGroup(QString("PasteServices/%1").arg(i));

			service->saveSettings();

			settings->endGroup();
		}

		service->retryPaste();
	}

	dlg->deleteLater();
}

void HaveClip::checkSelection()
{
	if(!isUserSelecting())
	{
		qDebug() << "User stopped selecting";
		clipboardChanged(QClipboard::Selection); // FIXME: user selections is then double checked in clipboardChanged again
	}
}

void HaveClip::loadHistory()
{
	QFile file(historyFilePath());

	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Unable to open history file for reading";
		return;
	}

	QDataStream ds(&file);

	quint32 magic;
	qint32 version;
	ClipboardContent *cnt;

	ds >> magic;

	if(magic != HISTORY_MAGIC_NUMBER)
	{
		qDebug() << "Bad file format: magic number does not match";
		return;
	}

	ds >> version;

	while(!ds.atEnd())
	{
		cnt = ClipboardContent::load(ds);
		cnt->init();

		history << cnt;
	}

	file.close();
}

void HaveClip::saveHistory()
{
	if(!histSave)
		return;

	QFileInfo fi(historyFilePath());
	QDir d;
	d.mkpath(fi.absolutePath());

	qDebug() << "Save history to" << fi.absoluteFilePath();

	QFile file(fi.absoluteFilePath());

	if(!file.open(QIODevice::WriteOnly))
	{
		qDebug() << "Unable to open history file for writing";
		return;
	}

	QDataStream ds(&file);

	ds << (quint32) HISTORY_MAGIC_NUMBER;
	ds << (qint32) HISTORY_VERSION;

	// Saved from oldest to newest
	foreach(ClipboardContent *cnt, history)
		ds << *cnt;

	file.close();
}
