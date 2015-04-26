#ifndef CERTIFICATEGENERATORTHREAD_H
#define CERTIFICATEGENERATORTHREAD_H

#include <QObject>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#ifndef OPENSSL_NO_ENGINE
# include <openssl/engine.h>
#endif

class CertificateGeneratorThread : public QObject
{
	Q_OBJECT
public:
	explicit CertificateGeneratorThread(QString commonName, QObject *parent = 0);
	~CertificateGeneratorThread();

signals:
	void errorOccurred(const QString &error);
	void finished();

public slots:
	void generate();
	void savePrivateKeyToFile(const QString &path);
	void saveCertificateToFile(const QString &path);

private slots:
	void finish();

private:
	bool m_done;
	QString m_commonName;
	BIO *m_bio;
	X509 *m_x509;
	EVP_PKEY *m_pkey;

	bool addExtension(int nid, char *value);
	void error();
};

#endif // CERTIFICATEGENERATORTHREAD_H
