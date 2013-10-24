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

#include "SerialModeBase.h"
#include "../Commands/Cmd_SerialModeBase.h"

using namespace Conversations;

SerialModeBase::SerialModeBase(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent) :
	Conversation(r, cont, parent),
	m_batchId(id)
{
}

void SerialModeBase::setBatchId(qint64 id)
{
	m_batchId = id;
}

BaseCommand* SerialModeBase::addSerialCommand(BaseCommand::Type t, Communicator::Role r)
{
	Commands::SerialModeBase *cmd = static_cast<Commands::SerialModeBase*>(addCommand(t, r));
	cmd->setBatchId(m_batchId);

	return cmd;
}
