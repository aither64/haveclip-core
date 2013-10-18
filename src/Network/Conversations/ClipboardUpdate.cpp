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

#include "ClipboardUpdate.h"

using namespace Conversations;

ClipboardUpdate::ClipboardUpdate(Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: Conversation(r, cont, parent)
{
	addCommand(BaseCommand::ClipboardUpdateReady, r);
	addCommand(BaseCommand::ClipboardUpdateConfirm, reverse(r));
}

Conversation::Type ClipboardUpdate::type() const
{
	return Conversation::ClipboardUpdate;
}

void ClipboardUpdate::nextCommand(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::ClipboardUpdateConfirm:
		// Remove commands if confirm is not ok
		if((m_role == Communicator::Send && m_cmds[index]->status() == BaseCommand::Ok)
			|| m_role == Communicator::Receive)
		{
			addCommand(BaseCommand::ClipboardUpdateSend, m_role);
			addCommand(BaseCommand::Confirm, reverse(m_role));
		}

		break;

	case BaseCommand::ClipboardUpdateSend:
		if(m_role == Communicator::Receive)
		{
			m_cont = m_cmds[index]->container();
			emit clipboardSync(m_cont);
		}

		break;

	case BaseCommand::Confirm:
		qDebug() << "Confirmed";

		break;

	default:
		break;
	}
}
