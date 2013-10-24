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

#include "SerialModeNext.h"
#include "../Commands/SerialModeInfo.h"
#include "SerialModeCopy.h"
#include "../../ClipboardSerialBatch.h"
#include "ClipboardUpdate.h"

using namespace Conversations;

SerialModeNext::SerialModeNext(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: SerialModeBase(id, r, cont, parent),
	  m_morph(false)
{
	addSerialCommand(BaseCommand::SerialModeInfo, r);
	addCommand(BaseCommand::Confirm, reverse(r));
}

Conversation::Type SerialModeNext::type() const
{
	return Conversation::SerialModeNext;
}

void SerialModeNext::nextCommandSender(BaseCommand::Type lastCmd, int index)
{
#ifdef INCLUDE_SERIAL_MODE
	switch(lastCmd)
	{
	case BaseCommand::Confirm: {
		qDebug() << "Sender::Confirmed";

		BaseCommand::Status s = m_cmds[index]->status();

		if(s == BaseCommand::NotExists || s == BaseCommand::NotMatches)
			morph(new Conversations::SerialModeCopy(m_batchId, m_role, m_cont));

		else if(s == BaseCommand::NotUnderstood)
			morph(new Conversations::ClipboardUpdate(m_role, m_cont->item()));

		break;
	}

	default:
		break;
	}
#endif
}

void SerialModeNext::nextCommandReceiver(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::SerialModeInfo: {
#ifdef INCLUDE_SERIAL_MODE
		Commands::SerialModeInfo *cmd = static_cast<Commands::SerialModeInfo*>(m_cmds[index]);

		ClipboardSerialBatch *batch;

		if((batch = m_history->searchBatchById(cmd->batchId())))
		{
			if(batch->count() == cmd->itemCount())
			{
				emit serialModeNext();

			} else {
				m_cmds[index+1]->setStatus(BaseCommand::NotMatches);
				m_morph = true;
			}

		} else {
			m_cmds[index+1]->setStatus(BaseCommand::NotExists);
			m_morph = true;
		}
#else
		m_cmds[index+1]->setStatus(BaseCommand::NotUnderstood);
		m_morph = true;
#endif

		break;
	}

	case BaseCommand::Confirm:
		if(m_morph)
		{
#ifdef INCLUDE_SERIAL_MODE
			morph(new Conversations::SerialModeCopy(m_batchId, m_role, m_cont));
#else
			morph(new Conversations::ClipboardUpdate(m_role, m_cont));
#endif
		}

		break;

#ifdef INCLUDE_SERIAL_MODE
	case BaseCommand::ClipboardUpdateSend:
		emit serialModeNext();

		break;
#endif

	default:
		break;
	}
}

