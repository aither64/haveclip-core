#ifndef SERIALMODECOPY_H
#define SERIALMODECOPY_H

#include "SerialModeBase.h"
#include "../Commands/SerialModeInfo.h"

namespace Conversations {
	class SerialModeCopy : public SerialModeBase
	{
		Q_OBJECT
	public:
		SerialModeCopy(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);

	protected:
		virtual void nextCommandSender(BaseCommand::Type lastCmd, int index);
		virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);

	private:
		Commands::SerialModeInfo *m_cmdInfo;
	};
}

#endif // SERIALMODECOPY_H
