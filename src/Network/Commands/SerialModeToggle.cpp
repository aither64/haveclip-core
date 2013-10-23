#include "SerialModeToggle.h"

using namespace Commands;

SerialModeToggle::SerialModeToggle(ClipboardContainer *cont, Communicator::Role r)
	: SerialModeBase(cont, r)
{
}

BaseCommand::Type SerialModeToggle::type() const
{
	return BaseCommand::SerialModeToggle;
}

void SerialModeToggle::receive(QDataStream &ds)
{
	ds >> m_batchId;

	finish();
}

void SerialModeToggle::send(QDataStream &ds)
{
	ds << m_batchId;

	finish();
}
