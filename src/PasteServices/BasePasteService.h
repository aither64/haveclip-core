#ifndef BASEPASTESERVICE_H
#define BASEPASTESERVICE_H

#include <QObject>
#include <QSettings>
#include <QHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class BasePasteService : public QObject
{
	Q_OBJECT
public:
	enum PasteService {
		Stikked=0,
		PasteServiceCount,
		None
	};

	explicit BasePasteService(QSettings *settings, QObject *parent = 0);
	virtual PasteService type() = 0;
	virtual QString internalName() = 0;
	virtual QString label() = 0;
	virtual void applySettings(QHash<QString, QVariant> s) = 0;

signals:
	void pasted(QUrl url);
	
public slots:
	virtual void paste(QString data) = 0;
	virtual void paste(QHash<QString, QVariant> settings, QString data) = 0;

protected slots:
	virtual void requestFinished(QNetworkReply *reply);

protected:
	QSettings *settings;
	QNetworkAccessManager *manager;
};

#endif // BASEPASTESERVICE_H
