#include "ConfigMigration.h"

ConfigMigration::ConfigMigration(QObject *parent) :
        QObject(parent)
{
}

void ConfigMigration::setSettings(QSettings *s)
{
	m_settings = s;
}

void ConfigMigration::up()
{

}

void ConfigMigration::down()
{

}
