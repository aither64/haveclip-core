#include "SerialModeAppendReady.h"
#include "../../ClipboardSerialBatch.h"

using namespace Commands;

SerialModeAppendReady::SerialModeAppendReady(ClipboardContainer *cont, Communicator::Role r)
	: SerialModeBase(cont, r)
{
}

BaseCommand::Type SerialModeAppendReady::type() const
{
	return BaseCommand::SerialModeAppendReady;
}

void SerialModeAppendReady::receive(QDataStream &ds)
{
	ds >> m_batchId;
	ds >> m_itemCnt;

	finish();
}

void SerialModeAppendReady::send(QDataStream &ds)
{
	ds << m_batchId;
	ds << static_cast<ClipboardSerialBatch*>(m_cont)->items().count();

	finish();
}

int SerialModeAppendReady::itemCount()
{
	return m_itemCnt;
}
