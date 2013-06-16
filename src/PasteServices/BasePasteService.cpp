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
