#ifndef SERIALMODENEXT_H
#define SERIALMODENEXT_H

#include "SerialModeBase.h"
#include "HistoryMixin.h"

namespace Conversations {
	class SerialModeNext : public SerialModeBase, public HistoryMixin
	{
		Q_OBJECT
	public:
		SerialModeNext(qint64 id, Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
		virtual Type type() const;

	protected:
		virtual void nextCommandSender(BaseCommand::Type lastCmd, int index);
		virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);

	private:
		bool m_morph;
	};
}

#endif // SERIALMODENEXT_H
