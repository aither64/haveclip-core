#ifndef CLIPBOARDUPDATEREADY_H
#define CLIPBOARDUPDATEREADY_H

#include "../NetworkCommand.h"

class ClipboardUpdateReady : public NetworkCommand
{
public:
	ClipboardUpdateReady(ClipboardContainer *cont, Communicator::Role r);
	virtual Type type() const;
	virtual void receive(QDataStream &ds);
	virtual void send(QDataStream &ds);
};

#endif // CLIPBOARDUPDATEREADY_H
