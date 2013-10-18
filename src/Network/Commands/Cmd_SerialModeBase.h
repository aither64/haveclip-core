#ifndef CMD_SERIALMODEBASE_H
#define CMD_SERIALMODEBASE_H

#include "../Command.h"

namespace Commands {
	class SerialModeBase : public BaseCommand
	{
	public:
		SerialModeBase(ClipboardContainer *cont, Communicator::Role r);
		void setBatchId(qint64 id);
		qint64 batchId() const;

	protected:
		qint64 m_batchId;
	};
}

#endif // CMD_SERIALMODEBASE_H
