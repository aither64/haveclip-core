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

#include "SerialModeBegin.h"
#include "../Commands/Cmd_SerialModeBase.h"

using namespace Conversations;

SerialModeBegin::SerialModeBegin(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: SerialModeBase(id, r, cont, parent)
{
	addSerialCommand(BaseCommand::SerialModeToggle, r);
	addCommand(BaseCommand::Confirm, reverse(r));
}

Conversation::Type SerialModeBegin::type() const
{
	return Conversation::SerialModeBegin;
}

void SerialModeBegin::nextCommandSender(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::Confirm:
		qDebug() << "Sender::Confirmed";

		break;

	default:
		break;
	}
}

void SerialModeBegin::nextCommandReceiver(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::SerialModeToggle:
		// Check serial mode availability
		m_cmds[index+1]->setStatus(BaseCommand::Ok);

		emit serialModeToggled(true, static_cast<Commands::SerialModeBase*>(m_cmds[index])->batchId());

		break;

	case BaseCommand::Confirm:
		qDebug() << "Receiver::Confirmed";

		break;

	default:
		break;
	}
}
