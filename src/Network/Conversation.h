/*
  HaveClip

  Copyright (C) 2013 Jakub Skokan <aither@havefun.cz>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONVERSATION_H
#define CONVERSATION_H

#include <QObject>
#include <QDataStream>

#include "ConnectionManager.h"
#include "../ClipboardItem.h"
#include "Command.h"

namespace Conversations {
	class Verification;
}

class Conversation : public QObject
{
	Q_OBJECT
public:
	enum Type {
		Introduction,
		Verification,
		ClipboardUpdate
	};

	Conversation(Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
	~Conversation();
	Communicator::Role currentRole() const;
	BaseCommand::Type currentCommandType() const;
	bool isDone() const;
	virtual Type type() const = 0;
	virtual void receive(QDataStream &ds);
	virtual void send(QDataStream &ds);
	virtual void postDone();
	virtual ConnectionManager::AuthMode authenticate();

protected:
	Communicator::Role m_role;
	ClipboardContainer *m_cont;
	QList<BaseCommand*> m_cmds;
	int m_currentCmd;
	bool m_done;

	BaseCommand* addCommand(BaseCommand::Type t, Communicator::Role r);
	Communicator::Role reverse(Communicator::Role r) const;
	virtual void moveToNextCommand();
	virtual void nextCommand(BaseCommand::Type lastCmd, int index);
	virtual void nextCommandSender(BaseCommand::Type lastCmd, int index);
	virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);
	virtual void postDoneSender();
	virtual void postDoneReceiver();
	void confirm(BaseCommand::Status s);
	void morph(Conversation *c);

signals:
	void introductionFinished(QString name);
	void verificationRequested(QString name, quint16 port);
	void verificationCodeReceived(Conversations::Verification *v, QString code);
	void verificationFinished(int validity);
	void clipboardSync(ClipboardContainer *cont);
	void done();
	void morphed(Conversation *c);
};

#endif // CONVERSATION_H
