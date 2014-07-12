#ifndef PING_H
#define PING_H

#include "../Command.h"

namespace Commands {
	class Ping : public BaseCommand
	{
	public:
		Ping(ClipboardContainer *cont, Communicator::Role r);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);

	signals:

	public slots:

	};
}

#endif // PING_H
