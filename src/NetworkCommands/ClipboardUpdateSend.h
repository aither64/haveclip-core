#ifndef CLIPBOARDUPDATESEND_H
#define CLIPBOARDUPDATESEND_H

#include "../NetworkCommand.h"

class ClipboardUpdateSend : public NetworkCommand
{
public:
	ClipboardUpdateSend(ClipboardContainer *cont, Communicator::Role r);
	virtual Type type() const;
	virtual void receive(QDataStream &ds);
	virtual void send(QDataStream &ds);
};

#endif // CLIPBOARDUPDATESEND_H
