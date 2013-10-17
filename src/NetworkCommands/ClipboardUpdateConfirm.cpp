#include "ClipboardUpdateConfirm.h"

ClipboardUpdateConfirm::ClipboardUpdateConfirm(ClipboardContainer *cont, Communicator::Role r)
	: NetworkCommand(cont, r)
{
}

NetworkCommand::Type ClipboardUpdateConfirm::type() const
{
	return NetworkCommand::ClipboardUpdateConfirm;
}

void ClipboardUpdateConfirm::receive(QDataStream &ds)
{
	readStatus(ds);

	QStringList tmp;
	ds >> tmp;
	ds >> tmp;

	finish();
}

void ClipboardUpdateConfirm::send(QDataStream &ds)
{
	writeStatus(ds, NetworkCommand::Ok);

	ds << QStringList();
	ds << QStringList();

	finish();
}
