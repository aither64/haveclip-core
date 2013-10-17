#ifndef CONFIRM_H
#define CONFIRM_H

#include "../NetworkCommand.h"

class Confirm : public NetworkCommand
{
public:
	Confirm(ClipboardContainer *cont, Communicator::Role r);
	virtual Type type() const;
	virtual void receive(QDataStream &ds);
	virtual void send(QDataStream &ds);
};

#endif // CONFIRM_H
