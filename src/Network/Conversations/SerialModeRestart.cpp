#include "SerialModeRestart.h"
#include "SerialModeCopy.h"
#include "../../ClipboardSerialBatch.h"

using namespace Conversations;

SerialModeRestart::SerialModeRestart(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: SerialModeBase(id, r, cont, parent),
	  m_morph(false)
{
	addSerialCommand(BaseCommand::SerialModeInfo, r);
	addCommand(BaseCommand::ClipboardUpdateConfirm, reverse(r));
}

Conversation::Type SerialModeRestart::type() const
{
	return Conversation::SerialModeRestart;
}

void SerialModeRestart::nextCommandSender(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::ClipboardUpdateConfirm: {
		BaseCommand::Status s = m_cmds[index]->status();

		qDebug() << "COmmand status" << s;

		if(s == BaseCommand::NotExists || s == BaseCommand::NotMatches)
			morph(new Conversations::SerialModeCopy(m_batchId, m_role, m_cont));

		break;
	}

	default:
		break;
	}
}

void SerialModeRestart::nextCommandReceiver(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::SerialModeInfo: {
		Commands::SerialModeInfo *cmd = static_cast<Commands::SerialModeInfo*>(m_cmds[index]);

		ClipboardSerialBatch *batch = 0;

		if( !(batch = m_history->searchBatchById(cmd->batchId())) )
		{
			qDebug() << "!!! Batch not found!";

			m_cmds[index+1]->setStatus(BaseCommand::NotExists);
			m_morph = true;

		} else if(batch->count() != cmd->itemCount()) {
			qDebug() << "!!! Batch not matches!";

			m_cmds[index+1]->setStatus(BaseCommand::NotMatches);
			m_morph = true;

		} else {
			emit serialModeRestart(batch);
		}

		break;
	}

	case BaseCommand::ClipboardUpdateConfirm:
		if(m_morph)
			morph(new Conversations::SerialModeCopy(m_batchId, m_role, m_cont));

		break;

	default:
		break;
	}
}
