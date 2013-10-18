#ifndef CMD_SERIALMODEEND_H
#define CMD_SERIALMODEEND_H

#include "Cmd_SerialModeBase.h"

namespace Commands {
	class SerialModeEnd : public SerialModeBase
	{
	public:
		SerialModeEnd(ClipboardContainer *cont, Communicator::Role r);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);
	};
}

#endif // CMD_SERIALMODEEND_H
