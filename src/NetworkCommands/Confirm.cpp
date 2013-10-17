#include "Confirm.h"

Confirm::Confirm(ClipboardContainer *cont, Communicator::Role r)
	: NetworkCommand(cont, r)
{
}

NetworkCommand::Type Confirm::type() const
{
	return NetworkCommand::Confirm;
}

void Confirm::receive(QDataStream &ds)
{
	readStatus(ds);

	finish();
}

void Confirm::send(QDataStream &ds)
{
	writeStatus(ds, Ok);

	finish();
}
