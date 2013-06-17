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
	struct Language {
		const char *name;
		const char *label;
	};

	enum PasteService {
		Stikked=0,
		Pastebin,
		PasteServiceCount,
		None
	};

	explicit BasePasteService(QSettings *settings, QObject *parent = 0);
	virtual PasteService type() = 0;
	virtual QString internalName() = 0;
	virtual QString label() = 0;
	virtual void applySettings(QHash<QString, QVariant> s) = 0;
	static int langIndexFromName(Language *lang, QString name);

signals:
	void authenticationRequired(QString username, bool failed, QString msg);
	void pasteFailed(QString error);
	void pasted(QUrl url);
	void errorOccured(QString error);
	
public slots:
	virtual void provideAuthentication(QString username, QString password);
	virtual void paste(QString data) = 0;
	virtual void paste(QHash<QString, QVariant> settings, QString data) = 0;

protected slots:
	virtual void requestFinished(QNetworkReply *reply);

protected:
	QSettings *settings;
	QNetworkAccessManager *manager;

	QByteArray buildPostData(QHash<QString, QString> &post);
};

#endif // BASEPASTESERVICE_H
