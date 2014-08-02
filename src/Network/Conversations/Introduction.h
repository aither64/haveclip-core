#ifndef INTRODUCTION_H
#define INTRODUCTION_H

#include "../Conversation.h"

namespace Conversations {
	class Introduction : public Conversation
	{
		Q_OBJECT
	public:
		Introduction(Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
		virtual Type type() const;
		virtual ConnectionManager::AuthMode authenticate();

		void setName(QString name);
		void setPort(quint16 port);

	protected:
		virtual void postDoneSender();
		virtual void postDoneReceiver();

	};
}

#endif // INTRODUCTION_H
