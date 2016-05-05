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

#ifndef CLIPBOARDUPDATESEND_H
#define CLIPBOARDUPDATESEND_H

#include <QRegExp>

#include "../Command.h"
#include "../../Settings.h"

namespace Commands {
	class ClipboardUpdateSend : public BaseCommand
	{
	public:
		ClipboardUpdateSend(ClipboardContainer *cont, Communicator::Role r);
		virtual Type type() const;
		virtual void receive(QDataStream &ds);
		virtual void send(QDataStream &ds);
		void setSendFilters(Settings::MimeFilterMode mode, const QStringList &filters);
		void setRecvFilters(Settings::MimeFilterMode mode, const QStringList &filters);

	private:
		Settings::MimeFilterMode m_sendMode;
		QStringList m_sendFilters;
		QList<QRegExp> m_sendRx;
		Settings::MimeFilterMode m_recvMode;
		QStringList m_recvFilters;
		QList<QRegExp> m_recvRx;

		void initRx(const QStringList &what, QList<QRegExp> *where);
		bool shouldSend(const QString &mimeType);
		bool shouldReceive(const QString &mimeType);
	};
}

#endif // CLIPBOARDUPDATESEND_H
