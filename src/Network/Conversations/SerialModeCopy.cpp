#include "SerialModeCopy.h"
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
