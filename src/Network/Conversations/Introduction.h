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

	protected:
		virtual void nextCommand(BaseCommand::Type lastCmd, int index);

	};
}

#endif // INTRODUCTION_H
