#include "Cmd_SerialModeEnd.h"

using namespace Commands;

SerialModeEnd::SerialModeEnd(ClipboardContainer *cont, Communicator::Role r)
	: SerialModeBase(cont, r)
{
}

BaseCommand::Type SerialModeEnd::type() const
{
	return BaseCommand::SerialModeEnd;
}

void SerialModeEnd::receive(QDataStream &ds)
{
	ds >> m_batchId;

	finish();
}

void SerialModeEnd::send(QDataStream &ds)
{
	ds << m_batchId;

	finish();
}
