#ifndef BEGINSERIALMODE_H
#define BEGINSERIALMODE_H

#include "SerialModeBase.h"

namespace Conversations {
	class SerialModeBegin : public SerialModeBase
	{
		Q_OBJECT
	public:
		SerialModeBegin(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
		virtual Type type() const;

	protected:
		virtual void nextCommandSender(BaseCommand::Type lastCmd, int index);
		virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);
	};
}

#endif // BEGINSERIALMODE_H
