#ifndef VERIFICATION_H
#define VERIFICATION_H

#include "../Conversation.h"

namespace Conversations {
	class Verification : public Conversation
	{
		Q_OBJECT
	public:
		explicit Verification(Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);

		virtual Type type() const;
		virtual ConnectionManager::AuthMode authenticate();
		void setSecurityCode(QString code);
		void setValid(ConnectionManager::CodeValidity v);

	protected:
		virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);
		virtual void postDone();

	private:
		ConnectionManager::CodeValidity m_valid;

		BaseCommand::Status codeValidityToCommandStatus(ConnectionManager::CodeValidity v);
		ConnectionManager::CodeValidity commandStatusToCodeValidity(BaseCommand::Status s);
	};
}

#endif // VERIFICATION_H
