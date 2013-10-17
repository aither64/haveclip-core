#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <QObject>
#include <QDataStream>

#include "ClipboardItem.h"
#include "NetworkCommand.h"

class Conversation : public QObject
{
	Q_OBJECT
public:
	enum Type {
		ClipboardUpdate,
		SerialModeBegin,
		SerialModeEnd,
		SerialModeAppend,
		SerialModeNext
	};

	Conversation(Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
	~Conversation();
	Communicator::Role currentRole() const;
	NetworkCommand::Type currentCommandType() const;
	bool isDone() const;
	virtual Type type() const = 0;
	virtual void receive(QDataStream &ds);
	virtual void send(QDataStream &ds);

protected:
	Communicator::Role m_role;
	ClipboardContainer *m_cont;
	QList<NetworkCommand*> m_cmds;
	int m_currentCmd;
	bool m_done;

	NetworkCommand* addCommand(NetworkCommand::Type t, Communicator::Role r);
	Communicator::Role reverse(Communicator::Role r) const;
	virtual void moveToNextCommand();
	virtual void nextCommand(NetworkCommand::Type lastCmd, int index);

signals:
	void clipboardSync(ClipboardContainer *cont);
	void done();
};

#endif // CONVERSATION_H
