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
	void setDeleteContentOnSent(bool del);

private:
	HaveClip::Node *m_node;
	const ClipboardContent *content;
	HaveClip::Encryption encryption;
	QString password;
	bool deleteContent;

private slots:
	void onError(QAbstractSocket::SocketError socketError);
	void onSslError(const QList<QSslError> &errors);
	void onConnect();
	void onDisconnect();
	
};

#endif // DISTRIBUTOR_H
