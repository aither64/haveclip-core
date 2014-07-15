#include "Verification.h"

#include "../Commands/SecurityCode.h"

using namespace Conversations;

Verification::Verification(Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: Conversation(r, cont, parent),
	  m_valid(false)
{
	addCommand(BaseCommand::SecurityCode, r);
	addCommand(BaseCommand::Confirm, reverse(r));
}

Conversation::Type Verification::type() const
{
	return Conversation::Verification;
}

ConnectionManager::AuthMode Verification::authenticate()
{
	return ConnectionManager::Introduced;
}

void Verification::setSecurityCode(QString code)
{
	static_cast<Commands::SecurityCode*>(m_cmds[0])->setCode(code);
}

void Verification::setValid(bool v)
{
	m_valid = v;
}

void Verification::nextCommandReceiver(BaseCommand::Type lastCmd, int index)
{
	Q_UNUSED(lastCmd);

	if(index == 0)
	{
		emit verificationCodeReceived(this, static_cast<Commands::SecurityCode*>(m_cmds[index])->code() );
		m_cmds[1]->setStatus(m_valid ? BaseCommand::Ok : BaseCommand::NotMatches);
	}
}

void Verification::postDoneSender()
{
	emit verificationFinished(m_cmds[1]->status() == BaseCommand::Ok);
}
