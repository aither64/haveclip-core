#ifndef V2MIGRATION_H
#define V2MIGRATION_H

#include "../ConfigMigration.h"

class V2Migration : public ConfigMigration
{
	Q_OBJECT
public:
	explicit V2Migration(QObject *parent = 0);

signals:

public slots:

};

#endif // V2MIGRATION_H
