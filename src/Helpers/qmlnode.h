/*
  HaveClip

  Copyright (C) 2013-2015 Jakub Skokan <aither@havefun.cz>

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

#ifndef QMLNODE_H
#define QMLNODE_H

#include <QObject>

#include "../Node.h"

class CertificateInfo;

class QmlNode : public QObject
{
	Q_OBJECT
public:
	explicit QmlNode(QObject *parent = 0);
	QmlNode(const Node& n, QObject *parent = 0);

	Q_PROPERTY(unsigned int id READ id)
	unsigned int id() const;

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	QString name() const;
	void setName(QString name);

	Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
	QString host() const;
	void setHost(QString host);

	Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
	quint16 port() const;
	void setPort(quint16 port);

	Q_PROPERTY(bool sendEnabled READ isSendEnabled WRITE setSendEnabled NOTIFY sendEnabledChanged)
	bool isSendEnabled() const;
	void setSendEnabled(bool enabled);

	Q_PROPERTY(bool receiveEnabled READ isReceiveEnabled WRITE setReceiveEnabled NOTIFY receiveEnabledChanged)
	bool isReceiveEnabled() const;
	void setReceiveEnabled(bool enabled);

	Q_PROPERTY(CertificateInfo* sslCertificate READ sslCertificate NOTIFY sslCertificateChanged)
	CertificateInfo* sslCertificate();

	Node node() const;
	void setNode(const Node &n);

	Q_INVOKABLE void update();

signals:
	void nameChanged();
	void hostChanged();
	void portChanged(quint16 port);
	void sendEnabledChanged();
	void receiveEnabledChanged();
	void sslCertificateChanged();

private:
	Node m_node;

	CertificateInfo* m_sslCertificate;
};

#endif // QMLNODE_H
