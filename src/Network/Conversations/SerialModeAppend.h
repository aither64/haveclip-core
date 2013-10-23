#ifndef SERIALMODEAPPEND_H
#define SERIALMODEAPPEND_H

#include "SerialModeBase.h"
#include "HistoryMixin.h"

namespace Conversations {
	class SerialModeAppend : public SerialModeBase, public HistoryMixin
	{
		Q_OBJECT
	public:
		SerialModeAppend(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
		virtual Type type() const;

	protected:
		virtual void nextCommandSender(BaseCommand::Type lastCmd, int index);
		virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);

	private:
		bool m_morph;
	};
}

#endif // SERIALMODEAPPEND_H
