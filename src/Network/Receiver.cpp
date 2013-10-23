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

#include <QStringList>
#include <QImage>
#include <QDataStream>

#include "Receiver.h"
#include "Conversation.h"

Receiver::Receiver(History *history, ClipboardManager::Encryption enc, QObject *parent) :
	Communicator(history, parent)
{
	encryption = enc;
}

void Receiver::communicate()
{
	if(encryption != ClipboardManager::None)
	{
		switch(encryption)
		{
		case ClipboardManager::Ssl:
			setProtocol(QSsl::SslV3);
			break;
		case ClipboardManager::Tls:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
			setProtocol(QSsl::TlsV1_0);
#else
			setProtocol(QSsl::TlsV1);
#endif
			break;
		}

		setPeerVerifyMode(QSslSocket::VerifyNone);

		startServerEncryption();
	}
}

void Receiver::conversationSignals()
{
	Communicator::conversationSignals();

	connect(m_conversation, SIGNAL(clipboardSync(ClipboardContainer*)), this, SIGNAL(clipboardUpdated(ClipboardContainer*)));

#ifdef INCLUDE_SERIAL_MODE
	connect(m_conversation, SIGNAL(serialModeToggled(bool,qint64)), this, SIGNAL(serialModeToggled(bool,qint64)));
	connect(m_conversation, SIGNAL(serialModeNewBatch(ClipboardSerialBatch*)), this, SIGNAL(serialModeNewBatch(ClipboardSerialBatch*)));
	connect(m_conversation, SIGNAL(serialModeAppend(ClipboardItem*)), this, SIGNAL(serialModeAppend(ClipboardItem*)));
	connect(m_conversation, SIGNAL(serialModeNext()), this, SIGNAL(serialModeNext()));
	connect(m_conversation, SIGNAL(serialModeRestart(ClipboardSerialBatch*)), this, SIGNAL(serialModeRestart(ClipboardSerialBatch*)));
#endif
}
