/*
  HaveClip

  Copyright (C) 2013-2015 Jakub Skokan <aither@havefun.cz>

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

#include "Sync.h"

using namespace RemoteControls;

Sync::Sync(QObject *parent)
	: RemoteBase(parent)
{

}

Sync::~Sync()
{

}

RemoteControl::Command Sync::type()
{
	return RemoteControl::Sync;
}

bool Sync::sendCommand(QDataStream &ds)
{
	Q_UNUSED(ds);

	return true;
}

bool Sync::receiveCommand(QDataStream &ds)
{
	Q_UNUSED(ds);

	qDebug() << "RemoteControl: synchronize clipboard";

	m_manager->distributeCurrentClipboard();

	return true;
}

