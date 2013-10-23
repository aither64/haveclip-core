#ifndef SERIALMODEINFO_H
#define SERIALMODEINFO_H

#include "Cmd_SerialModeBase.h"

namespace Commands {
	class SerialModeInfo : public SerialModeBase
	{
	public:
		SerialModeInfo(ClipboardContainer *cont, Communicator::Role r);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);
		qint32 itemCount();
		qint32 currentIndex();

	private:
		qint32 m_itemCnt;
		qint32 m_currentIndex;
	};
}

#endif // SERIALMODEINFO_H
