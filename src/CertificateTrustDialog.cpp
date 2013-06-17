/*
  HaveClip

  Copyright (C) 2013 Jakub Skokan <aither@havefun.cz>

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

#include <QDateTime>

#include "CertificateTrustDialog.h"
#include "ui_CertificateTrustDialog.h"

CertificateTrustDialog::CertificateTrustDialog(HaveClip::Node *node, const QList<QSslError> &errors, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::CertificateTrustDialog)
{
	ui->setupUi(this);

	QString errStr = ui->errorLabel->text().arg(node->toString());
	errStr += "<br><br>";

	foreach(QSslError e, errors)
		errStr += e.errorString() + "<br>";

	ui->errorLabel->setText(errStr);

	QSslCertificate cert = errors.first().certificate();

	// issued to
	ui->toCommonNameLabel->setText( cert.subjectInfo(QSslCertificate::CommonName) );
	ui->toOrgLabel->setText( cert.subjectInfo(QSslCertificate::Organization) );
	ui->toOrgUnitLabel->setText( cert.subjectInfo(QSslCertificate::OrganizationalUnitName) );
	ui->serialLabel->setText( formatDigest(cert.serialNumber()) );

	// issuer
	ui->byCommonNameLabel->setText( cert.issuerInfo(QSslCertificate::CommonName) );
	ui->byOrgLabel->setText( cert.issuerInfo(QSslCertificate::Organization) );
	ui->byOrgUnitLabel->setText( cert.issuerInfo(QSslCertificate::OrganizationalUnitName) );

	// validity
	ui->issuedOnLabel->setText( cert.effectiveDate().toString("d/M/yyyy") );
	ui->expiresLabel->setText( cert.expiryDate().toString("d/M/yyyy") );

	// fingerprints
	ui->sha1FingerLabel->setText( formatDigest(cert.digest(QCryptographicHash::Sha1)) );
	ui->md5FingerLabel->setText( formatDigest(cert.digest(QCryptographicHash::Md5)) );
}

CertificateTrustDialog::~CertificateTrustDialog()
{
	delete ui;
}

QString CertificateTrustDialog::formatDigest(QByteArray raw)
{
	QString digest = QString(raw.toHex()).toUpper();

	for(int i = 2; i < digest.size(); i+=3)
		digest.insert(i, ":");

	return digest;
}

bool CertificateTrustDialog::remember()
{
	return ui->rememberCheckBox->isChecked();
}
