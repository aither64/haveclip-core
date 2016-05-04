/*
  HaveClip

  Copyright (C) 2013-2016 Jakub Skokan <aither@havefun.cz>

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

#ifndef CERTIFICATEINFO_H
#define CERTIFICATEINFO_H

#include <QObject>
#include <QSslCertificate>
#include <QDateTime>

class CertificateInfo : public QObject
{
	Q_OBJECT
public:
	explicit CertificateInfo(QObject *parent = 0);
	CertificateInfo(const QSslCertificate &cert, QObject *parent = 0);

	Q_PROPERTY(bool null READ isNull NOTIFY nullChanged)
	bool isNull() const;

	Q_PROPERTY(QString commonName READ commonName NOTIFY commonNameChanged)
	QString commonName() const;

	Q_PROPERTY(QString organization READ organization NOTIFY organizationChanged)
	QString organization() const;

	Q_PROPERTY(QString organizationUnit READ organizationUnit NOTIFY organizationUnitChanged)
	QString organizationUnit() const;

	Q_PROPERTY(QString serialNumber READ serialNumber NOTIFY serialNumberChanged)
	QString serialNumber() const;

	Q_PROPERTY(QDateTime issuedOn READ issuedOn NOTIFY issuedOnChanged)
	QDateTime issuedOn() const;

	Q_PROPERTY(QDateTime expiryDate READ expiryDate NOTIFY expiryDateChanged)
	QDateTime expiryDate() const;

	Q_PROPERTY(QString sha1Digest READ sha1Digest NOTIFY sha1DigestChanged)
	QString sha1Digest() const;

	Q_PROPERTY(QString md5Digest READ md5Digest NOTIFY md5DigestChanged)
	QString md5Digest() const;

	void setCertificate(const QSslCertificate &cert);

signals:
	void nullChanged();
	void commonNameChanged();
	void organizationChanged();
	void organizationUnitChanged();
	void serialNumberChanged();
	void issuedOnChanged();
	void expiryDateChanged();
	void sha1DigestChanged();
	void md5DigestChanged();

private:
	QSslCertificate m_cert;

	QString formatDigest(QByteArray raw) const;
	QString subjectInfo(QSslCertificate::SubjectInfo info) const;

};

#endif // CERTIFICATEINFO_H
