#ifndef SERIALMODEBASE_H
#define SERIALMODEBASE_H

#include "../Conversation.h"

namespace Conversations {
	class SerialModeBase : public Conversation
	{
		Q_OBJECT
	public:
		explicit SerialModeBase(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
		virtual Type type() const = 0;
		void setBatchId(qint64 id);
		BaseCommand* addSerialCommand(BaseCommand::Type t, Communicator::Role r);

	protected:
		qint64 m_batchId;
	};
}

#endif // SERIALMODEBASE_H
