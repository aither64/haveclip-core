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
