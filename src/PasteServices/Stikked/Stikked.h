#ifndef STIKKED_H
#define STIKKED_H

#include "../BasePasteService.h"

class Stikked : public BasePasteService
{
	Q_OBJECT
public:
	struct Language {
		const char *name;
		const char *label;
	};

	static Language languages[];

	explicit Stikked(QSettings *settings, QObject *parent = 0);
	PasteService type();
	QString internalName();
	QString label();
	void applySettings(QHash<QString, QVariant> s);
	QString url();
	QString name();
	QString title();
	bool isPrivate();
	QString lang();
	int expiration();
	
signals:
	
public slots:
	void paste(QString data);
	void paste(QHash<QString, QVariant> settings, QString data);

protected slots:
	void requestFinished(QNetworkReply *reply);

private:
	QString m_url;
	QString m_name;
	QString m_title;
	bool m_privatePaste;
	QString m_lang;
	int m_expire;

	QByteArray buildPostData(QHash<QString, QString> &post);

private slots:
	void paste(QHash<QString, QString> &data);
	
};

#endif // STIKKED_H
