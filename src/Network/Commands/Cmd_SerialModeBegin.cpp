#include "Cmd_SerialModeBegin.h"

using namespace Commands;

SerialModeBegin::SerialModeBegin(ClipboardContainer *cont, Communicator::Role r)
	: SerialModeBase(cont, r)
{
}

BaseCommand::Type SerialModeBegin::type() const
{
	return BaseCommand::SerialModeBegin;
}

void SerialModeBegin::receive(QDataStream &ds)
{
	ds >> m_batchId;

	finish();
}

void SerialModeBegin::send(QDataStream &ds)
{
	ds << m_batchId;

	finish();
}
