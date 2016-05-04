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

#ifndef SYNC_H
#define SYNC_H

#include "RemoteBase.h"

namespace RemoteControls {
	class Sync : public RemoteBase
	{
		Q_OBJECT

	public:
		explicit Sync(QObject *parent = 0);
		~Sync();
		virtual RemoteControl::Command type();
		virtual bool sendCommand(QDataStream &ds);
		virtual bool receiveCommand(QDataStream &ds);
	};
}

#endif // SYNC_H
