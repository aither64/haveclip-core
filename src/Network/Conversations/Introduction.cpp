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

#include "Introduction.h"

#include "../Commands/Introduce.h"

using namespace Conversations;

Introduction::Introduction(Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: Conversation(r, cont, parent)
{
	addCommand(BaseCommand::Introduce, r);
	addCommand(BaseCommand::Introduce, reverse(r));
}

Conversation::Type Introduction::type() const
{
	return Conversation::Introduction;
}

void Introduction::setName(QString name)
{
	static_cast<Commands::Introduce*>(m_cmds[m_role == Communicator::Send ? 0 : 1])->setName(name);
}

void Introduction::setPort(quint16 port)
{
	static_cast<Commands::Introduce*>(m_cmds[m_role == Communicator::Send ? 0 : 1])->setPort(port);
}

ConnectionManager::AuthMode Introduction::authenticate()
{
	return ConnectionManager::NoAuth;
}

void Introduction::postDoneSender()
{
	emit introductionFinished( static_cast<Commands::Introduce*>(m_cmds[1])->name() );
}

void Introduction::postDoneReceiver()
{
	Commands::Introduce *cmd = static_cast<Commands::Introduce*>(m_cmds[0]);

	// show dialog to input security code
	emit verificationRequested(cmd->name(), cmd->port());
}
