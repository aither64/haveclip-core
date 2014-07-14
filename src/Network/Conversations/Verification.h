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
		void setValid(bool v);

	protected:
		virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);
		virtual void postDoneSender();

	private:
		bool m_valid;
	};
}

#endif // VERIFICATION_H
