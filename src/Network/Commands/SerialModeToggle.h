#ifndef SERIALMODEBEGIN_H
#define SERIALMODEBEGIN_H

#include "Cmd_SerialModeBase.h"

namespace Commands {
	class SerialModeToggle : public SerialModeBase
	{
	public:
		SerialModeToggle(ClipboardContainer *cont, Communicator::Role r);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);
	};
}

#endif // SERIALMODEBEGIN_H
