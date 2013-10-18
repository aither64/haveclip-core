#include "Cmd_SerialModeBase.h"

using namespace Commands;

SerialModeBase::SerialModeBase(ClipboardContainer *cont, Communicator::Role r)
	: BaseCommand(cont, r),
	  m_batchId(0)
{
}

void SerialModeBase::setBatchId(qint64 id)
{
	m_batchId = id;
}

qint64 SerialModeBase::batchId() const
{
	return m_batchId;
}
