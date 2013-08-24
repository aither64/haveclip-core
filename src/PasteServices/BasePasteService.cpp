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

#include <QDebug>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QUrlQuery>
#endif

#include "BasePasteService.h"

BasePasteService::BasePasteService(QObject *parent) :
	QObject(parent)
{

}

BasePasteService::BasePasteService(QSettings *settings, QObject *parent) :
	QObject(parent),
	settings(settings)
{
	manager = new QNetworkAccessManager(this);

	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
	connect(manager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(onSslError(QNetworkReply*,QList<QSslError>)));

	m_label = settings->value("Label").toString();

	QByteArray cert = settings->value("Certificate").toString().toUtf8();

	if(!cert.isEmpty())
		certificate = QSslCertificate::fromData(cert).first();
}

QString BasePasteService::label()
{
	return m_label;
}

void BasePasteService::applySettings(QHash<QString, QVariant> s)
{
	m_label = s["Label"].toString();
}

void BasePasteService::saveSettings()
{
	settings->setValue("Type", type());
	settings->setValue("Label", m_label);

	if(!certificate.isNull())
		settings->setValue("Certificate", QString(certificate.toPem()));
}

void BasePasteService::retryPaste()
{

}

void BasePasteService::requestFinished(QNetworkReply *reply)
{

}

void BasePasteService::provideAuthentication(QString username, QString password)
{

}

void BasePasteService::onSslError(QNetworkReply *reply, const QList<QSslError> &errors)
{
	QList<QSslError::SslError> recoverable;
	recoverable << QSslError::SelfSignedCertificate
		<< QSslError::CertificateUntrusted
		<< QSslError::HostNameMismatch
		<< QSslError::CertificateExpired;

	bool exception = true;

	foreach(QSslError e, errors)
	{
		if(!recoverable.contains(e.error()))
		{
			qDebug() << "Unrecoverable SSL error" << e;
			return;
		}

		if(e.certificate() != certificate)
		{
			exception = false;
			break;
		}
	}

	if(exception)
	{
		qDebug() << "SSL errors ignored because of exception";
		reply->ignoreSslErrors();

	} else {
		emit untrustedCertificateError(this, errors);
	}
}

QByteArray BasePasteService::buildPostData(QHash<QString, QString> &data)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	QUrlQuery post;
#else
	QUrl post;
#endif

	QHashIterator<QString, QString> i(data);

	while(i.hasNext())
	{
		i.next();

		post.addQueryItem(i.key(), i.value());
	}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
	return post.toString(QUrl::FullyEncoded).replace('+', "%2B").toUtf8();
#else
	return post.encodedQuery().replace('+', "%2B");
#endif
}

int BasePasteService::langIndexFromName(Language* langs, QString name)
{
	if(langs == 0)
		return -1;

	for(int i = 0; langs[i].name != 0; i++)
		if(langs[i].name == name)
			return i;

	return -1;
}

void BasePasteService::setCertificate(QSslCertificate cert)
{
	certificate = cert;
}
