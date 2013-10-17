#include "Conversation.h"

#include "NetworkCommands/ClipboardUpdateReady.h"
#include "NetworkCommands/ClipboardUpdateConfirm.h"
#include "NetworkCommands/ClipboardUpdateSend.h"
#include "NetworkCommands/Confirm.h"

Conversation::Conversation(Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: QObject(parent),
	  m_role(r),
	  m_cont(cont),
	  m_currentCmd(0),
	  m_done(false)
{
}

Conversation::~Conversation()
{
	qDeleteAll(m_cmds);
}

Communicator::Role Conversation::currentRole() const
{
	return m_cmds[m_currentCmd]->role();
}

NetworkCommand::Type Conversation::currentCommandType() const
{
	return m_cmds[m_currentCmd]->type();
}

bool Conversation::isDone() const
{
	return m_done;
}

void Conversation::receive(QDataStream &ds)
{
	qDebug() << "Conversation::receive" << m_currentCmd;

	m_cmds[m_currentCmd]->receive(ds);

	moveToNextCommand();
}

void Conversation::send(QDataStream &ds)
{
	qDebug() << "Conversation::send" << m_currentCmd;

	m_cmds[m_currentCmd]->send(ds);

	moveToNextCommand();
}

NetworkCommand* Conversation::addCommand(NetworkCommand::Type t, Communicator::Role r)
{
	NetworkCommand *cmd;

	switch(t)
	{
	case NetworkCommand::ClipboardUpdateReady:
		cmd = new ClipboardUpdateReady(m_cont, r);
		break;

	case NetworkCommand::ClipboardUpdateConfirm:
		cmd = new ClipboardUpdateConfirm(m_cont, r);
		break;

	case NetworkCommand::ClipboardUpdateSend:
		cmd = new ClipboardUpdateSend(m_cont, r);
		break;

	case NetworkCommand::Confirm:
		cmd = new Confirm(m_cont, r);
		break;
	}

	m_cmds << cmd;

	return cmd;
}

Communicator::Role Conversation::reverse(Communicator::Role r) const
{
	return (r == Communicator::Send) ? Communicator::Receive : Communicator::Send;
}

void Conversation::moveToNextCommand()
{
	if(!m_cmds[m_currentCmd]->isFinished())
		return;

	nextCommand(m_cmds[m_currentCmd]->type(), m_currentCmd);

	if(m_cmds.size() > (m_currentCmd+1))
		m_currentCmd++;
	else {
		m_done = true;
		emit done();
	}
}

void Conversation::nextCommand(NetworkCommand::Type lastCmd, int index)
{

}
