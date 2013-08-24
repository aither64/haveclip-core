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

#include "ClipboardManager.h"

#include <QApplication>
#include <QMimeData>
#include <QStringList>
#include <QUrl>
#include <QImage>
#include <QColor>
#include <QLabel>
#include <QTimer>
#include <QTextDocument>
#include <QFileInfo>
#include <QDesktopServices>
#include <QDir>

#include "Receiver.h"
#include "Sender.h"

#include "PasteServices/PasteDialog.h"
#include "PasteServices/Stikked/Stikked.h"
#include "PasteServices/Pastebin/Pastebin.h"

#ifdef Q_WS_X11
#include <QX11Info>
extern "C" {
	#include <X11/Xlib.h>
}
#endif

QString ClipboardManager::Node::toString()
{
	return host + ":" + QString::number(port);
}

ClipboardManager::ClipboardManager(QObject *parent) :
	QTcpServer(parent),
	m_currentItem(0),
	clipboardChangedCalled(false),
	uniteCalled(false)
{
	clipboard = QApplication::clipboard();

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
	m_settings = new QSettings(this);

	loadNodes();

	m_host = m_settings->value("Connection/Host", "0.0.0.0").toString();
	m_port = m_settings->value("Connection/Port", 9999).toInt();

	m_clipSync = m_settings->value("Sync/Enable", true).toBool();
	m_clipSnd = m_settings->value("Sync/Send", true).toBool();
	m_clipRecv = m_settings->value("Sync/Receive", true).toBool();

	m_histEnabled = m_settings->value("History/Enable", true).toBool();
	m_histSize = m_settings->value("History/Size", 10).toInt();
	m_histSave = m_settings->value("History/Save", true).toBool();

	m_selectionMode = (ClipboardManager::SelectionMode) m_settings->value("Selection/Mode", ClipboardManager::Separate).toInt();
	m_syncMode = (ClipboardManager::SynchronizeMode) m_settings->value("Sync/Synchronize", ClipboardManager::Both).toInt();

	m_encryption = (ClipboardManager::Encryption) m_settings->value("Connection/Encryption", 0).toInt();
	m_certificate = m_settings->value("Connection/Certificate", "certs/haveclip.crt").toString();
	m_privateKey = m_settings->value("Connection/PrivateKey", "certs/haveclip.key").toString();

	m_password = m_settings->value("AccessPolicy/Password").toString();

	loadPasteServices();
}

ClipboardManager::~ClipboardManager()
{
	qDeleteAll(pool);
	qDeleteAll(m_history);
}

void ClipboardManager::start()
{
	// Load history
	if(m_histSave)
		loadHistory();
	else
		deleteHistoryFile();

	// Start server
	startListening();

	// Load contents of clipboard
	clipboardChanged();
}

QSettings* ClipboardManager::settings()
{
	return m_settings;
}

QList<BasePasteService*> ClipboardManager::pasteServices()
{
	return m_pasteServices;
}

QList<ClipboardContent*> ClipboardManager::history()
{
	return m_history;
}

ClipboardContent* ClipboardManager::currentItem()
{
	return m_currentItem;
}

bool ClipboardManager::isHistoryEnabled()
{
	return m_histEnabled;
}

bool ClipboardManager::isSyncEnabled()
{
	return m_clipSync;
}

bool ClipboardManager::isSendingEnabled()
{
	return m_clipSnd;
}

bool ClipboardManager::isReceivingEnabled()
{
	return m_clipRecv;
}

void ClipboardManager::setNodes(QStringList nodes)
{
	m_settings->setValue("Pool/Nodes", nodes);
	loadNodes();
}

void ClipboardManager::setHistoryEnabled(bool enable)
{
	m_histEnabled = enable;
}

void ClipboardManager::setHistorySize(int size)
{
	m_histSize = size;
}

void ClipboardManager::setHistorySave(bool save)
{
	m_histSave = save;
}

void ClipboardManager::setSelectionMode(SelectionMode m)
{
	m_selectionMode = m;
}

void ClipboardManager::setSyncMode(SynchronizeMode m)
{
	m_syncMode = m;
}

void ClipboardManager::setListenHost(QString host, quint16 port)
{
	if(host != m_host || port != m_port)
	{
		m_host = host;

		if(isListening())
			close();

		startListening();
	}
}

void ClipboardManager::setEncryption(Encryption encryption)
{
	m_encryption = encryption;
}

void ClipboardManager::setCertificate(QString cert)
{
	m_certificate = cert;
}

void ClipboardManager::setPrivateKey(QString key)
{
	m_privateKey = key;
}

void ClipboardManager::setPassword(QString pass)
{
	m_password = pass;
}

void ClipboardManager::setPasteServices(QList<BasePasteService*> services)
{
	m_pasteServices = services;
}

void ClipboardManager::distributeCurrentClipboard()
{
	distributeClipboard(m_currentItem);
}

void ClipboardManager::jumpTo(ClipboardContent *content)
{
	popToFront(content);
	updateClipboard(content, true);

	if(m_clipSnd)
		distributeClipboard(content);
}

void ClipboardManager::saveSettings()
{
	m_settings->setValue("History/Enable", m_histEnabled);
	m_settings->setValue("History/Size", m_histSize);
	m_settings->setValue("History/Save", m_histSave);

	if(!m_histSave)
		deleteHistoryFile();

	m_settings->setValue("Selection/Mode", m_selectionMode);
	m_settings->setValue("Sync/Synchronize", m_syncMode);

	m_settings->setValue("Connection/Host", m_host);
	m_settings->setValue("Connection/Port", m_port);
	m_settings->setValue("Connection/Encryption", m_encryption);
	m_settings->setValue("Connection/Certificate", m_certificate);
	m_settings->setValue("Connection/PrivateKey", m_privateKey);

	m_settings->setValue("AccessPolicy/Password", m_password);

	// Paste services
	m_settings->beginGroup("PasteServices");
	m_settings->remove("");

	int i = 0;

	foreach(BasePasteService *s, m_pasteServices)
	{
		m_settings->beginGroup(QString::number(i++));

		s->saveSettings();

		m_settings->endGroup();
	}

	m_settings->endGroup();

	clearPasteServices();
	loadPasteServices();
}

/**
  Called when system clipboard is changed
  */
void ClipboardManager::clipboardChanged()
{
	clipboardChanged(QClipboard::Clipboard);
}

void ClipboardManager::clipboardChanged(QClipboard::Mode m)
{
	if(clipboardChangedCalled)
	{
		qDebug() << "ClipboardChanged already called, end";
		return;
	}

	clipboardChangedCalled = true;

	if((m != QClipboard::Clipboard && m != QClipboard::Selection)
			|| (m_syncMode != ClipboardManager::Both
			    && ((m == QClipboard::Selection && m_syncMode == ClipboardManager::Clipboard) || (m == QClipboard::Clipboard && m_syncMode == ClipboardManager::Selection)))
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

	if(m_currentItem && *m_currentItem == *cnt)
	{
		if(m_currentItem->mode != ClipboardContent::ClipboardAndSelection && m_currentItem->mode != cnt->mode)
		{
			m_currentItem->mode = ClipboardContent::ClipboardAndSelection;
			distributeClipboard(m_currentItem);
		}

		delete cnt;
		clipboardChangedCalled = false;
		return;

	} else if(m_currentItem && cnt->formats.isEmpty()) { // empty clipboard, restore last content
		qDebug() << "Clipboard is empty, reset";
		updateClipboard(m_currentItem, true);
		delete cnt;
		clipboardChangedCalled = false;
		return;
	}

	cnt->init();

	addToHistory(cnt);

	emit historyChanged();

	if(m_selectionMode == ClipboardManager::United)
		uniteClipboards(m_currentItem);

	if(m_clipSnd)
		distributeClipboard(m_currentItem);

	clipboardChangedCalled = false;
}

void ClipboardManager::distributeClipboard(ClipboardContent *content, bool deleteLater)
{
	foreach(Node *n, pool)
	{
		Sender *d = new Sender(m_encryption, n, this);
		d->setDeleteContentOnSent(deleteLater);

		connect(d, SIGNAL(untrustedCertificateError(ClipboardManager::Node*,QList<QSslError>)), this, SIGNAL(untrustedCertificateError(ClipboardManager::Node*,QList<QSslError>)));
		connect(d, SIGNAL(sslFatalError(QList<QSslError>)), this, SIGNAL(sslFatalError(QList<QSslError>)));

		d->distribute(content, m_password);
	}
}

#ifdef Q_WS_X11
bool ClipboardManager::isUserSelecting()
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

void ClipboardManager::incomingConnection(int handle)
{
	Receiver *c = new Receiver(m_encryption, this);
	c->setSocketDescriptor(handle);

	connect(c, SIGNAL(clipboardUpdated(ClipboardContent*)), this, SLOT(updateClipboard(ClipboardContent*)));

	c->setCertificateAndKey(m_certificate, m_privateKey);
	c->setAcceptPassword(m_password);
	c->communicate();
}

/**
  Called when new clipboard is received via network
  */
void ClipboardManager::updateClipboard(ClipboardContent *content, bool fromHistory)
{
	qDebug() << "Update clipboard";

	m_currentItem = content;

	if(m_selectionMode == ClipboardManager::United || content->mode == ClipboardContent::ClipboardAndSelection)
	{
		uniteClipboards(content);
	} else
		clipboard->setMimeData(copyMimeData(content->mimeData), ClipboardContent::ownModeToQt(content->mode));

	// FIXME
	if(!fromHistory)
		addToHistory(content);

	emit historyChanged();

	qDebug() << "Update clipboard end";
}

void ClipboardManager::uniteClipboards(ClipboardContent *content)
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

void ClipboardManager::ensureClipboardContent(ClipboardContent *content, QClipboard::Mode mode)
{
	if(!ClipboardContent::compareMimeData(content->mimeData, clipboard->mimeData(mode), mode == QClipboard::Selection))
	{
		qDebug() << "Update" << mode;
		clipboard->setMimeData(copyMimeData(content->mimeData), mode);
	} else {
		qDebug() << "No need to update" << mode;
	}
}

void ClipboardManager::addToHistory(ClipboardContent *content)
{
	if(!m_histEnabled)
	{
		if(m_currentItem)
			delete m_currentItem;

		m_currentItem = content;

		return;
	}

	foreach(ClipboardContent *c, m_history)
	{
		if(*c == *content)
		{
			m_currentItem = c;

			if(m_currentItem->mode != content->mode)
				m_currentItem->mode = ClipboardContent::ClipboardAndSelection;

			if(c != content)
				delete content;

			popToFront(m_currentItem);
			return;
		}
	}

	if(m_history.size() >= m_histSize)
		delete m_history.takeFirst();

	m_history << content;
	m_currentItem = content;
}

void ClipboardManager::popToFront(ClipboardContent *content)
{
	m_history.removeOne(content);
	m_history << content;
}

QString ClipboardManager::historyFilePath()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	return QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/history.dat";
#else
	return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/history.dat";
#endif
}

void ClipboardManager::deleteHistoryFile()
{
	QFile::remove(historyFilePath());
}

void ClipboardManager::loadNodes()
{
	pool.clear();

	foreach(QString node, m_settings->value("Pool/Nodes").toStringList())
	{
		Node *n = new Node;
		n->host = node.section(':', 0, 0);
		n->port = node.section(':', 1, 1).toUShort();

		QByteArray cert = m_settings->value("Node:" + n->toString() + "/Certificate").toString().toUtf8();

		if(!cert.isEmpty())
			n->certificate = QSslCertificate::fromData(cert).first();

		pool << n;
	}
}

void ClipboardManager::toggleSharedClipboard(bool enabled)
{
	m_clipSync = enabled;

	toggleClipboardSending(enabled);
	toggleClipboardReceiving(enabled);

	m_settings->setValue("Sync/Enable", m_clipSync);
}

void ClipboardManager::toggleClipboardSending(bool enabled)
{
	m_clipSnd = enabled;

	m_settings->setValue("Sync/Send", m_clipSnd);
}

void ClipboardManager::toggleClipboardReceiving(bool enabled)
{
	if(enabled && !m_clipRecv)
		startListening();
	else if(!enabled && m_clipRecv)
		close();

	m_clipRecv = enabled;
	m_settings->setValue("Sync/Receive", m_clipRecv);
}

QMimeData* ClipboardManager::copyMimeData(const QMimeData *mimeReference)
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

void ClipboardManager::startListening(QHostAddress addr)
{
	if(addr.isNull())
		addr.setAddress(m_host);

	if(!addr.isNull())
	{
		if(!listen(addr, m_port))
		{
			//QMessageBox::warning(0, tr("Unable to start listening"), tr("Listening failed, clipboard receiving is not active!\n\n") + errorString());
			emit listenFailed(tr("Listen failed: %1").arg(errorString()));
			return;
		}

		qDebug() << "Listening on" << serverAddress() << serverPort();

	} else {
		QHostInfo::lookupHost(m_host, this, SLOT(listenOnHost(QHostInfo)));
	}
}

void ClipboardManager::listenOnHost(const QHostInfo &host)
{
	if(host.error() != QHostInfo::NoError) {
//		QMessageBox::warning(0, tr("Unable to resolve hostname"), tr("Resolving failed: ") + host.errorString());
		emit listenFailed(tr("Resolving failed: %1").arg(host.errorString()));
		return;
	}

	QList<QHostAddress> addrs = host.addresses();

	if(addrs.size() == 0)
	{
//		QMessageBox::warning(0, tr("Hostname has no IP address"), tr("Hostname has no IP addresses. Clipboard receiving is not active."));
		emit listenFailed(tr("Hostname has no IP addresses. Clipboard receiving is not active."));
		return;
	}

	startListening(addrs.first());
}

void ClipboardManager::loadPasteServices()
{
	m_settings->beginGroup("PasteServices");

	foreach(QString i, m_settings->childGroups())
	{
		m_settings->beginGroup(i);

		BasePasteService *s;

		switch(m_settings->value("Type").toInt())
		{
		case BasePasteService::Stikked:
			s = new Stikked(m_settings, this);
			break;
		case BasePasteService::Pastebin:
			s = new Pastebin(m_settings, this);
			break;
		default:
			m_settings->endGroup();
			continue;
		}

		// FIXME
//		connect(s, SIGNAL(authenticationRequired(BasePasteService*,QString,bool,QString)), this, SLOT(pasteServiceRequiresAuthentication(BasePasteService*,QString,bool,QString)));
		connect(s, SIGNAL(pasted(QUrl)), this, SLOT(receivePasteUrl(QUrl)));
//		connect(s, SIGNAL(errorOccured(QString)), this, SLOT(pasteServiceError(QString)));
//		connect(s, SIGNAL(untrustedCertificateError(BasePasteService*,QList<QSslError>)), this, SLOT(determineCertificateTrust(BasePasteService*,QList<QSslError>)));

		m_pasteServices << s;

		m_settings->endGroup();
	}

	m_settings->endGroup();
}

void ClipboardManager::clearPasteServices()
{
	m_pasteServices.clear();
}

void ClipboardManager::receivePasteUrl(QUrl url)
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

#ifdef Q_WS_X11
void ClipboardManager::checkSelection()
{
	if(!isUserSelecting())
	{
		qDebug() << "User stopped selecting";
		clipboardChanged(QClipboard::Selection); // FIXME: user selections is then double checked in clipboardChanged again
	}
}
#endif

void ClipboardManager::loadHistory()
{
	QFile file(historyFilePath());

	if(!file.open(QIODevice::ReadOnly))
	{
		qDebug() << "Unable to open history file for reading";
		return;
	}

	QDataStream ds(&file);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	ds.setVersion(QDataStream::Qt_5_1);
#else
	ds.setVersion(QDataStream::Qt_4_6);
#endif

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

		m_history << cnt;
	}

	file.close();
}

void ClipboardManager::saveHistory()
{
	if(!m_histSave)
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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	ds.setVersion(QDataStream::Qt_5_1);
#else
	ds.setVersion(QDataStream::Qt_4_6);
#endif

	ds << (quint32) HISTORY_MAGIC_NUMBER;
	ds << (qint32) HISTORY_VERSION;

	// Saved from oldest to newest
	foreach(ClipboardContent *cnt, m_history)
		ds << *cnt;

	file.close();
}
