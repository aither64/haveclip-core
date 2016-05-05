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

#include "qmlhelpers.h"

#include "../Settings.h"
#include "../ClipboardManager.h"
#include "qmlnode.h"
#include "../CertificateInfo.h"

QmlHelpers::QmlHelpers(QObject *parent) :
	QObject(parent),
	m_verifiedNode(0),
	m_selfCert(0)
{
	m_selfCert = new CertificateInfo(Settings::get()->certificate(), this);

	connect(Settings::get(), SIGNAL(certificateChanged(QSslCertificate)), this, SLOT(updateSelfSslCertificate(QSslCertificate)));
}

CertificateInfo* QmlHelpers::selfSslCertificate() const
{
	return m_selfCert;
}

QmlNode* QmlHelpers::verifiedNode()
{
	const Node &n = ClipboardManager::instance()->connectionManager()->verifiedNode();

	if(!m_verifiedNode)
		m_verifiedNode = new QmlNode(n, this);

	else if(m_verifiedNode->node().id() != n.id())
		m_verifiedNode->setNode(n);

	return m_verifiedNode;
}

QString QmlHelpers::communicationStatusToString(Communicator::CommunicationStatus status) const
{
	return Communicator::statusToString(status);
}

void QmlHelpers::updateSelfSslCertificate(const QSslCertificate &certificate)
{
	m_selfCert->setCertificate(certificate);
}
