#ifndef V2MIGRATION_H
#define V2MIGRATION_H

#include "../ConfigMigration.h"

namespace ConfigMigrations {
	class V2Migration : public ConfigMigration
	{
		Q_OBJECT
	public:
		explicit V2Migration(QObject *parent = 0);
		void up();

	private:
		QString storagePath();

	};
}

#endif // V2MIGRATION_H
