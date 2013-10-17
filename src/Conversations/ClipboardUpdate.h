#ifndef CLIPBOARDUPDATE_H
#define CLIPBOARDUPDATE_H

#include "../Conversation.h"

class ClipboardUpdate : public Conversation
{
public:
	ClipboardUpdate(Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
	virtual Type type() const;

protected:
	virtual void nextCommand(NetworkCommand::Type lastCmd, int index);
};

#endif // CLIPBOARDUPDATE_H
