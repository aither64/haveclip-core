#include "Settings.h"

#include <QDesktopServices>
#include <QFile>

#include "ConfigMigrations/V2Migration.h"

Settings* Settings::m_instance = 0;

Settings::Settings(QObject *parent) :
	QObject(parent),
	m_nextNodeId(1)
{
	m_settings = new QSettings(this);

	int v = m_settings->value("Version", isFirstLaunch() ? CONFIG_VERSION : 1).toInt();

	if(v != CONFIG_VERSION)
		migrate(v);
	else
		m_settings->setValue("Version", CONFIG_VERSION);

	load();
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

QString Settings::host()
{
	return m_host;
}

void Settings::setHost(QString host)
{
	m_host = host;

	emit hostChanged(host);
}

quint16 Settings::port()
{
	return m_port;
}

void Settings::setPort(quint16 port)
{
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

bool Settings::isHistoryEnabled() const
{
	return m_historyEnabled;
}

void Settings::setHistoryEnabled(bool enabled)
{
	m_historyEnabled = enabled;
}

int Settings::historySize() const
{
	return m_historySize;
}

void Settings::setHistorySize(int size)
{
	m_historySize = size;
}

bool Settings::saveHistory() const
{
	return m_saveHistory;
}

void Settings::setSaveHistory(bool save)
{
	m_saveHistory = save;

	emit saveHistoryChanged(save);
}

bool Settings::isSyncEnabled() const
{
	return m_syncEnabled;
}

void Settings::setSyncEnabled(bool enabled)
{
	m_syncEnabled = enabled;

	emit syncEnabledChanged(enabled);
}

bool Settings::isSendEnabled() const
{
	return m_sendEnabled;
}

void Settings::setSendEnabled(bool enabled)
{
	m_sendEnabled = enabled;
}

bool Settings::isRecvEnabled() const
{
	return m_recvEnabled;
}

void Settings::setRecvEnabled(bool enabled)
{
	m_recvEnabled = enabled;
}

ClipboardManager::SynchronizeMode Settings::syncMode() const
{
	return m_syncMode;
}

void Settings::setSyncMode(ClipboardManager::SynchronizeMode mode)
{
	m_syncMode = mode;
}

ConnectionManager::Encryption Settings::encryption() const
{
	return m_encryption;
}

void Settings::setEncryption(ConnectionManager::Encryption enc)
{
	m_encryption = enc;

	emit encryptionChanged(enc);
}

QString Settings::certificatePath()
{
	return m_certificatePath;
}

void Settings::setCertificatePath(QString cert)
{
	m_certificatePath = cert;

	QList<QSslCertificate> certs = QSslCertificate::fromPath(cert);

	if(certs.empty())
		m_certificate = QSslCertificate();
	else
		m_certificate = certs.first();

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
	m_privateKeyPath = key;

	QFile f(key);

	if(!f.open(QIODevice::ReadOnly))
	{
		m_privateKey = QSslKey();
		return;
	}

	m_privateKey = QSslKey(&f, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey);

	f.close();
}

QSslKey& Settings::privateKey()
{
	return m_privateKey;
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

unsigned int Settings::nextNodeId()
{
	return m_nextNodeId++;
}

void Settings::save()
{
	// Connection
	m_settings->setValue("Connection/Host", m_host);
	m_settings->setValue("Connection/Port", m_port);
	m_settings->setValue("Connection/Encryption", m_encryption);
	m_settings->setValue("Connection/Certificate", m_certificatePath);
	m_settings->setValue("Connection/PrivateKey", m_privateKeyPath);

	// Auto discovery
	m_settings->setValue("Connection/AutoDiscovery/Allow", m_allowAutoDiscovery);
	m_settings->setValue("Connection/NetworkName", m_networkName);

	// History
	m_settings->setValue("History/Enable", m_historyEnabled);
	m_settings->setValue("History/Size", m_historySize);
	m_settings->setValue("History/Save", m_saveHistory);

	// Sync
	m_settings->setValue("Sync/Enable", m_syncEnabled);
	m_settings->setValue("Sync/Send", m_sendEnabled);
	m_settings->setValue("Sync/Receive", m_recvEnabled);
	m_settings->setValue("Sync/Synchronize", m_syncMode);

	saveNodes();
}

void Settings::saveNodes()
{
	m_settings->beginGroup("Pool/Nodes");
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
	// Connection
	m_host = m_settings->value("Connection/Host", "0.0.0.0").toString();
	m_port = m_settings->value("Connection/Port", 9999).toInt();

	m_encryption = (ConnectionManager::Encryption) m_settings->value("Connection/Encryption", ConnectionManager::Tls).toInt();
	setCertificatePath(m_settings->value("Connection/Certificate", dataStoragePath() + "/haveclip.crt").toString());
	setPrivateKeyPath(m_settings->value("Connection/PrivateKey", dataStoragePath() + "/haveclip.key").toString());

	// Auto discovery
	m_allowAutoDiscovery = m_settings->value("Connection/AutoDiscovery/Allow", true).toBool();
	m_networkName = m_settings->value("Connection/NetworkName", QHostInfo::localHostName()).toString();

	// History
	m_historyEnabled = m_settings->value("History/Enable", true).toBool();
	m_historySize = m_settings->value("History/Size", 10).toInt();
	m_saveHistory = m_settings->value("History/Save", true).toBool();

	// Sync
	m_syncEnabled = m_settings->value("Sync/Enable", true).toBool();
	m_sendEnabled = m_settings->value("Sync/Send", true).toBool();
	m_recvEnabled = m_settings->value("Sync/Receive", true).toBool();
	m_syncMode = (ClipboardManager::SynchronizeMode) m_settings->value("Sync/Synchronize", ClipboardManager::Both).toInt();

	loadNodes();
}

void Settings::loadNodes()
{
	m_settings->beginGroup("Pool/Nodes");

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
