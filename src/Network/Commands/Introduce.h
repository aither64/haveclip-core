#ifndef INTRODUCE_H
#define INTRODUCE_H

#include "../Command.h"

namespace Commands {
	class Introduce : public BaseCommand
	{
	public:
		Introduce(ClipboardContainer *cont, Communicator::Role r);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);
		QString name();
		void setName(QString name);
		quint16 port();
		void setPort(quint16 port);

	private:
		QString m_name;
		quint16 m_port;

	};
}

#endif // INTRODUCE_H
