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

#include "CertificateInfo.h"

#include <QStringList>

CertificateInfo::CertificateInfo(QObject *parent) :
        QObject(parent)
{
}

CertificateInfo::CertificateInfo(const QSslCertificate &cert, QObject *parent) :
	QObject(parent),
	m_cert(cert)
{

}

bool CertificateInfo::isNull() const
{
	return m_cert.isNull();
}

QString CertificateInfo::commonName() const
{
	return subjectInfo(QSslCertificate::CommonName);
}

QString CertificateInfo::organization() const
{
	return subjectInfo(QSslCertificate::Organization);
}

QString CertificateInfo::organizationUnit() const
{
	return subjectInfo(QSslCertificate::OrganizationalUnitName);
}

QString CertificateInfo::serialNumber() const
{
	return formatDigest(m_cert.serialNumber());
}

QDateTime CertificateInfo::issuedOn() const
{
	return m_cert.effectiveDate();
}

QDateTime CertificateInfo::expiryDate() const
{
	return m_cert.expiryDate();
}

QString CertificateInfo::sha1Digest() const
{
	return formatDigest(m_cert.digest(QCryptographicHash::Sha1));
}

QString CertificateInfo::md5Digest() const
{
	return formatDigest(m_cert.digest(QCryptographicHash::Md5));
}

void CertificateInfo::setCertificate(const QSslCertificate &cert)
{
	m_cert = cert;

	emit nullChanged();
	emit commonNameChanged();
	emit organizationChanged();
	emit organizationUnitChanged();
	emit serialNumberChanged();
	emit issuedOnChanged();
	emit expiryDateChanged();
	emit sha1DigestChanged();
	emit md5DigestChanged();
}

QString CertificateInfo::formatDigest(QByteArray raw) const
{
	QString digest = QString(raw.toHex()).toUpper();

	for(int i = 2; i < digest.size(); i+=3)
		digest.insert(i, ":");

	return digest;
}

QString CertificateInfo::subjectInfo(QSslCertificate::SubjectInfo info) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	return m_cert.subjectInfo(info).join(" ");
#else
	return m_cert.subjectInfo(info);
#endif
}

