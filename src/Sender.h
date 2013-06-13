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
	HaveClip::Node *node();
	
signals:
	void untrustedCertificateError(HaveClip::Node *node, const QList<QSslError> errors);
	void sslFatalError(const QList<QSslError> errors);
	
public slots:
	void distribute(const ClipboardContent *content, QString password);

private:
	HaveClip::Node *m_node;
	const ClipboardContent *content;
	HaveClip::Encryption encryption;
	QString password;

private slots:
	void onError(QAbstractSocket::SocketError socketError);
	void onSslError(const QList<QSslError> &errors);
	void onConnect();
	void onDisconnect();
	
};

#endif // DISTRIBUTOR_H
