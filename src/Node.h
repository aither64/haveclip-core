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
	QSslCertificate certificate() const;
	bool isCompatible() const;
	bool isValid() const;
	bool hasId() const;
	void setName(QString name);
	void setHost(QString host);
	void setPort(quint16 port);
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
	QSslCertificate m_certificate;
	bool m_compatible;
};

#endif // NODE_H
