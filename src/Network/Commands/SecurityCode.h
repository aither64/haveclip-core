#ifndef SECURITYCODE_H
#define SECURITYCODE_H

#include "../Command.h"

namespace Commands {
	class SecurityCode : public BaseCommand
	{
	public:
		SecurityCode(ClipboardContainer *cont, Communicator::Role r);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);
		void setCode(QString code);
		QString code();

	private:
		QString m_code;
	};
}

#endif // SECURITYCODE_H
