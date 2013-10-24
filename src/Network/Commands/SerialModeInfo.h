/*
  HaveClip

  Copyright (C) 2013 Jakub Skokan <aither@havefun.cz>

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

#ifndef SERIALMODEINFO_H
#define SERIALMODEINFO_H

#include "Cmd_SerialModeBase.h"

namespace Commands {
	class SerialModeInfo : public SerialModeBase
	{
	public:
		SerialModeInfo(ClipboardContainer *cont, Communicator::Role r);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);
		qint32 itemCount();
		qint32 currentIndex();

	private:
		qint32 m_itemCnt;
		qint32 m_currentIndex;
	};
}

#endif // SERIALMODEINFO_H
