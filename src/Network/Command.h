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

#ifndef NETWORKCOMMAND_H
#define NETWORKCOMMAND_H

#include <QDataStream>

#include "../ClipboardItem.h"
#include "Communicator.h"

class BaseCommand
{
public:
	enum Type {
		Ping=0,
		Introduce,
		SecurityCode,
		ClipboardUpdateReady,
		ClipboardUpdateConfirm,
		ClipboardUpdateSend,
		Confirm
	};

	enum Status {
		Ok=0,
		Abort,
		NotExists,
		NotMatches,
		NotUnderstood,
		Undefined
	};

	BaseCommand(ClipboardContainer *cont, Communicator::Role r);
	virtual ~BaseCommand();
	Communicator::Role role() const;
	bool isFinished() const;
	Status status() const;
	void setStatus(Status s);
	ClipboardContainer *container();
	void setContainer(ClipboardContainer *cont);
	virtual Type type() const = 0;
	virtual void receive(QDataStream &ds) = 0;
	virtual void send(QDataStream &ds) = 0;

protected:
	ClipboardContainer *m_cont;
	Communicator::Role m_role;

	void readStatus(QDataStream &ds);
	void writeStatus(QDataStream &ds, Status s = Undefined);
	void finish();

private:
	bool m_finished;
	Status m_status;

friend class Conversation;
};

#endif // NETWORKCOMMAND_H
