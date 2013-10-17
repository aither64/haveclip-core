#include "ClipboardUpdateReady.h"

ClipboardUpdateReady::ClipboardUpdateReady(ClipboardContainer *cont, Communicator::Role r)
	: NetworkCommand(cont, r)
{
}

NetworkCommand::Type ClipboardUpdateReady::type() const
{
	return NetworkCommand::ClipboardUpdateReady;
}

void ClipboardUpdateReady::receive(QDataStream &ds)
{
	qint32 mode;
	ds >> mode;

	// determine if we care about this mode

	finish();
}

void ClipboardUpdateReady::send(QDataStream &ds)
{
	ds << (qint32) m_cont->mode;

	finish();
}
