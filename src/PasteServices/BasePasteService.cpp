#include <QDebug>
#include "BasePasteService.h"

BasePasteService::BasePasteService(QSettings *settings, QObject *parent) :
	QObject(parent),
	settings(settings)
{
	manager = new QNetworkAccessManager(this);

	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
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
