#ifndef CONFIGMIGRATION_H
#define CONFIGMIGRATION_H

#include <QObject>
#include <QSettings>

class ConfigMigration : public QObject
{
	Q_OBJECT
public:
	explicit ConfigMigration(QObject *parent = 0);
	void setSettings(QSettings *s);
	virtual void up();
	virtual void down();

private:
	QSettings *m_settings;

};

#endif // CONFIGMIGRATION_H
