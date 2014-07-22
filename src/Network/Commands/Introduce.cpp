#include "Introduce.h"

using namespace Commands;

Introduce::Introduce(ClipboardContainer *cont, Communicator::Role r)
	: BaseCommand(cont, r)
{
}

BaseCommand::Type Introduce::type() const
{
	return BaseCommand::Introduce;
}

void Introduce::receive(QDataStream &ds)
{
	ds >> m_name;
	ds >> m_port;

	finish();
}

void Introduce::send(QDataStream &ds)
{
	ds << m_name;
	ds << m_port;

	finish();
}

QString Introduce::name()
{
	return m_name;
}

void Introduce::setName(QString name)
{
	m_name = name;
}

quint16 Introduce::port()
{
	return m_port;
}

void Introduce::setPort(quint16 port)
{
	m_port = port;
}
