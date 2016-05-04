#ifndef V3MIGRATION_H
#define V3MIGRATION_H

#include "../ConfigMigration.h"

namespace ConfigMigrations {
	/**
	 * Disables SSL in favor of TLS.
	 *
	 * Old settings:
	 *   None = 0
	 *   SSL  = 1
	 *   TLS  = 2
	 *
	 * New settings:
	 *   None = 0
	 *   TLS  = 1
	 */
	class V3Migration : public ConfigMigration
	{
		Q_OBJECT
	public:
		explicit V3Migration(QObject *parent = 0);

		// Encryption = 0/1 (None or TLS)
		void up();

		// Encryption = 0/2 (None or TLS)
		void down();
	};
}

#endif // V3MIGRATION_H
