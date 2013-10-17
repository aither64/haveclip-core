#include "NetworkCommand.h"

NetworkCommand::NetworkCommand(ClipboardContainer *cont, Communicator::Role r)
	: m_cont(cont),
	  m_role(r),
	  m_finished(false),
	  m_status(Undefined)
{
}

Communicator::Role NetworkCommand::role() const
{
	return m_role;
}

bool NetworkCommand::isFinished() const
{
	return m_finished;
}

NetworkCommand::Status NetworkCommand::status() const
{
	return m_status;
}

ClipboardContainer* NetworkCommand::container()
{
	return m_cont;
}

void NetworkCommand::readStatus(QDataStream &ds)
{
	qint32 s;
	ds >> s;

	m_status = (NetworkCommand::Status) s;
}

void NetworkCommand::writeStatus(QDataStream &ds, Status s)
{
	ds << (qint32) (s == Undefined ? m_status : s);
}

void NetworkCommand::setStatus(Status s)
{
	m_status = s;
}

void NetworkCommand::finish()
{
	m_finished = true;
}
