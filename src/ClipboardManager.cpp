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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QGuiApplication>
#else
#include <QApplication>
#endif

#include <QMimeData>
#include <QStringList>
#include <QUrl>
#include <QImage>
#include <QColor>
#include <QTimer>
#include <QTextDocument>

#include "Network/Receiver.h"
#include "Network/Sender.h"

#ifdef Q_WS_X11
#include <QX11Info>
extern "C" {
	#include <X11/Xlib.h>
	#include <X11/Xatom.h>
}
#endif

ClipboardManager *ClipboardManager::m_instance = 0;
QClipboard *ClipboardManager::clipboard = 0;

QString ClipboardManager::Node::toString()
{
	return host + ":" + QString::number(port);
}

ClipboardManager::ClipboardManager(QObject *parent) :
	QTcpServer(parent),
	clipboardChangedCalled(false),
	uniteCalled(false)
{
	m_instance = this;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	clipboard = QGuiApplication::clipboard();
#else
	clipboard = QApplication::clipboard();
#endif

	m_history = new History(this);

	connect(qApp, SIGNAL(aboutToQuit()), m_history, SLOT(save()));

	delayedEnsureTimer = new QTimer(this);
	delayedEnsureTimer->setSingleShot(true);

	connect(delayedEnsureTimer, SIGNAL(timeout()), this, SLOT(delayedClipboardEnsure()));

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

	m_history->setEnabled(m_settings->value("History/Enable", true).toBool());
	m_history->setStackSize(m_settings->value("History/Size", 10).toInt());
	m_history->setSave(m_settings->value("History/Save", true).toBool());

	m_selectionMode = (ClipboardManager::SelectionMode) m_settings->value("Selection/Mode", ClipboardManager::Separate).toInt();
	m_syncMode = (ClipboardManager::SynchronizeMode) m_settings->value("Sync/Synchronize", ClipboardManager::Both).toInt();

	m_encryption = (ClipboardManager::Encryption) m_settings->value("Connection/Encryption", 0).toInt();
	m_certificate = m_settings->value("Connection/Certificate", "certs/haveclip.crt").toString();
	m_privateKey = m_settings->value("Connection/PrivateKey", "certs/haveclip.key").toString();

	m_password = m_settings->value("AccessPolicy/Password").toString();
}

ClipboardManager::~ClipboardManager()
{
	qDeleteAll(pool);
}

ClipboardManager* ClipboardManager::instance()
{
	return m_instance;
}

void ClipboardManager::start()
{
	// Load history
	m_history->init();

	// Start server
	if(shouldListen())
		startListening();

	// Load contents of clipboard
	clipboardChanged();

#if defined(Q_OS_LINUX)
	connect(clipboard, SIGNAL(changed(QClipboard::Mode)), this, SLOT(clipboardChanged(QClipboard::Mode)));

#elif defined(Q_OS_WIN32)
	// Signal change(QClipboard::Mode) is not sent on Windows
	connect(clipboard, SIGNAL(dataChanged()), this, SLOT(clipboardChanged()));

#elif defined(Q_OS_MAC)
	// There's no notification about clipboard changes on OS X, active checking is needed
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(clipboardChanged()));
	timer->start(1000);
#endif
}

void ClipboardManager::delayedStart(int msecs)
{
	QTimer::singleShot(msecs, this, SLOT(start()));
}

QSettings* ClipboardManager::settings()
{
	return m_settings;
}

History* ClipboardManager::history()
{
	return m_history;
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

QString ClipboardManager::host()
{
	return m_host;
}

quint16 ClipboardManager::port()
{
	return m_port;
}

QString ClipboardManager::password()
{
	return m_password;
}

QList<ClipboardManager::Node*> ClipboardManager::nodes()
{
	return pool;
}

void ClipboardManager::setNodes(QStringList nodes)
{
	m_settings->setValue("Pool/Nodes", nodes);
	loadNodes();
}

void ClipboardManager::setNodes(QList<ClipboardManager::Node*> nodes)
{
	pool = nodes;

	QStringList tmp;

	foreach(Node *n, pool)
		tmp << QString("%1:%2").arg(n->host).arg(n->port);

	m_settings->setValue("Pool/Nodes", tmp);
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
		m_port = port;

		if(isListening())
			close();

		startListening();
	}
}

void ClipboardManager::setHost(QString host)
{
	setListenHost(host, m_port);
}

void ClipboardManager::setPort(quint16 port)
{
	setListenHost(m_host, port);
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

void ClipboardManager::distributeCurrentClipboard()
{
	distributeClipboard(m_history->currentItem());
}

qint32 ClipboardManager::supportedModes()
{
	qint8 ret = ClipboardContainer::Clipboard;

	if(clipboard->supportsSelection())
	{
		ret |= ClipboardContainer::Selection;
		ret |= ClipboardContainer::ClipboardAndSelection;
	}

	if(clipboard->supportsFindBuffer())
		ret |= ClipboardContainer::FindBuffer;

	return ret;
}

void ClipboardManager::gracefullyExit(int sig)
{
	qDebug() << "Gracefully exit";

	qApp->quit();
}

bool ClipboardManager::shouldDistribute() const
{
	return m_clipSync && m_clipSnd;
}

bool ClipboardManager::shouldListen() const
{
	return m_clipSync && m_clipRecv;
}

void ClipboardManager::jumpTo(ClipboardItem *content)
{
	m_history->jumpTo(content);
	updateClipboard(content, true);

	if(shouldDistribute())
		distributeClipboard(content);
}

void ClipboardManager::jumpToItemAt(int index)
{
	jumpTo(m_history->containerAt(index)->item());
}

void ClipboardManager::saveSettings()
{
	m_settings->setValue("History/Enable", m_history->isEnabled());
	m_settings->setValue("History/Size", m_history->stackSize());
	m_settings->setValue("History/Save", m_history->isSaving());

	if(!m_history->isEnabled())
		m_history->deleteFile();

	m_settings->setValue("Selection/Mode", m_selectionMode);
	m_settings->setValue("Sync/Synchronize", m_syncMode);

	m_settings->setValue("Connection/Host", m_host);
	m_settings->setValue("Connection/Port", m_port);
	m_settings->setValue("Connection/Encryption", m_encryption);
	m_settings->setValue("Connection/Certificate", m_certificate);
	m_settings->setValue("Connection/PrivateKey", m_privateKey);

	m_settings->setValue("AccessPolicy/Password", m_password);
}

/**
  Called when system clipboard is changed
  */
void ClipboardManager::clipboardChanged()
{
	clipboardChanged(QClipboard::Clipboard);
}

void ClipboardManager::clipboardChanged(QClipboard::Mode m, bool fromSelection)
{
	if(clipboardChangedCalled)
	{
		qDebug() << "ClipboardChanged already called, end";
		return;

	} else if((m == QClipboard::Clipboard && clipboard->ownsClipboard())
		  || (m == QClipboard::Selection && clipboard->ownsSelection())) {
		qDebug() << "HaveClip already owns the clipboard/selection, ignore";
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

	ClipboardItem::Mode mode = ClipboardItem::qtModeToOwn(m);
	const QMimeData *mimeData = clipboard->mimeData(m);
	QMimeData *copiedMimeData;

	if(!mimeData)
	{
		qDebug() << "Clipboard is empty";
		clipboardChangedCalled = false;
		return;
	}

	if(m == QClipboard::Selection) // Selection has only text and html
	{
		copiedMimeData = new QMimeData();

		if(mimeData->hasText())
			copiedMimeData->setText(mimeData->text());

		if(mimeData->hasHtml())
			copiedMimeData->setHtml(mimeData->html());
	} else
		copiedMimeData = copyMimeData(mimeData);

	ClipboardItem *cnt = new ClipboardItem(mode, copiedMimeData);
	ClipboardItem *lastItem = m_history->lastItem();

	if(lastItem && *lastItem == *cnt)
	{
		if(lastItem->mode != ClipboardItem::ClipboardAndSelection && lastItem->mode != cnt->mode)
		{
			lastItem->mode = ClipboardItem::ClipboardAndSelection;

			if(shouldDistribute())
				distributeClipboard(lastItem);
		}

		delete cnt;
		clipboardChangedCalled = false;
		return;

	} else if(lastItem && cnt->formats.isEmpty()) { // empty clipboard, restore last content
		qDebug() << "Clipboard is empty, reset";
		updateClipboard(lastItem, true);
		delete cnt;
		clipboardChangedCalled = false;
		return;
	}

	cnt->init();

	lastItem = m_history->add(cnt, !fromSelection);

	if(m_selectionMode == ClipboardManager::United)
		uniteClipboards(lastItem);

	if(shouldDistribute())
		distributeClipboard(lastItem);

	clipboardChangedCalled = false;
}

void ClipboardManager::distributeClipboard(ClipboardItem *content)
{
	foreach(Node *n, pool)
	{
		Sender *d = new Sender(m_history, m_encryption, n, this);
		d->setPassword(m_password);

		connect(d, SIGNAL(untrustedCertificateError(ClipboardManager::Node*,QList<QSslError>)), this, SIGNAL(untrustedCertificateError(ClipboardManager::Node*,QList<QSslError>)));
		connect(d, SIGNAL(sslFatalError(QList<QSslError>)), this, SIGNAL(sslFatalError(QList<QSslError>)));

		d->distribute(content);
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
	Receiver *c = new Receiver(m_history, m_encryption, this);
	c->setSocketDescriptor(handle);

	connect(c, SIGNAL(clipboardUpdated(ClipboardContainer*)), this, SLOT(updateClipboardFromNetwork(ClipboardContainer*)));

	c->setCertificateAndKey(m_certificate, m_privateKey);
	c->setPassword(m_password);
	c->communicate();
}

/**
  Called when new clipboard is received via network
  */
void ClipboardManager::updateClipboard(ClipboardContainer *cont, bool fromHistory)
{
	qDebug() << "Update clipboard";
	ClipboardItem *it = cont->item();

	// FIXME
	if(!fromHistory)
		it = m_history->add(it, false);

	if(m_selectionMode == ClipboardManager::United || it->mode == ClipboardItem::ClipboardAndSelection)
	{
		uniteClipboards(it);
	} else
		clipboard->setMimeData(copyMimeData(it->mimeData()), ClipboardItem::ownModeToQt(it->mode));

	qDebug() << "Update clipboard end";
}

void ClipboardManager::updateClipboardFromNetwork(ClipboardContainer *cont)
{
	clipboardChangedCalled = true;

	updateClipboard(cont);

	clipboardChangedCalled = false;
}

void ClipboardManager::uniteClipboards(ClipboardItem *content)
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

void ClipboardManager::ensureClipboardContent(ClipboardItem *content, QClipboard::Mode mode)
{
	qDebug() << "Ensure clipboard content" << content->toPlainText() << mode;
//	if(!ClipboardContent::compareMimeData(content->mimeData, clipboard->mimeData(mode), mode == QClipboard::Selection))
//	{
//		qDebug() << "Update" << mode;
		clipboard->setMimeData(copyMimeData(content->mimeData()), mode);
//	} else {
//		qDebug() << "No need to update" << mode;
//	}
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
	toggleClipboardSending(enabled, true);
	toggleClipboardReceiving(enabled, true);

	m_clipSync = enabled;
	m_settings->setValue("Sync/Enable", m_clipSync);
}

void ClipboardManager::toggleClipboardSending(bool enabled, bool masterChange)
{
	if(masterChange)
		return;

	m_clipSnd = enabled;
	m_settings->setValue("Sync/Send", m_clipSnd);
}

void ClipboardManager::toggleClipboardReceiving(bool enabled, bool masterChange)
{
	if(enabled && !shouldListen())
		startListening();
	else if(!enabled && shouldListen())
		close();

	if(!masterChange)
	{
		m_clipRecv = enabled;
		m_settings->setValue("Sync/Receive", m_clipRecv);
	}
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

void ClipboardManager::delayedClipboardEnsure()
{
	qDebug() << "Delayed ensure";

	clipboardChangedCalled = true;

	if(delayedEnsureItem->mode == ClipboardContainer::ClipboardAndSelection)
	{
		ensureClipboardContent(delayedEnsureItem, QClipboard::Selection);
		ensureClipboardContent(delayedEnsureItem, QClipboard::Clipboard);

	} else
		ensureClipboardContent(delayedEnsureItem, ClipboardContainer::ownModeToQt(delayedEnsureItem->mode));

	clipboardChangedCalled = false;
}

#ifdef Q_WS_X11
void ClipboardManager::checkSelection()
{
	if(!isUserSelecting())
	{
		qDebug() << "User stopped selecting";
		clipboardChanged(QClipboard::Selection, true); // FIXME: user selections is then double checked in clipboardChanged again
	}
}
#endif
