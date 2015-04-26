#ifndef CERTIFICATEGENERATOR_H
#define CERTIFICATEGENERATOR_H

#include <QObject>

class CertificateGeneratorThread;

class CertificateGenerator : public QObject
{
	Q_OBJECT
public:
	enum ErrorType {
		GenerateFailed,
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
    Q_INVOKABLE void savePrivateKeyToFile(QString path);
    Q_INVOKABLE void saveCertificateToFile(QString path);

signals:
	void start();
	void commonNameChanged(const QString &name);
	void finished();
	void errorOccurred(CertificateGenerator::ErrorType type, const QString &msg);

private slots:
	void error(const QString &error);
	void finish();

private:
	CertificateGeneratorThread *m_generator;
	QString m_commonName;
	ErrorType m_phase;

	bool mkpath(QString &path);

};

#endif // CERTIFICATEGENERATOR_H
