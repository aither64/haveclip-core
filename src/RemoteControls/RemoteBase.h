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

#ifndef REMOTEBASE_H
#define REMOTEBASE_H

#include <QObject>
#include <QDataStream>
#include "../Cli.h"
#include "../RemoteControl.h"
#include "../ClipboardManager.h"

namespace RemoteControls {
	class RemoteBase : public QObject
	{
		Q_OBJECT

	public:
		explicit RemoteBase(QObject *parent = 0);
		~RemoteBase();
		void setCli(Cli *cli);
		void setManager(ClipboardManager *manager);
		virtual RemoteControl::Command type() = 0;
		virtual bool sendCommand(QDataStream &ds) = 0;
		virtual bool receiveCommand(QDataStream &ds) = 0;

	protected:
		Cli *m_cli;
		ClipboardManager *m_manager;
	};
}

#endif // REMOTEBASE_H
