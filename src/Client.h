#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include "HaveClip.h"

/* Represents client connecting to this instance of HaveClip */
class Client : public QTcpSocket
{
	Q_OBJECT
public:
	explicit Client(QObject *parent = 0);
	void communicate();
	
signals:
	void clipboardUpdated(HaveClip::MimeType t, QVariant data);
	
public slots:

private:
	QByteArray buffer;
	quint64 len;
	quint64 dataRead;
	HaveClip::MimeType type;

private slots:
	void onRead();
	void onDisconnect();
	
};

#endif // CLIENT_H
