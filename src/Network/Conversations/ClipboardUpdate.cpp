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

#include "ClipboardUpdate.h"
#include "../Commands/ClipboardUpdateReady.h"
#include "../Commands/ClipboardUpdateConfirm.h"
#include "../Commands/ClipboardUpdateSend.h"
#include "../../ClipboardManager.h"

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

void ClipboardUpdate::setFilters(Settings::MimeFilterMode mode, const QStringList &filters)
{
	m_filterMode = mode;
	m_filters = filters;

	if(m_role == Communicator::Receive)
		static_cast<Commands::ClipboardUpdateConfirm*>(m_cmds[1])->setFilters(mode, filters);
}

void ClipboardUpdate::nextCommand(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::ClipboardUpdateReady: {
		if(m_role == Communicator::Receive)
		{
			Commands::ClipboardUpdateReady *cmd = static_cast<Commands::ClipboardUpdateReady*>(m_cmds[index]);
			qint32 supported = ClipboardManager::supportedModes();
			ClipboardContainer::Mode mode = cmd->mode();

			if(mode == ClipboardContainer::ClipboardAndSelection
					&& ((supported & ClipboardContainer::Clipboard) == ClipboardContainer::Clipboard
						|| (supported & ClipboardContainer::Selection) == ClipboardContainer::Selection)
			)
			; // pass
			else if((supported & mode) != mode) {
				qDebug() << "Mode" << mode << "not supported, ignored";
				m_cmds[index+1]->setStatus(BaseCommand::Abort);
			}
		}

		break;
	}

	case BaseCommand::ClipboardUpdateConfirm:
		// Remove commands if confirm is not ok
		if((m_role == Communicator::Send && m_cmds[index]->status() == BaseCommand::Ok)
			|| m_role == Communicator::Receive)
		{
			addCommand(BaseCommand::ClipboardUpdateSend, m_role);
			addCommand(BaseCommand::Confirm, reverse(m_role));

			Commands::ClipboardUpdateConfirm *confirm = static_cast<Commands::ClipboardUpdateConfirm*>(m_cmds[1]);
			Commands::ClipboardUpdateSend *send = static_cast<Commands::ClipboardUpdateSend*>(m_cmds[2]);

			// Sender sends intersection of his send filters and receiver's accept filters
			// Receiver receives only his receive filters

			if(m_role == Communicator::Send)
			{
				send->setSendFilters(m_filterMode, m_filters);
				send->setRecvFilters(confirm->filterMode(), confirm->filters());

			} else { // Receive
				send->setRecvFilters(m_filterMode, m_filters);
			}
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
