/*
  HaveClip

  Copyright (C) 2013-2015 Jakub Skokan <aither@havefun.cz>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
