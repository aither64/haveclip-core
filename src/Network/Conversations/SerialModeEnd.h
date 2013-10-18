#ifndef SERIALMODEEND_H
#define SERIALMODEEND_H

#include "SerialModeBase.h"

namespace Conversations {
	class SerialModeEnd : public SerialModeBase
	{
		Q_OBJECT
	public:
		SerialModeEnd(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
		virtual Type type() const;

	protected:
		virtual void nextCommandSender(BaseCommand::Type lastCmd, int index);
		virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);
	};
}

#endif // SERIALMODEEND_H
