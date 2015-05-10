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

#ifndef NODE_H
#define NODE_H

#include <QSettings>
#include <QSslCertificate>

class Node
{
public:
	Node();
	Node(const Node &other);
	unsigned int id() const;
	QString name() const;
	QString host() const;
	quint16 port() const;
	bool isSendEnabled() const;
	bool isReceiveEnabled() const;
	QSslCertificate certificate() const;
	bool isCompatible() const;
	bool isValid() const;
	bool hasId() const;
	void setName(QString name);
	void setHost(QString host);
	void setPort(quint16 port);
	void setSendEnabled(bool send);
	void setReceiveEnabled(bool recv);
	void setCertificate(QSslCertificate cert);
	void setCompatible(bool compat);
	void setId();
	void update(const Node &other);
	static Node load(QSettings *settings, unsigned int id);
	void save(QSettings *settings);

private:
	unsigned int m_id;
	QString m_name;
	QString m_host;
	quint16 m_port;
	bool m_send;
	bool m_recv;
	QSslCertificate m_certificate;
	bool m_compatible;
};

#endif // NODE_H
