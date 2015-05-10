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

#ifndef INTRODUCE_H
#define INTRODUCE_H

#include "../Command.h"

namespace Commands {
	class Introduce : public BaseCommand
	{
	public:
		Introduce(ClipboardContainer *cont, Communicator::Role r);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);
		QString name();
		void setName(QString name);
		quint16 port();
		void setPort(quint16 port);

	private:
		QString m_name;
		quint16 m_port;

	};
}

#endif // INTRODUCE_H
