#include "Verification.h"

#include "../Commands/SecurityCode.h"

using namespace Conversations;

Verification::Verification(Communicator::Role r, ClipboardContainer *cont, QObject *parent)
	: Conversation(r, cont, parent),
	  m_valid(ConnectionManager::Refused)
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

void Verification::setValid(ConnectionManager::CodeValidity v)
{
	m_valid = v;
}

void Verification::nextCommandReceiver(BaseCommand::Type lastCmd, int index)
{
	Q_UNUSED(lastCmd);

	if(index == 0)
	{
		emit verificationCodeReceived(this, static_cast<Commands::SecurityCode*>(m_cmds[index])->code() );

		m_cmds[1]->setStatus(codeValidityToCommandStatus(m_valid));
	}
}

void Verification::postDone()
{
	emit verificationFinished(commandStatusToCodeValidity(m_cmds[1]->status()));
}

BaseCommand::Status Verification::codeValidityToCommandStatus(ConnectionManager::CodeValidity v)
{
	switch(v)
	{
	case ConnectionManager::Valid:
		return BaseCommand::Ok;

	case ConnectionManager::NotValid:
		return BaseCommand::NotMatches;

	case ConnectionManager::Refused:
	default:
		return BaseCommand::Abort;
	}
}

ConnectionManager::CodeValidity Verification::commandStatusToCodeValidity(BaseCommand::Status s)
{
	switch(s)
	{
	case BaseCommand::Ok:
		return ConnectionManager::Valid;

	case BaseCommand::NotMatches:
		return ConnectionManager::NotValid;

	case BaseCommand::Abort:
	default:
		return ConnectionManager::Refused;
	}
}
