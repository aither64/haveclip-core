#include "ClipboardUpdate.h"

ClipboardUpdate::ClipboardUpdate(Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: Conversation(r, cont, parent)
{
	addCommand(NetworkCommand::ClipboardUpdateReady, r);
	addCommand(NetworkCommand::ClipboardUpdateConfirm, reverse(r));
}

Conversation::Type ClipboardUpdate::type() const
{
	return Conversation::ClipboardUpdate;
}

void ClipboardUpdate::nextCommand(NetworkCommand::Type lastCmd, int index)
{
	switch(lastCmd)
	{
	case NetworkCommand::ClipboardUpdateConfirm:
		// Remove commands if confirm is not ok
		if((m_role == Communicator::Send && m_cmds[index]->status() == NetworkCommand::Ok)
			|| m_role == Communicator::Receive)
		{
			addCommand(NetworkCommand::ClipboardUpdateSend, m_role);
			addCommand(NetworkCommand::Confirm, reverse(m_role));
		}

		break;

	case NetworkCommand::ClipboardUpdateSend:
		if(m_role == Communicator::Receive)
		{
			m_cont = m_cmds[index]->container();
			emit clipboardSync(m_cont);
		}

		break;

	case NetworkCommand::Confirm:
		qDebug() << "Confirmed";

		break;

	default:
		break;
	}
}
