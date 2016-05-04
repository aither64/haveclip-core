/*
  HaveClip

  Copyright (C) 2013-2016 Jakub Skokan <aither@havefun.cz>

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

#ifndef CLIPBOARDUPDATE_H
#define CLIPBOARDUPDATE_H

#include "../Conversation.h"
#include "../../Settings.h"

namespace Conversations {
	class ClipboardUpdate : public Conversation
	{
		Q_OBJECT
	public:
		ClipboardUpdate(Communicator::Role r, ClipboardContainer *cont, QObject *parent = 0);
		virtual Type type() const;
		void setFilters(Settings::MimeFilterMode mode, const QStringList &filters);

	protected:
		virtual void nextCommand(BaseCommand::Type lastCmd, int index);

	private:
		Settings::MimeFilterMode m_filterMode;
		QStringList m_filters;
	};
}

#endif // CLIPBOARDUPDATE_H
