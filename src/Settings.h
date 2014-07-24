#ifndef SETTINGS_H
#define SETTINGS_H

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
	static Settings* create(QObject *parent = 0);
	static Settings* get();

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

	Q_PROPERTY(quint32 maxSendSize READ maxSendSize WRITE setMaxSendSize)
	quint32 maxSendSize() const;
	void setMaxSendSize(quint32 size);

	Q_PROPERTY(quint32 maxReceiveSize READ maxReceiveSize WRITE setMaxReceiveSize)
	quint32 maxReceiveSize() const;
	void setMaxReceiveSize(quint32 size);

	Q_PROPERTY(bool historyEnabled READ isHistoryEnabled WRITE setHistoryEnabled NOTIFY historyEnabledChanged)
	bool isHistoryEnabled() const;
	void setHistoryEnabled(bool enabled);

	Q_PROPERTY(int historySize READ historySize WRITE setHistorySize)
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

	Q_PROPERTY(QString certificatePath READ certificatePath WRITE setCertificatePath)
	QString certificatePath();
	void setCertificatePath(QString cert);

	Q_PROPERTY(QSslCertificate certificate READ certificate)
	QSslCertificate& certificate();

	Q_PROPERTY(QString privateKeyPath READ privateKeyPath WRITE setPrivateKeyPath)
	QString privateKeyPath();
	void setPrivateKeyPath(QString key);

	Q_PROPERTY(QSslKey privateKey READ privateKey)
	QSslKey& privateKey();

	QList<Node> nodes();
	void setNodes(QList<Node> &nodes);
	void addOrUpdateNode(Node &n);

	unsigned int nextNodeId();

	Q_INVOKABLE void save();
	Q_INVOKABLE void reset();

signals:
	void hostChanged(QString host);
	void portChanged(quint16 port);
	void hostAndPortChanged(QString host, quint16 port);
	void allowAutoDiscoveryChanged(bool allow);
	void networkNameChanged(QString name);
	void historyEnabledChanged(bool enabled);
	void saveHistoryChanged(bool save);
	void syncEnabledChanged(bool enabled);
	void sendEnabledChanged(bool enabled);
	void recvEnabledChanged(bool enabled);
	void syncModeChanged(ClipboardManager::SynchronizeMode mode);
	void encryptionChanged(Communicator::Encryption encryption);
	void nodeUpdated(const Node &node);
	void nodeAdded(const Node &node);

private:
	static Settings *m_instance;
	QSettings *m_settings;

	// properties
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

	Settings(QObject *parent = 0);
	~Settings();
	void load();
	void loadNodes();
	void saveNodes();
	void migrate(int from, int to = CONFIG_VERSION);
	void upgrade(int from, int to);
	void downgrade(int from, int to);
	ConfigMigration* createMigration(int v);
	QString dataStoragePath();
	bool isFirstLaunch();

};

#endif // SETTINGS_H
