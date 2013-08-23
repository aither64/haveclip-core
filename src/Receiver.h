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

#ifndef CLIENT_H
#define CLIENT_H

#include <QSslSocket>
#include "HaveClip.h"

/* Represents client connecting to this instance of HaveClip */
class Receiver : public QSslSocket
{
	Q_OBJECT
public:
	explicit Receiver(ClipboardManager::Encryption enc, QObject *parent = 0);
	void communicate();
	void setCertificateAndKey(QString cert, QString key);
	void setAcceptPassword(QString m_password);
	
signals:
	void clipboardUpdated(ClipboardContent *content);
	
public slots:

private:
	QByteArray buffer;
	quint64 len;
	quint64 dataRead;
	ClipboardManager::Encryption encryption;
	QString certificate;
	QString privateKey;
	QString m_password;

private slots:
	void onRead();
	void onDisconnect();
	void onSslError(const QList<QSslError> &errors);
	
};

#endif // CLIENT_H
