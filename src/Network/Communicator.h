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

#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#define HEADER_SIZE 24

#include <QSslSocket>

#include "../ClipboardManager.h"

class Conversation;

class Communicator : public QSslSocket
{
	Q_OBJECT
public:
	enum Role {
		Send,
		Receive
	};

	explicit Communicator(QObject *parent = 0);
	~Communicator();
	ClipboardManager::Node *node();
	void setCertificateAndKey(QString cert, QString key);
	void setPassword(QString m_password);

signals:
	void untrustedCertificateError(ClipboardManager::Node *node, const QList<QSslError> errors);
	void sslFatalError(const QList<QSslError> errors);

protected:
	ClipboardContainer *container;
	ClipboardManager::Encryption encryption;
	QString m_password;
	Conversation *m_conversation;

	void sendMessage();
	void receiveMessage();
	void readHeader();
	virtual void conversationSignals();
	void continueConversation();

protected slots:
	virtual void onError(QAbstractSocket::SocketError socketError);
	virtual void onSslError(const QList<QSslError> &errors);
	void onConnect();
	void onRead();
	void onDisconnect();
	virtual void conversationDone();
	void morphConversation(Conversation *c);

private:
	// Current message
	quint64 dataRead;
	QByteArray buffer;
	bool haveHeader;
	quint64 msgLen;
	
};

#endif // COMMUNICATOR_H
