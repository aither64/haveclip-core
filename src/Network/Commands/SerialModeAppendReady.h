#ifndef SERIALMODEAPPENDREADY_H
#define SERIALMODEAPPENDREADY_H

#include "Cmd_SerialModeBase.h"

namespace Commands {
	class SerialModeAppendReady : public SerialModeBase
	{
	public:
		SerialModeAppendReady(ClipboardContainer *cont, Communicator::Role r);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);
		int itemCount();

	private:
		int m_itemCnt;
	};
}

#endif // SERIALMODEAPPENDREADY_H
