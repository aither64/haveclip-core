#ifndef HAVESNIPPET_H
#define HAVESNIPPET_H

#include "../BasePasteService.h"

class HaveSnippet : public BasePasteService
{
	Q_OBJECT
public:
	explicit HaveSnippet(QObject *parent = 0);
	explicit HaveSnippet(QSettings *settings, QObject *parent = 0);
	PasteService type();
	QString internalName();
	void applySettings(QHash<QString, QVariant> s);
	void saveSettings();
	QString url();
	QString name();
	QString title();
	QString shortDescription();
	QString lang();
	QString fullDescription();
	QString tags();
	int accessibility();
	int expiration();
	QString apiKey();
	static QUrl apiUrl(QString base, QString cmd);

signals:

public slots:
	void paste(QString data);
	void paste(QHash<QString, QVariant> settings, QString data);
	void retryPaste();

protected slots:
	void requestFinished(QNetworkReply *reply);

private:
	QString m_url;
	QString m_name;
	QString m_title;
	QString m_shortDescription;
	QString m_lang;
	QString m_fullDescription;
	QString m_tags;
	int m_accessibility;
	int m_expire;
	QString m_apiKey;
	QByteArray lastPaste;

private slots:
	void paste(QHash<QString, QString> &data);
};

#endif // HAVESNIPPET_H
