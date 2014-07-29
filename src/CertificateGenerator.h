#ifndef CERTIFICATEGENERATOR_H
#define CERTIFICATEGENERATOR_H

#include <QObject>
#include <QtCrypto/QtCrypto>

class CertificateGenerator : public QObject
{
	Q_OBJECT
public:
	enum ErrorType {
		UnsupportedKeyType,
		MkpathFailed,
		SaveFailed
	};

	explicit CertificateGenerator(QObject *parent = 0);
	~CertificateGenerator();
	void generate();
	void generatePrivateKey(QCA::PKey::Type type, int bits, QCA::DLGroupSet set);
	void savePrivateKeyToFile(QString path);
	void saveCertificateToFile(QString path);

signals:
	void privateKeyGenerated();
	void finished();
	void errorOccured(CertificateGenerator::ErrorType type, const QString &msg);

public slots:
	void generateCertificate();

private slots:
	void privateKeyReady();

private:
	QCA::Initializer m_qcaInit;
	QCA::KeyGenerator m_generator;
	QCA::PKey::Type m_type;
	QCA::PrivateKey m_key;
	QCA::DLGroupSet m_set;
	QCA::DLGroup m_group;
	QCA::Certificate m_cert;

	bool mkpath(QString &path);

};

#endif // CERTIFICATEGENERATOR_H
