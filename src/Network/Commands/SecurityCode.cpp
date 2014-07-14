#include "SecurityCode.h"

using namespace Commands;

SecurityCode::SecurityCode(ClipboardContainer *cont, Communicator::Role r)
	: BaseCommand(cont, r)
{
}


BaseCommand::Type SecurityCode::type() const
{
	return BaseCommand::SecurityCode;
}

void SecurityCode::receive(QDataStream &ds)
{
	ds >> m_code;

	finish();
}

void SecurityCode::send(QDataStream &ds)
{
	ds << m_code;

	finish();
}

void SecurityCode::setCode(QString code)
{
	m_code = code;
}

QString SecurityCode::code()
{
	return m_code;
}

