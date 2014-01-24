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

#include "HaveSnippet.h"

#include <QDateTime>
#include <QDebug>

HaveSnippet::HaveSnippet(QObject *parent) :
	BasePasteService(parent)
{

}

HaveSnippet::HaveSnippet(QSettings *settings, QObject *parent) :
	BasePasteService(settings, parent)
{
	m_url = settings->value("Url").toString();
	m_name = settings->value("Name").toString();
	m_title = settings->value("Title").toString();
	m_shortDescription = settings->value("ShortDescription").toString();
	m_lang = settings->value("Language", "text").toString();
	m_fullDescription = settings->value("FullDescription").toString();
	m_tags = settings->value("Tags").toString();
	m_accessibility = settings->value("Accessibility", 1).toInt();
	m_expire = settings->value("Expiration", 0).toInt();
	m_apiKey = settings->value("ApiKey").toString();
}

BasePasteService::PasteService HaveSnippet::type()
{
	return BasePasteService::HaveSnippet;
}

QString HaveSnippet::internalName()
{
	return "HaveSnippet";
}

void HaveSnippet::applySettings(QHash<QString, QVariant> s)
{
	BasePasteService::applySettings(s);

	m_url = s["Url"].toString();
	m_name = s["Name"].toString();
	m_title = s["Title"].toString();
	m_shortDescription = s["ShortDescription"].toString();
	m_lang = s["Language"].toString();
	m_fullDescription = s["FullDescription"].toString();
	m_tags = s["Tags"].toString();
	m_accessibility = s["Accessibility"].toInt();
	m_expire = s["Expiration"].toInt();
	m_apiKey = s["ApiKey"].toString();
}

void HaveSnippet::saveSettings()
{
	BasePasteService::saveSettings();

	settings->setValue("Url", m_url);
	settings->setValue("Name", m_name);
	settings->setValue("Title", m_title);
	settings->setValue("ShortDescription", m_shortDescription);
	settings->setValue("Language", m_lang);
	settings->setValue("FullDescription", m_fullDescription);
	settings->setValue("Tags", m_tags);
	settings->setValue("Accessibility", m_accessibility);
	settings->setValue("Expiration", m_expire);
	settings->setValue("ApiKey", m_apiKey);
}

QString HaveSnippet::url()
{
	return m_url;
}

QString HaveSnippet::name()
{
	return m_name;
}

QString HaveSnippet::title()
{
	return m_title;
}

QString HaveSnippet::shortDescription()
{
	return m_shortDescription;
}

QString HaveSnippet::lang()
{
	return m_lang;
}

QString HaveSnippet::fullDescription()
{
	return m_fullDescription;
}

QString HaveSnippet::tags()
{
	return m_tags;
}

int HaveSnippet::accessibility()
{
	return m_accessibility;
}


int HaveSnippet::expiration()
{
	return m_expire;
}

QString HaveSnippet::apiKey()
{
	return m_apiKey;
}

QUrl HaveSnippet::apiUrl(QString base, QString cmd)
{
	base = base.trimmed();

	while(base.endsWith("/"))
		base.chop(1);

	if(!base.endsWith("/api"))
		base += "/api";

	base += "/" + cmd + "/";

	return QUrl(base);
}

void HaveSnippet::paste(QString data)
{
	QHash<QString, QString> post;
	post["nick"] = m_name;
	post["title"] = m_title;
	post["short_description"] = m_shortDescription;
	post["language"] = m_lang;
	post["full_description"] = m_fullDescription;
	post["tags"] = m_tags;
	post["accessibility"] = QString::number(m_accessibility);
	post["expiration"] = QString::number( QDateTime::currentDateTimeUtc().addSecs( m_expire * 60 ).toMSecsSinceEpoch() / 1000 );
	post["content"] = data;

	paste(post);
}

void HaveSnippet::paste(QHash<QString, QVariant> settings, QString data)
{
	QHash<QString, QString> post;
	post["nick"] = settings["Name"].toString();
	post["title"] = settings["Title"].toString();
	post["short_description"] = settings["ShortDescription"].toString();
	post["language"] = settings["Language"].toString();
	post["full_description"] = settings["FullDescription"].toString();
	post["tags"] = settings["Tags"].toString();
	post["accessibility"] = QString::number(settings["Accessibility"].toInt());
	post["expiration"] = QString::number( QDateTime::currentDateTimeUtc().addSecs( settings["Expiration"].toInt() * 60 ).toMSecsSinceEpoch() / 1000 );
	post["content"] = data;

	paste(post);
}

void HaveSnippet::paste(QHash<QString, QString> &post)
{
	if(!m_apiKey.isEmpty())
		post["api_key"] = m_apiKey;

	lastPaste = buildPostData(post);

	retryPaste();
}

void HaveSnippet::retryPaste()
{
	QNetworkRequest request(apiUrl(m_url, "paste"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	qDebug() << lastPaste;

	manager->post(request, lastPaste);
}

void HaveSnippet::requestFinished(QNetworkReply *reply)
{
	if(reply->error() != QNetworkReply::NoError)
	{
		qDebug() << "Error pasting to havesnippet" << reply->error();
		emit errorOccured(reply->errorString());
		reply->deleteLater();
		return;
	}

	QString ret = QString(reply->readAll()).trimmed();

	if(ret.isEmpty() || ret.startsWith("ERROR"))
	{
		emit errorOccured(tr("Bad response from %1").arg(label()));

	} else {

		qDebug() << "Paste link" << ret;

		emit pasted(QUrl(ret));
	}

	reply->deleteLater();
}
