#include "SerialModeInfo.h"
#include "../../ClipboardSerialBatch.h"

using namespace Commands;

SerialModeInfo::SerialModeInfo(ClipboardContainer *cont, Communicator::Role r)
	: SerialModeBase(cont, r)
{
}

BaseCommand::Type SerialModeInfo::type() const
{
	return BaseCommand::SerialModeInfo;
}

void SerialModeInfo::receive(QDataStream &ds)
{
	ds >> m_batchId;
	ds >> m_itemCnt;
	ds >> m_currentIndex;

	finish();
}

void SerialModeInfo::send(QDataStream &ds)
{
	ClipboardSerialBatch *batch = static_cast<ClipboardSerialBatch*>(m_cont);

	ds << m_batchId;
	ds << (qint32) batch->count();
	ds << (qint32) batch->currentIndex();

	finish();
}

qint32 SerialModeInfo::itemCount()
{
	return m_itemCnt;
}

qint32 SerialModeInfo::currentIndex()
{
	return m_currentIndex;
}
