#ifndef SETTINGS_H
#define SETTINGS_H

#define SETTINGS_NETWORK "Network"
#define SETTINGS_NETWORK_LIMITS "Network/Limits"
#define SETTINGS_NETWORK_FILTERS "Network/Filters"
#define SETTINGS_HISTORY "History"
#define SETTINGS_SYNC "Sync"
#define SETTINGS_SECURITY "Security"
#define SETTINGS_POOL "Pool"
#define SETTINGS_NODES "Pool/Nodes"

#if defined(MER_SAILFISH) // Sailfish default settings
#define SETTINGS_DEFAULT_MAX_SEND_SIZE (5*1024*1024)
#define SETTINGS_DEFAULT_MAX_RECV_SIZE (5*1024*1024)

#define SETTINGS_DEFAULT_FILTER_SEND_MODE Settings::Accept
#define SETTINGS_DEFAULT_FILTER_SEND_FILTERS (QStringList() << "text/.+")

#define SETTINGS_DEFAULT_FILTER_RECV_MODE Settings::Accept
#define SETTINGS_DEFAULT_FILTER_RECV_FILTERS (QStringList() << "text/.+")

#else // All other platforms

#define SETTINGS_DEFAULT_MAX_SEND_SIZE 100*1024*1024
#define SETTINGS_DEFAULT_MAX_RECV_SIZE 100*1024*1024

#define SETTINGS_DEFAULT_FILTER_SEND_MODE Settings::Except
#define SETTINGS_DEFAULT_FILTER_SEND_FILTERS (QStringList())

#define SETTINGS_DEFAULT_FILTER_RECV_MODE Settings::Except
#define SETTINGS_DEFAULT_FILTER_RECV_FILTERS (QStringList())
#endif

#include <QObject>
#include <QSettings>
#include <QSslCertificate>
#include <QSslKey>

#include "Version.h"
#include "ClipboardManager.h"
#include "Node.h"

class ConfigMigration;

class Settings : public QObject
{
	Q_OBJECT
public:
	enum MimeFilterMode {
		Accept=0,
		Except
	};

	Q_ENUMS(MimeFilterMode)

	~Settings();
	static Settings* create(QObject *parent = 0);
	static Settings* get();

	void init();

    Q_PROPERTY(bool firstStart READ isFirstStart)
    bool isFirstStart() const;

	Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
	QString host();
	void setHost(QString host);

	Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
	quint16 port();
	void setPort(quint16 port);

	void setHostAndPort(QString host, quint16 port);

	Q_PROPERTY(bool allowAutoDiscovery READ allowAutoDiscovery WRITE setAllowAutoDiscovery NOTIFY allowAutoDiscoveryChanged)
	bool allowAutoDiscovery() const;
	void setAllowAutoDiscovery(bool allow);

	Q_PROPERTY(QString networkName READ networkName WRITE setNetworkName NOTIFY networkNameChanged)
	QString networkName();
	void setNetworkName(QString name);

	Q_PROPERTY(quint32 maxSendSize READ maxSendSize WRITE setMaxSendSize NOTIFY maxSendSizeChanged)
	quint32 maxSendSize() const;
	void setMaxSendSize(quint32 size);

	Q_PROPERTY(quint32 maxReceiveSize READ maxReceiveSize WRITE setMaxReceiveSize NOTIFY maxReceiveSizeChanged)
	quint32 maxReceiveSize() const;
	void setMaxReceiveSize(quint32 size);

	Q_PROPERTY(bool historyEnabled READ isHistoryEnabled WRITE setHistoryEnabled NOTIFY historyEnabledChanged)
	bool isHistoryEnabled() const;
	void setHistoryEnabled(bool enabled);

	Q_PROPERTY(int historySize READ historySize WRITE setHistorySize NOTIFY historySizeChanged)
	int historySize() const;
	void setHistorySize(int size);

	Q_PROPERTY(bool saveHistory READ saveHistory WRITE setSaveHistory NOTIFY saveHistoryChanged)
	bool saveHistory() const;
	void setSaveHistory(bool save);

	Q_PROPERTY(bool syncEnabled READ isSyncEnabled WRITE setSyncEnabled NOTIFY syncEnabledChanged)
	bool isSyncEnabled() const;
	void setSyncEnabled(bool enabled);

	Q_PROPERTY(bool sendEnabled READ isSendEnabled WRITE setSendEnabled NOTIFY sendEnabledChanged)
	bool isSendEnabled() const;
	void setSendEnabled(bool enabled);

	Q_PROPERTY(bool recvEnabled READ isRecvEnabled WRITE setRecvEnabled NOTIFY recvEnabledChanged)
	bool isRecvEnabled() const;
	void setRecvEnabled(bool enabled);

	Q_PROPERTY(ClipboardManager::SynchronizeMode syncMode READ syncMode WRITE setSyncMode NOTIFY syncModeChanged)
	ClipboardManager::SynchronizeMode syncMode() const;
	void setSyncMode(ClipboardManager::SynchronizeMode mode);

	Q_PROPERTY(Communicator::Encryption encryption READ encryption WRITE setEncryption NOTIFY encryptionChanged)
	Communicator::Encryption encryption() const;
	void setEncryption(Communicator::Encryption enc);

	Q_PROPERTY(QString certificatePath READ certificatePath WRITE setCertificatePath NOTIFY certificatePathChanged)
	QString certificatePath();
	void setCertificatePath(QString cert);

	Q_PROPERTY(QSslCertificate certificate READ certificate NOTIFY certificateChanged)
	QSslCertificate& certificate();

	Q_PROPERTY(QString privateKeyPath READ privateKeyPath WRITE setPrivateKeyPath NOTIFY privateKeyPathChanged)
	QString privateKeyPath();
	void setPrivateKeyPath(QString key);

	Q_PROPERTY(QSslKey privateKey READ privateKey NOTIFY privateKeyChanged)
	QSslKey& privateKey();

	Q_PROPERTY(Settings::MimeFilterMode sendFilterMode READ sendFilterMode WRITE setSendFilterMode NOTIFY sendFilterModeChanged)
	MimeFilterMode sendFilterMode() const;
	void setSendFilterMode(MimeFilterMode mode);

	Q_PROPERTY(QStringList sendFilters READ sendFilters WRITE setSendFilters NOTIFY sendFiltersChanged)
	QStringList sendFilters() const;
	void setSendFilters(QStringList filters);

	Q_PROPERTY(Settings::MimeFilterMode receiveFilterMode READ receiveFilterMode WRITE setReceiveFilterMode NOTIFY receiveFilterModeChanged)
	MimeFilterMode receiveFilterMode() const;
	void setReceiveFilterMode(MimeFilterMode mode);

	Q_PROPERTY(QStringList receiveFilters READ receiveFilters WRITE setReceiveFilters NOTIFY receiveFiltersChanged)
	QStringList receiveFilters() const;
	void setReceiveFilters(QStringList filters);

	QList<Node> nodes();
	void setNodes(QList<Node> &nodes);
	void addOrUpdateNode(Node &n);

	Q_INVOKABLE void reloadIdentity();

	unsigned int nextNodeId();

	Q_INVOKABLE void save();
	Q_INVOKABLE void reset();

signals:
	void firstStart();
	void hostChanged(QString host);
	void portChanged(quint16 port);
	void hostAndPortChanged(QString host, quint16 port);
	void allowAutoDiscoveryChanged(bool allow);
	void networkNameChanged(QString name);
	void maxSendSizeChanged(quint32 size);
	void maxReceiveSizeChanged(quint32 size);
	void historyEnabledChanged(bool enabled);
	void historySizeChanged(int size);
	void saveHistoryChanged(bool save);
	void syncEnabledChanged(bool enabled);
	void sendEnabledChanged(bool enabled);
	void recvEnabledChanged(bool enabled);
	void syncModeChanged(ClipboardManager::SynchronizeMode mode);
	void encryptionChanged(Communicator::Encryption encryption);
	void certificatePathChanged();
	void certificateChanged(const QSslCertificate &certificate);
	void privateKeyPathChanged();
	void privateKeyChanged(const QSslKey &key);
	void sendFilterModeChanged();
	void sendFiltersChanged();
	void receiveFilterModeChanged();
	void receiveFiltersChanged();
	void nodeUpdated(const Node &node);
	void nodeAdded(const Node &node);

private:
	static Settings *m_instance;
	QSettings *m_settings;

	// properties
    bool m_firstStart;
	QString m_host;
	quint16 m_port;
	bool m_allowAutoDiscovery;
	QString m_networkName;
	quint32 m_maxSendSize;
	quint32 m_maxRecvSize;
	Communicator::Encryption m_encryption;
	QString m_certificatePath;
	QSslCertificate m_certificate;
	QString m_privateKeyPath;
	QSslKey m_privateKey;
	bool m_historyEnabled;
	int m_historySize;
	bool m_saveHistory;
	bool m_syncEnabled;
	bool m_sendEnabled;
	bool m_recvEnabled;
	ClipboardManager::SynchronizeMode m_syncMode;
	QList<Node> m_nodes;
	unsigned int m_nextNodeId;
	MimeFilterMode m_sendFilterMode;
	QStringList m_sendFilters;
	MimeFilterMode m_recvFilterMode;
	QStringList m_recvFilters;

	Settings(QObject *parent = 0);
	void load();
	void loadNodes();
	void saveNodes();
	void migrate(int from, int to = CONFIG_VERSION);
	void upgrade(int from, int to);
	void downgrade(int from, int to);
	ConfigMigration* createMigration(int v);
	QString dataStoragePath();
	bool isFirstLaunch();
	void loadCertificate();
	void loadPrivateKey();

};

#endif // SETTINGS_H
