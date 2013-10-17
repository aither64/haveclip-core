/*
  HaveClip

  Copyright (C) 2013 Jakub Skokan <aither@havefun.cz>

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

#include "Command.h"

BaseCommand::BaseCommand(ClipboardContainer *cont, Communicator::Role r)
	: m_cont(cont),
	  m_role(r),
	  m_finished(false),
	  m_status(Undefined)
{
}

Communicator::Role BaseCommand::role() const
{
	return m_role;
}

bool BaseCommand::isFinished() const
{
	return m_finished;
}

BaseCommand::Status BaseCommand::status() const
{
	return m_status;
}

ClipboardContainer* BaseCommand::container()
{
	return m_cont;
}

void BaseCommand::readStatus(QDataStream &ds)
{
	qint32 s;
	ds >> s;

	m_status = (BaseCommand::Status) s;
}

void BaseCommand::writeStatus(QDataStream &ds, Status s)
{
	ds << (qint32) (s == Undefined ? m_status : s);
}

void BaseCommand::setStatus(Status s)
{
	m_status = s;
}

void BaseCommand::finish()
{
	m_finished = true;
}
