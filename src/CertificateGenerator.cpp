#include "CertificateGenerator.h"

CertificateGenerator::CertificateGenerator(QObject *parent) :
        QObject(parent)
{
	m_generator.setBlockingEnabled(false);
	connect(&m_generator, SIGNAL(finished()), this, SLOT(privateKeyReady()));
}

CertificateGenerator::~CertificateGenerator()
{

}

void CertificateGenerator::generate()
{
	QCA::PKey::Type type;

	if(QCA::isSupported("rsa"))
	{
		type = QCA::PKey::RSA;

	} else if(QCA::isSupported("dsa")) {
		type = QCA::PKey::DSA;

	} else {
		Q_ASSERT_X(false, "CertificateGenerator::generate()", "no private key type available (rsa, dsa)");
	}

	connect(this, SIGNAL(privateKeyGenerated()), this, SLOT(generateCertificate()));

	generatePrivateKey(type, 2048, QCA::DSA_1024);
}

void CertificateGenerator::generatePrivateKey(QCA::PKey::Type type, int bits, QCA::DLGroupSet set)
{
	m_type = type;

	if(type == QCA::PKey::RSA)
		m_generator.createRSA(bits);
	else
		m_generator.createDLGroup(set);
}

void CertificateGenerator::generateCertificate()
{
	QCA::CertificateOptions opts;

	QCA::CertificateInfo info;
	info.insert(QCA::CommonName, "haveclip (FIXME)");
	info.insert(QCA::Country, "Czech Republic");
	info.insert(QCA::Organization, "HaveFun.cz");
	info.insert(QCA::Email, "");
	opts.setInfo(info);

	QDateTime start = QDateTime::currentDateTime().toUTC();

	opts.setValidityPeriod(start, start.addYears(10));

	m_cert = QCA::Certificate(opts, m_key);

	emit finished();
}

void CertificateGenerator::privateKeyReady()
{
	if(m_type == QCA::PKey::DSA)
	{
		if(m_group.isNull())
		{
			m_group = m_generator.dlGroup();
			m_generator.createDSA(m_group);

			return;
		}
	}

	m_group = QCA::DLGroup();
	m_key = m_generator.key();

	emit privateKeyGenerated();
}

void CertificateGenerator::savePrivateKeyToFile(QString path)
{
	m_key.toPEMFile(path);
}

void CertificateGenerator::saveCertificateToFile(QString path)
{
	m_cert.toPEMFile(path);
}
