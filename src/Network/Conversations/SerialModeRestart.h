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

#ifndef SERIALMODERESTART_H
#define SERIALMODERESTART_H

#include "SerialModeBase.h"
#include "HistoryMixin.h"

namespace Conversations {
	class SerialModeRestart : public SerialModeBase, public HistoryMixin
	{
		Q_OBJECT
	public:
		SerialModeRestart(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
		virtual Type type() const;

	protected:
		virtual void nextCommandSender(BaseCommand::Type lastCmd, int index);
		virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);

	private:
		bool m_morph;
	};
}

#endif // SERIALMODERESTART_H
