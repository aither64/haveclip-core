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

#include "Conversation.h"

#include "Commands/Ping.h"
#include "Commands/Introduce.h"
#include "Commands/SecurityCode.h"
#include "Commands/ClipboardUpdateReady.h"
#include "Commands/ClipboardUpdateConfirm.h"
#include "Commands/ClipboardUpdateSend.h"
#include "Commands/Confirm.h"

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

BaseCommand::Type Conversation::currentCommandType() const
{
	return m_cmds[m_currentCmd]->type();
}

bool Conversation::isDone() const
{
	return m_done;
}

void Conversation::receive(QDataStream &ds)
{
//	qDebug() << "Conversation::receive" << m_currentCmd;

	m_cmds[m_currentCmd]->receive(ds);

	moveToNextCommand();
}

void Conversation::send(QDataStream &ds)
{
//	qDebug() << "Conversation::send" << m_currentCmd;

	m_cmds[m_currentCmd]->send(ds);

	moveToNextCommand();
}

void Conversation::postDone()
{
	if(m_role == Communicator::Send)
		postDoneSender();

	else
		postDoneReceiver();
}

ConnectionManager::AuthMode Conversation::authenticate()
{
	return ConnectionManager::Verified;
}

BaseCommand* Conversation::addCommand(BaseCommand::Type t, Communicator::Role r)
{
	BaseCommand *cmd;

	using namespace Commands;

	switch(t)
	{
	case BaseCommand::Ping:
		cmd = new Ping(m_cont, r);
		break;

	case BaseCommand::Introduce:
		cmd = new Introduce(m_cont, r);
		break;

	case BaseCommand::SecurityCode:
		cmd = new SecurityCode(m_cont, r);
		break;

	case BaseCommand::ClipboardUpdateReady:
		cmd = new ClipboardUpdateReady(m_cont, r);
		break;

	case BaseCommand::ClipboardUpdateConfirm:
		cmd = new ClipboardUpdateConfirm(m_cont, r);
		break;

	case BaseCommand::ClipboardUpdateSend:
		cmd = new ClipboardUpdateSend(m_cont, r);
		break;

	case BaseCommand::Confirm:
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

	if(m_role == Communicator::Send)
		nextCommandSender(m_cmds[m_currentCmd]->type(), m_currentCmd);
	else
		nextCommandReceiver(m_cmds[m_currentCmd]->type(), m_currentCmd);

	if(m_cmds.size() > (m_currentCmd+1))
		m_currentCmd++;
	else {
		m_done = true;
		emit done();
	}
}

void Conversation::nextCommand(BaseCommand::Type lastCmd, int index)
{
	Q_UNUSED(lastCmd);
	Q_UNUSED(index);
}

void Conversation::nextCommandSender(BaseCommand::Type lastCmd, int index)
{
	Q_UNUSED(lastCmd);
	Q_UNUSED(index);
}

void Conversation::nextCommandReceiver(BaseCommand::Type lastCmd, int index)
{
	Q_UNUSED(lastCmd);
	Q_UNUSED(index);
}

void Conversation::postDoneSender()
{

}

void Conversation::postDoneReceiver()
{

}

void Conversation::confirm(BaseCommand::Status s)
{
	addCommand(BaseCommand::Confirm, Communicator::Send)->setStatus(s);
}

void Conversation::morph(Conversation *c)
{
	qDebug() << "Morphing conversation" << type() << "into" << c->type();

	m_done = true;

	emit morphed(c);
}
