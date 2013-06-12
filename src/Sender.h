#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <QSslSocket>
#include <QMimeData>
#include "HaveClip.h"

class Sender : public QSslSocket
{
	Q_OBJECT
public:
	enum Protocol {
		CLIPBOARD_SYNC=1
	};

	explicit Sender(HaveClip::Encryption enc, HaveClip::Node *node, QObject *parent = 0);
	
signals:
	
public slots:
	void distribute(const ClipboardContent *content);

private:
	HaveClip::Node *node;
	const ClipboardContent *content;
	HaveClip::Encryption encryption;

private slots:
	void onError(QAbstractSocket::SocketError socketError);
	void onSslError(const QList<QSslError> &errors);
	void onConnect();
	void onDisconnect();
	
};

#endif // DISTRIBUTOR_H
