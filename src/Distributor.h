#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <QTcpSocket>
#include "HaveClip.h"

class Distributor : public QTcpSocket
{
	Q_OBJECT
public:
	enum Protocol {
		CLIPBOARD_SYNC=1
	};

	explicit Distributor(HaveClip::Node *node, QObject *parent = 0);
	
signals:
	
public slots:
	void distribute(HaveClip::MimeType type, QVariant data);

private:
	HaveClip::Node *node;
	HaveClip::MimeType type;
	QVariant data;

private slots:
	void onError(QAbstractSocket::SocketError socketError);
	void onConnect();
	void onDisconnect();
	
};

#endif // DISTRIBUTOR_H
