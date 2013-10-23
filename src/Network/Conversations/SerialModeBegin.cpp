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
