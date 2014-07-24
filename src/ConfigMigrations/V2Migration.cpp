#include "V2Migration.h"

#include <QDesktopServices>
#include <QStringList>

using namespace ConfigMigrations;

V2Migration::V2Migration(QObject *parent) :
        ConfigMigration(parent)
{
}

void V2Migration::up()
{
	// Migrate nodes
	QStringList oldNodes = m_settings->value("Pool/Nodes").toStringList();

	m_settings->remove("Pool/Nodes");

	int i = 0;

	foreach(const QString &addr, oldNodes)
	{
		QString cert = m_settings->value("Node:" + addr + "/Certificate").toString();

		m_settings->beginGroup("Pool/Nodes");
		m_settings->beginGroup(QString::number(i));

		m_settings->setValue("Name", addr);
		m_settings->setValue("Host", addr.section(':', 0, 0));
		m_settings->setValue("Port", addr.section(':', 1, 1).toUShort());
		m_settings->setValue("Certificate", cert);

		m_settings->endGroup();
		m_settings->endGroup();

		i++;
	}

	foreach(const QString &sec, m_settings->childGroups())
	{
		if(sec.startsWith("Node:"))
			m_settings->remove(sec);
	}

	// Change old and invalid certificate setting
	if(m_settings->value("Connection/Certificate").toString() == "certs/haveclip.crt")
		m_settings->setValue("Network/Certificate", storagePath() + "/haveclip.crt");

	if(m_settings->value("Connection/PrivateKey").toString() == "certs/haveclip.key")
		m_settings->setValue("Network/PrivateKey", storagePath() + "/haveclip.key");

	// Split Connection to Network and Security
	m_settings->setValue("Network/Host", m_settings->value("Connection/Host"));
	m_settings->setValue("Network/Port", m_settings->value("Connection/Port"));

	m_settings->setValue("Security/Encryption", m_settings->value("Connection/Encryption"));
	m_settings->setValue("Security/Certificate", m_settings->value("Connection/Certificate"));
	m_settings->setValue("Security/PrivateKey", m_settings->value("Connection/PrivateKey"));

	m_settings->remove("Connection");

	// Remove deprecated settings
	m_settings->remove("PasteServices");
	m_settings->remove("AccessPolicy");
	m_settings->remove("Selection");
}

QString V2Migration::storagePath()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
	return QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
}
