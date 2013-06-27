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

	m_label = settings->value("Label").toString();
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
}

void BasePasteService::requestFinished(QNetworkReply *reply)
{

}

void BasePasteService::provideAuthentication(QString username, QString password)
{

}

QByteArray BasePasteService::buildPostData(QHash<QString, QString> &data)
{
	QUrl post;
	QHashIterator<QString, QString> i(data);

	while(i.hasNext())
	{
		i.next();
		post.addQueryItem(i.key(), i.value());
	}

	return post.encodedQuery();
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
