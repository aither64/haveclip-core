#ifndef CLIPBOARDUPDATECONFIRM_H
#define CLIPBOARDUPDATECONFIRM_H

#include "../NetworkCommand.h"

class ClipboardUpdateConfirm : public NetworkCommand
{
public:
	ClipboardUpdateConfirm(ClipboardContainer *cont, Communicator::Role r);
	virtual Type type() const;
	virtual void receive(QDataStream &ds);
	virtual void send(QDataStream &ds);
};

#endif // CLIPBOARDUPDATECONFIRM_H
