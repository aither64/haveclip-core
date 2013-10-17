#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#define HEADER_SIZE 24

#include <QSslSocket>

#include "ClipboardManager.h"

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

private:
	// Current message
	quint64 dataRead;
	QByteArray buffer;
	bool haveHeader;
	quint64 msgLen;
	
};

#endif // COMMUNICATOR_H
