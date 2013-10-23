#include "SerialModeAppend.h"
#include "SerialModeCopy.h"
#include "../../ClipboardSerialBatch.h"
#include "../Commands/SerialModeAppendReady.h"

using namespace Conversations;

SerialModeAppend::SerialModeAppend(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: SerialModeBase(id, r, cont, parent),
	  m_morph(false)
{
	addSerialCommand(BaseCommand::SerialModeAppendReady, r);
	addCommand(BaseCommand::ClipboardUpdateConfirm, reverse(r));
}

Conversation::Type SerialModeAppend::type() const
{
	return Conversation::SerialModeAppend;
}

void SerialModeAppend::nextCommandSender(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::ClipboardUpdateConfirm: {
		BaseCommand::Status s = m_cmds[index]->status();

		if(s == BaseCommand::Ok)
		{
			addCommand(BaseCommand::ClipboardUpdateSend, m_role)->setContainer(m_cont->items().last());
			addCommand(BaseCommand::Confirm, reverse(m_role));

		} else if(s == BaseCommand::NotExists || s == BaseCommand::NotMatches) {
			morph(new Conversations::SerialModeCopy(m_batchId, m_role, m_cont));
		}

		break;
	}

	case BaseCommand::Confirm:
		qDebug() << "Sender::Confirmed";

		break;

	default:
		break;
	}
}

void SerialModeAppend::nextCommandReceiver(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::SerialModeAppendReady: {
		Commands::SerialModeAppendReady *cmd = static_cast<Commands::SerialModeAppendReady*>(m_cmds[index]);

		ClipboardSerialBatch *batch = 0;

		if(m_history->preparedSerialbatchId() == cmd->batchId() || (batch = m_history->searchBatchById(cmd->batchId())))
		{
			if(!batch || batch->count()+1 == cmd->itemCount())
			{
				addCommand(BaseCommand::ClipboardUpdateSend, m_role);
				addCommand(BaseCommand::Confirm, reverse(m_role));

			} else {
				qDebug() << "!!! Item count does not match";

				m_cmds[index+1]->setStatus(BaseCommand::NotMatches);
				m_morph = true;
			}

		} else {
			qDebug() << "!!! Batch not found!!";

			m_cmds[index+1]->setStatus(BaseCommand::NotExists);
			m_morph = true;
		}

		break;
	}

	case BaseCommand::ClipboardUpdateConfirm:
		if(m_morph)
			morph(new Conversations::SerialModeCopy(m_batchId, m_role, m_cont));

		break;

	case BaseCommand::ClipboardUpdateSend:
		emit serialModeAppend(static_cast<ClipboardItem*>(m_cmds[2]->container()));
		break;

	case BaseCommand::Confirm:
		qDebug() << "Receiver::Confirmed";

		break;

	default:
		break;
	}
}
