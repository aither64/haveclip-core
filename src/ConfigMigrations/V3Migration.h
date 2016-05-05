/*
  HaveClip

  Copyright (C) 2013-2016 Jakub Skokan <aither@havefun.cz>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
