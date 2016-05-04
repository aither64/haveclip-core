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

#ifndef CLIENT_H
#define CLIENT_H

#include <QSslSocket>

#include "Communicator.h"

namespace Conversations {
	class Verification;
}

/* Represents client connecting to this instance of HaveClip */
class Receiver : public Communicator
{
	Q_OBJECT
public:
	explicit Receiver(ConnectionManager *parent = 0);
	void communicate();

protected:
	virtual void conversationSignals();
	
signals:
	void verificationRequested(const Node &n);
	void verificationCodeReceived(Conversations::Verification *v, QString code);
	void clipboardUpdated(ClipboardContainer *cont);

private slots:
	void interceptVerificationRequest(QString name, quint16 port);
	
};

#endif // CLIENT_H
