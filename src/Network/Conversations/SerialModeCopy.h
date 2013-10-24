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

#ifndef SERIALMODECOPY_H
#define SERIALMODECOPY_H

#include "SerialModeBase.h"
#include "../Commands/SerialModeInfo.h"

#ifdef INCLUDE_SERIAL_MODE

namespace Conversations {
	class SerialModeCopy : public SerialModeBase
	{
		Q_OBJECT
	public:
		SerialModeCopy(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);

	protected:
		virtual void nextCommandSender(BaseCommand::Type lastCmd, int index);
		virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);

	private:
		Commands::SerialModeInfo *m_cmdInfo;
	};
}

#endif // INCLUDE_SERIAL_MODE

#endif // SERIALMODECOPY_H
