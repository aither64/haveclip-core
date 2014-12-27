#include "Settings.h"

#include <QDesktopServices>
#include <QFile>

#include "ConfigMigrations/V2Migration.h"

Settings* Settings::m_instance = 0;

Settings::Settings(QObject *parent) :
	QObject(parent),
    m_firstStart(false),
    m_nextNodeId(1)
{
	m_settings = new QSettings(this);
}

Settings::~Settings()
{
	m_instance = 0;
}

Settings* Settings::create(QObject *parent)
{
	if(m_instance)
		return m_instance;

	return m_instance = new Settings(parent);
}

Settings* Settings::get()
{
	return m_instance;
}

void Settings::init()
{
    m_firstStart = isFirstLaunch();
    int v = m_settings->value("Version", m_firstStart ? CONFIG_VERSION : 1).toInt();

	if(v != CONFIG_VERSION)
		migrate(v);
	else
		m_settings->setValue("Version", CONFIG_VERSION);

	load();

    if(m_firstStart)
        emit firstStart();
}

bool Settings::isFirstStart() const
{
    return m_firstStart;
}

QString Settings::host()
{
	return m_host;
}

void Settings::setHost(QString host)
{
	if(m_host == host)
		return;

	m_host = host;

	emit hostChanged(host);
}

quint16 Settings::port()
{
	return m_port;
}

void Settings::setPort(quint16 port)
{
	if(m_port == port)
		return;

	m_port = port;

	emit portChanged(port);
}

void Settings::setHostAndPort(QString host, quint16 port)
{
	if(m_host == host && m_port == port)
		return;

	m_host = host;
	m_port = port;

	emit hostAndPortChanged(host, port);
}

bool Settings::allowAutoDiscovery() const
{
	return m_allowAutoDiscovery;
}

void Settings::setAllowAutoDiscovery(bool allow)
{
	if(m_allowAutoDiscovery == allow)
		return;

	m_allowAutoDiscovery = allow;

	emit allowAutoDiscoveryChanged(allow);
}

QString Settings::networkName()
{
	return m_networkName;
}

void Settings::setNetworkName(QString name)
{
	m_networkName = name;
}


quint32 Settings::maxSendSize() const
{
	return m_maxSendSize;
}

void Settings::setMaxSendSize(quint32 size)
{
	if(m_maxSendSize == size)
		return;

	m_maxSendSize = size;

	emit maxSendSizeChanged(size);
}

quint32 Settings::maxReceiveSize() const
{   
	return m_maxRecvSize;
}

void Settings::setMaxReceiveSize(quint32 size)
{
	if(m_maxRecvSize == size)
		return;

	m_maxRecvSize = size;

	emit maxReceiveSizeChanged(size);
}

bool Settings::isHistoryEnabled() const
{
	return m_historyEnabled;
}

void Settings::setHistoryEnabled(bool enabled)
{
	if(m_historyEnabled == enabled)
		return;

	m_historyEnabled = enabled;

	emit historyEnabledChanged(enabled);
}

int Settings::historySize() const
{
	return m_historySize;
}

void Settings::setHistorySize(int size)
{
	if(m_historySize == size)
		return;

	m_historySize = size;

	emit historySizeChanged(size);
}

bool Settings::saveHistory() const
{
	return m_saveHistory;
}

void Settings::setSaveHistory(bool save)
{
	if(m_saveHistory == save)
		return;

	m_saveHistory = save;

	emit saveHistoryChanged(save);
}

bool Settings::isSyncEnabled() const
{
	return m_syncEnabled;
}

void Settings::setSyncEnabled(bool enabled)
{
	if(m_syncEnabled == enabled)
		return;

	m_syncEnabled = enabled;

	m_settings->beginGroup(SETTINGS_SYNC);
	{
		m_settings->setValue("Enable", m_syncEnabled);
	}
	m_settings->endGroup();

	emit syncEnabledChanged(enabled);
}

bool Settings::isSendEnabled() const
{
	return m_sendEnabled;
}

void Settings::setSendEnabled(bool enabled)
{
	if(m_sendEnabled == enabled)
		return;

	m_sendEnabled = enabled;

	m_settings->beginGroup(SETTINGS_SYNC);
	{
		m_settings->setValue("Send", m_sendEnabled);
	}
	m_settings->endGroup();

	emit sendEnabledChanged(enabled);
}

bool Settings::isRecvEnabled() const
{
	return m_recvEnabled;
}

void Settings::setRecvEnabled(bool enabled)
{
	if(m_recvEnabled == enabled)
		return;

	m_recvEnabled = enabled;

	m_settings->beginGroup(SETTINGS_SYNC);
	{
		m_settings->setValue("Receive", m_recvEnabled);
	}
	m_settings->endGroup();

	emit recvEnabledChanged(enabled);
}

ClipboardManager::SynchronizeMode Settings::syncMode() const
{
	return m_syncMode;
}

void Settings::setSyncMode(ClipboardManager::SynchronizeMode mode)
{
	m_syncMode = mode;
}

Communicator::Encryption Settings::encryption() const
{
	return m_encryption;
}

void Settings::setEncryption(Communicator::Encryption enc)
{
	if(m_encryption == enc)
		return;

	m_encryption = enc;

	emit encryptionChanged(enc);
}

QString Settings::certificatePath()
{
	return m_certificatePath;
}

void Settings::setCertificatePath(QString cert)
{
	if(m_certificatePath == cert)
		return;

	m_certificatePath = cert;

	emit certificatePathChanged();

	loadCertificate();
}

QSslCertificate& Settings::certificate()
{
	return m_certificate;
}

QString Settings::privateKeyPath()
{
	return m_privateKeyPath;
}

void Settings::setPrivateKeyPath(QString key)
{
	if(m_privateKeyPath == key)
		return;

	m_privateKeyPath = key;

	emit privateKeyPathChanged();

	loadPrivateKey();
}

QSslKey& Settings::privateKey()
{
	return m_privateKey;
}

Settings::MimeFilterMode Settings::sendFilterMode() const
{
	return m_sendFilterMode;
}

void Settings::setSendFilterMode(Settings::MimeFilterMode mode)
{
	if(m_sendFilterMode == mode)
		return;

	m_sendFilterMode = mode;
	emit sendFilterModeChanged();
}

QStringList Settings::sendFilters() const
{
	return m_sendFilters;
}

void Settings::setSendFilters(QStringList filters)
{
	if(m_sendFilters == filters)
		return;

	m_sendFilters = filters;
	emit sendFiltersChanged();
}

Settings::MimeFilterMode Settings::receiveFilterMode() const
{
	return m_recvFilterMode;
}

void Settings::setReceiveFilterMode(Settings::MimeFilterMode mode)
{
	if(m_recvFilterMode == mode)
		return;

	m_recvFilterMode = mode;
	emit receiveFilterModeChanged();
}

QStringList Settings::receiveFilters() const
{
	return m_recvFilters;
}

void Settings::setReceiveFilters(QStringList filters)
{
	if(m_recvFilters == filters)
		return;

	m_recvFilters = filters;
	emit receiveFiltersChanged();
}

QList<Node> Settings::nodes()
{
	return m_nodes;
}

void Settings::setNodes(QList<Node> &nodes)
{
	m_nodes = nodes;
}

void Settings::addOrUpdateNode(Node &n)
{
	int cnt = m_nodes.count();

	for(int i = 0; i < cnt; i++)
	{
		if(
			(n.hasId() && m_nodes[i].id() == n.id())
			||
			(m_nodes[i].host() == n.host() && m_nodes[i].port() == n.port())
		)
		{
			m_nodes[i].update(n);

			emit nodeUpdated(m_nodes[i]);
			return;
		}
	}

	n.setId();
	m_nodes << n;

	emit nodeAdded(n);
}

void Settings::reloadIdentity()
{
	loadPrivateKey();
	loadCertificate();
}

unsigned int Settings::nextNodeId()
{
	return m_nextNodeId++;
}

void Settings::save()
{
	// Network
	m_settings->beginGroup(SETTINGS_NETWORK);
	{
		m_settings->setValue("Host", m_host);
		m_settings->setValue("Port", m_port);

		// Auto discovery
		m_settings->setValue("AutoDiscovery/Allow", m_allowAutoDiscovery);
		m_settings->setValue("NetworkName", m_networkName);
	}
	m_settings->endGroup();

	// Limits
	m_settings->beginGroup(SETTINGS_NETWORK_LIMITS);
	{
		m_settings->setValue("MaxSendSize", m_maxSendSize);
		m_settings->setValue("MaxReceiveSize", m_maxRecvSize);
	}
	m_settings->endGroup();

	// Filters
	m_settings->beginGroup(SETTINGS_NETWORK_FILTERS);
	{
		m_settings->setValue("SendMode", m_sendFilterMode);
		m_settings->setValue("SendFilters", m_sendFilters);

		m_settings->setValue("RecvMode", m_recvFilterMode);
		m_settings->setValue("RecvFilters", m_recvFilters);
	}
	m_settings->endGroup();

	// Security
	m_settings->beginGroup(SETTINGS_SECURITY);
	{
		m_settings->setValue("Encryption", m_encryption);
		m_settings->setValue("Certificate", m_certificatePath);
		m_settings->setValue("PrivateKey", m_privateKeyPath);
	}
	m_settings->endGroup();

	// History
	m_settings->beginGroup(SETTINGS_HISTORY);
	{
		m_settings->setValue("Enable", m_historyEnabled);
		m_settings->setValue("Size", m_historySize);
		m_settings->setValue("Save", m_saveHistory);
	}
	m_settings->endGroup();

	// Sync
	m_settings->beginGroup(SETTINGS_SYNC);
	{
		m_settings->setValue("Enable", m_syncEnabled);
		m_settings->setValue("Send", m_sendEnabled);
		m_settings->setValue("Receive", m_recvEnabled);
		m_settings->setValue("Synchronize", m_syncMode);
	}
	m_settings->endGroup();

	saveNodes();
}

void Settings::saveNodes()
{
	m_settings->beginGroup(SETTINGS_NODES);
	m_settings->remove("");

	int cnt = m_nodes.count();

	for(int i = 0; i < cnt; i++)
	{
		m_settings->beginGroup(QString::number(i));

		m_nodes[i].save(m_settings);

		m_settings->endGroup();
	}

	m_settings->endGroup();
}

void Settings::load()
{
	// Network
	m_settings->beginGroup(SETTINGS_NETWORK);
	{
		m_host = m_settings->value("Host", "0.0.0.0").toString();
		m_port = m_settings->value("Port", 9999).toInt();

		// Auto discovery
		m_allowAutoDiscovery = m_settings->value("AutoDiscovery/Allow", true).toBool();
		m_networkName = m_settings->value("NetworkName", QHostInfo::localHostName()).toString();
	}
	m_settings->endGroup();

	// Limits
	m_settings->beginGroup(SETTINGS_NETWORK_LIMITS);
	{
		m_maxSendSize = m_settings->value("MaxSendSize", SETTINGS_DEFAULT_MAX_SEND_SIZE).toUInt();
		m_maxRecvSize = m_settings->value("MaxReceiveSize", SETTINGS_DEFAULT_MAX_RECV_SIZE).toUInt();
	}
	m_settings->endGroup();

	// Filters
	m_settings->beginGroup(SETTINGS_NETWORK_FILTERS);
	{
		m_sendFilterMode = (Settings::MimeFilterMode) m_settings->value("SendMode", SETTINGS_DEFAULT_FILTER_SEND_MODE).toInt();
		m_sendFilters = m_settings->value("SendFilters", SETTINGS_DEFAULT_FILTER_SEND_FILTERS).toStringList();

		m_recvFilterMode = (Settings::MimeFilterMode) m_settings->value("RecvMode", SETTINGS_DEFAULT_FILTER_RECV_MODE).toInt();
		m_recvFilters = m_settings->value("RecvFilters", SETTINGS_DEFAULT_FILTER_RECV_FILTERS).toStringList();
	}
	m_settings->endGroup();

	// Security
	m_settings->beginGroup(SETTINGS_SECURITY);
	{
		m_encryption = (Communicator::Encryption) m_settings->value("Encryption", Communicator::Tls).toInt();
		setCertificatePath(m_settings->value("Certificate", dataStoragePath() + "/haveclip.crt").toString());
		setPrivateKeyPath(m_settings->value("PrivateKey", dataStoragePath() + "/haveclip.key").toString());
	}
	m_settings->endGroup();

	// History
	m_settings->beginGroup(SETTINGS_HISTORY);
	{
		m_historyEnabled = m_settings->value("Enable", true).toBool();
		m_historySize = m_settings->value("Size", 10).toInt();
		m_saveHistory = m_settings->value("Save", true).toBool();
	}
	m_settings->endGroup();

	// Sync
	m_settings->beginGroup(SETTINGS_SYNC);
	{
		m_syncEnabled = m_settings->value("Enable", true).toBool();
		m_sendEnabled = m_settings->value("Send", true).toBool();
		m_recvEnabled = m_settings->value("Receive", true).toBool();
		m_syncMode = (ClipboardManager::SynchronizeMode) m_settings->value("Synchronize", ClipboardManager::Both).toInt();
	}
	m_settings->endGroup();

	loadNodes();
}

void Settings::loadNodes()
{
	m_nodes.clear();

	m_settings->beginGroup(SETTINGS_NODES);

	foreach(QString grp, m_settings->childGroups())
	{
		m_settings->beginGroup(grp);

		m_nodes << Node::load(m_settings, nextNodeId());

		m_settings->endGroup();
	}

	m_settings->endGroup();
}

void Settings::reset()
{
	m_settings->remove("");
	m_settings->setValue("Version", CONFIG_VERSION);

	load();
}

void Settings::migrate(int from, int to)
{
	qDebug() << "Migrate config from" << from << "to" << to;

	if(from > to)
	{
		downgrade(from, to);

	} else {
		upgrade(from, to);
	}
}

void Settings::upgrade(int from, int to)
{
	ConfigMigration *migration;

	for(int v = from + 1; v <= to; v++)
	{
		qDebug() << "Upgrade to v" << v;

		migration = createMigration(v);
		migration->up();
		delete migration;

		m_settings->setValue("Version", v);
	}
}

void Settings::downgrade(int from, int to)
{
	ConfigMigration *migration;

	for(int v = from; v > to; v--)
	{
		qDebug() << "Downgrade to v" << v-1;

		migration = createMigration(v);
		migration->down();
		delete migration;

		m_settings->setValue("Version", v-1);
	}
}

ConfigMigration* Settings::createMigration(int v)
{
	ConfigMigration *m;

	switch(v)
	{
	case 2:
		m = new ConfigMigrations::V2Migration(this);
		break;
	default:
		return 0;
	}

	m->setSettings(m_settings);

	return m;
}

QString Settings::dataStoragePath()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
	return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
}

bool Settings::isFirstLaunch()
{
	// Key Version is not present until v0.13.0, therefore the check for another setting
	// that was present in older versions.
	return m_settings->value("Version").isNull() && m_settings->value("Connection/Host").isNull();
}

void Settings::loadCertificate()
{
	QList<QSslCertificate> certs = QSslCertificate::fromPath(m_certificatePath);

	if(certs.empty())
		m_certificate = QSslCertificate();
	else
		m_certificate = certs.first();

	emit certificateChanged(m_certificate);
}

void Settings::loadPrivateKey()
{
	QFile f(m_privateKeyPath);

	if(!f.open(QIODevice::ReadOnly))
	{
		m_privateKey = QSslKey();
		return;
	}

	m_privateKey = QSslKey(&f, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);

	f.close();

	emit privateKeyChanged(m_privateKey);
}
