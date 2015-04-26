#include "CertificateGenerator.h"

#include <QThread>
#include <QFileInfo>
#include <QDir>

#include "CertificateGeneratorThread.h"

CertificateGenerator::CertificateGenerator(QObject *parent) :
		QObject(parent),
		m_generator(0),
		m_phase(GenerateFailed)
{
}

CertificateGenerator::~CertificateGenerator()
{
	if (m_generator)
		m_generator->deleteLater();
}

QString CertificateGenerator::commonName()
{
    return m_commonName;
}

void CertificateGenerator::setCommonName(QString name)
{
    if (m_commonName != name)
    {
        m_commonName = name;
        emit commonNameChanged(m_commonName);
    }
}

void CertificateGenerator::generate()
{
	QThread *thread = new QThread(this);
	m_generator = new CertificateGeneratorThread(m_commonName);
	m_generator->moveToThread(thread);

	connect(this, SIGNAL(start()), m_generator, SLOT(generate()));
	connect(m_generator, SIGNAL(errorOccurred(QString)), this, SLOT(error(QString)));
	connect(m_generator, SIGNAL(finished()), this, SLOT(finish()));

	thread->start();

	emit start();
}

bool CertificateGenerator::mkpath(QString &path)
{
	QFileInfo f(path);
	QDir d;

	if(d.mkpath(f.absolutePath()))
	{
		return true;

	} else {
		emit errorOccurred(MkpathFailed, f.absolutePath());
		return false;
	}
}

void CertificateGenerator::savePrivateKeyToFile(QString path)
{
	if(!mkpath(path))
		return;

	m_generator->savePrivateKeyToFile(path);
}

void CertificateGenerator::saveCertificateToFile(QString path)
{
	if(!mkpath(path))
		return;

	m_generator->saveCertificateToFile(path);
}

void CertificateGenerator::error(const QString &error)
{
	emit errorOccurred(m_phase, error);
}

void CertificateGenerator::finish()
{
	m_phase = SaveFailed;

	emit finished();
}
