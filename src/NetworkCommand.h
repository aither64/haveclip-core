#ifndef NETWORKCOMMAND_H
#define NETWORKCOMMAND_H

#include <QDataStream>

#include "ClipboardItem.h"
#include "Communicator.h"

class NetworkCommand
{
public:
	enum Type {
		ClipboardUpdateReady=0,
		ClipboardUpdateConfirm,
		ClipboardUpdateSend,
		Confirm
	};

	enum Status {
		Ok=0,
		Abort,
		NotExists,
		NotMatches,
		NotUnderstood,
		Undefined
	};

	NetworkCommand(ClipboardContainer *cont, Communicator::Role r);
	Communicator::Role role() const;
	bool isFinished() const;
	Status status() const;
	ClipboardContainer *container();
	virtual Type type() const = 0;
	virtual void receive(QDataStream &ds) = 0;
	virtual void send(QDataStream &ds) = 0;

protected:
	ClipboardContainer *m_cont;
	Communicator::Role m_role;

	void readStatus(QDataStream &ds);
	void writeStatus(QDataStream &ds, Status s = Undefined);
	void setStatus(Status s);
	void finish();

private:
	bool m_finished;
	Status m_status;
};

#endif // NETWORKCOMMAND_H
