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

    Q_ENUMS(ErrorType)

	explicit CertificateGenerator(QObject *parent = 0);
	~CertificateGenerator();

    Q_PROPERTY(QString commonName READ commonName WRITE setCommonName NOTIFY commonNameChanged)
    QString commonName();
	void setCommonName(QString name);

    Q_INVOKABLE void generate();
	void generatePrivateKey(QCA::PKey::Type type, int bits, QCA::DLGroupSet set);
    Q_INVOKABLE void savePrivateKeyToFile(QString path);
    Q_INVOKABLE void saveCertificateToFile(QString path);

signals:
    void commonNameChanged(const QString &name);
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
	QString m_commonName;

	bool mkpath(QString &path);

};

#endif // CERTIFICATEGENERATOR_H
