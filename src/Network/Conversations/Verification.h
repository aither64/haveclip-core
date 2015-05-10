/*
  HaveClip

  Copyright (C) 2013-2015 Jakub Skokan <aither@havefun.cz>

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

#ifndef VERIFICATION_H
#define VERIFICATION_H

#include "../Conversation.h"

namespace Conversations {
	class Verification : public Conversation
	{
		Q_OBJECT
	public:
		explicit Verification(Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);

		virtual Type type() const;
		virtual ConnectionManager::AuthMode authenticate();
		void setSecurityCode(QString code);
		void setValid(ConnectionManager::CodeValidity v);

	protected:
		virtual void nextCommandReceiver(BaseCommand::Type lastCmd, int index);
		virtual void postDone();

	private:
		ConnectionManager::CodeValidity m_valid;

		BaseCommand::Status codeValidityToCommandStatus(ConnectionManager::CodeValidity v);
		ConnectionManager::CodeValidity commandStatusToCodeValidity(BaseCommand::Status s);
	};
}

#endif // VERIFICATION_H
