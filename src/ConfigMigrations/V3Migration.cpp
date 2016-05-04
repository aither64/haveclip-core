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

#include "V3Migration.h"

#include <QDesktopServices>
#include <QStringList>

using namespace ConfigMigrations;

V3Migration::V3Migration(QObject *parent) :
		ConfigMigration(parent)
{
}

void V3Migration::up()
{
	// SSL is removed, use TLS
	m_settings->beginGroup("Security");

	int enc = m_settings->value("Encryption", 1).toInt(); // 1 is old SSL, new TLS

	if (enc < 0 || enc > 1)
		m_settings->setValue("Encryption", 1); // 1 is the new TLS

	m_settings->endGroup();
}

void V3Migration::down()
{
	// SSL is removed, use TLS
	m_settings->beginGroup("Security");

	int enc = m_settings->value("Encryption", 1).toInt(); // 1 is TLS

	if (enc != 0)
		m_settings->setValue("Encryption", 2); // 2 is the old TLS

	m_settings->endGroup();
}
