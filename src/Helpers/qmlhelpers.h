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

#ifndef QMLHELPERS_H
#define QMLHELPERS_H

#include <QObject>

#include "../Network/Communicator.h"

class QmlNode;
class CertificateInfo;

class QmlHelpers : public QObject
{
	Q_OBJECT
public:
	explicit QmlHelpers(QObject *parent = 0);

	Q_PROPERTY(CertificateInfo* selfSslCertificate READ selfSslCertificate NOTIFY selfSslCertificateChanged)
	CertificateInfo* selfSslCertificate() const;

	Q_INVOKABLE QmlNode* verifiedNode();
	Q_INVOKABLE QString communicationStatusToString(Communicator::CommunicationStatus status) const;

signals:
	void selfSslCertificateChanged();
	void verificationRequested(QmlNode *node);

private slots:
	void updateSelfSslCertificate(const QSslCertificate &certificate);

private:
	QmlNode *m_verifiedNode;
	CertificateInfo *m_selfCert;

};

#endif // QMLHELPERS_H
