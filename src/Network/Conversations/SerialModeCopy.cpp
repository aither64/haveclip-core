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

#include "SerialModeCopy.h"

#ifdef INCLUDE_SERIAL_MODE

#include "../Commands/SerialModeInfo.h"
#include "../../ClipboardSerialBatch.h"

using namespace Conversations;

SerialModeCopy::SerialModeCopy(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: SerialModeBase(id, r, cont, parent)
{
	m_cmdInfo = static_cast<Commands::SerialModeInfo*>(addSerialCommand(BaseCommand::SerialModeInfo, r));
	addCommand(BaseCommand::ClipboardUpdateSend, r);
	addCommand(BaseCommand::Confirm, reverse(r));
}

Conversation::Type SerialModeCopy::type() const
{
	return Conversation::SerialModeCopy;
}

void SerialModeCopy::receive(QDataStream &ds)
{
	qDebug() << "Conversation::receive" << m_currentCmd;

	if(m_currentCmd == 1)
	{
		ClipboardSerialBatch *batch = new ClipboardSerialBatch(m_cmdInfo->batchId());
		int cnt = m_cmdInfo->itemCount();

		for(int i = 0; i < cnt; i++)
		{
			m_cmds[m_currentCmd]->receive(ds);
			batch->addItem( static_cast<ClipboardItem*>(m_cmds[1]->container()), true );
		}

		emit serialModeNewBatch(batch);

	} else
		m_cmds[m_currentCmd]->receive(ds);

	moveToNextCommand();
}

void SerialModeCopy::send(QDataStream &ds)
{
	qDebug() << "Conversation::send" << m_currentCmd;

	if(m_currentCmd == 1)
	{
		QList<ClipboardItem*> items = m_cont->items();

		foreach(ClipboardItem *it, items)
		{
			m_cmds[m_currentCmd]->setContainer(it);
			m_cmds[m_currentCmd]->send(ds);
		}

	} else
		m_cmds[m_currentCmd]->send(ds);

	moveToNextCommand();
}

void SerialModeCopy::nextCommandSender(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::Confirm:
		qDebug() << "Sender::Confirmed";

		break;

	default:
		break;
	}
}

void SerialModeCopy::nextCommandReceiver(BaseCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case BaseCommand::Confirm:
		qDebug() << "Receiver::Confirmed";

		break;

	default:
		break;
	}
}

#endif // INCLUDE_SERIAL_MODE
