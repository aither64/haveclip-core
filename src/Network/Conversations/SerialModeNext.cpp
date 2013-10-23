#include "SerialModeNext.h"
#include "../Commands/SerialModeInfo.h"
#include "SerialModeCopy.h"
#include "../../ClipboardSerialBatch.h"

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
	switch(lastCmd)
	{
	case BaseCommand::Confirm: {
		qDebug() << "Sender::Confirmed";

		BaseCommand::Status s = m_cmds[index]->status();

		if(s == BaseCommand::NotExists || s == BaseCommand::NotMatches)
			morph(new Conversations::SerialModeCopy(m_batchId, m_role, m_cont));

		break;
	}

	default:
		break;
	}
}

void SerialModeNext::nextCommandReceiver(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::SerialModeInfo: {
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


		break;
	}

	case BaseCommand::Confirm:
		if(m_morph)
			morph(new Conversations::SerialModeCopy(m_batchId, m_role, m_cont));

		break;

	case BaseCommand::ClipboardUpdateSend:
		emit serialModeNext();

		break;

	default:
		break;
	}
}

