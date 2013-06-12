#ifndef CLIENT_H
#define CLIENT_H

#include <QSslSocket>
#include "HaveClip.h"

/* Represents client connecting to this instance of HaveClip */
class Receiver : public QSslSocket
{
	Q_OBJECT
public:
	explicit Receiver(HaveClip::Encryption enc, QObject *parent = 0);
	void communicate();
	void setCertificateAndKey(QString cert, QString key);
	void setAcceptPassword(QString password);
	
signals:
	void clipboardUpdated(ClipboardContent *content);
	
public slots:

private:
	QByteArray buffer;
	quint64 len;
	quint64 dataRead;
	HaveClip::Encryption encryption;
	QString certificate;
	QString privateKey;
	QString password;

private slots:
	void onRead();
	void onDisconnect();
	void onSslError(const QList<QSslError> &errors);
	
};

#endif // CLIENT_H
