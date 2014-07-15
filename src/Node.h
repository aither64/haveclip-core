#ifndef NODE_H
#define NODE_H

#include <QSettings>
#include <QSslCertificate>

class Node
{
public:
	Node(Node *other = 0);
	QString name();
	QString host();
	quint16 port();
	QSslCertificate certificate();
	bool isCompatible() const;
	void setName(QString name);
	void setHost(QString host);
	void setPort(quint16 port);
	void setCertificate(QSslCertificate cert);
	void setCompatible(bool compat);
	static Node* load(QSettings *settings);
	void save(QSettings *settings);

private:
	QString m_name;
	QString m_host;
	quint16 m_port;
	QSslCertificate m_certificate;
	bool m_compatible;
};

#endif // NODE_H
