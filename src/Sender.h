#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <QTcpSocket>
#include <QMimeData>
#include "HaveClip.h"

class Sender : public QTcpSocket
{
	Q_OBJECT
public:
	enum Protocol {
		CLIPBOARD_SYNC=1
	};

	explicit Sender(HaveClip::Node *node, QObject *parent = 0);
	
signals:
	
public slots:
	void distribute(const ClipboardContent *content);

private:
	HaveClip::Node *node;
	const ClipboardContent *content;

private slots:
	void onError(QAbstractSocket::SocketError socketError);
	void onConnect();
	void onDisconnect();
	
};

#endif // DISTRIBUTOR_H
