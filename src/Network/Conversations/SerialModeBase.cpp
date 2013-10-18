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

BaseCommand* SerialModeBase::addCommand(BaseCommand::Type t, Communicator::Role r)
{
	Commands::SerialModeBase *cmd = static_cast<Commands::SerialModeBase*>(Conversation::addCommand(t, r));
	cmd->setBatchId(m_batchId);

	return cmd;
}
