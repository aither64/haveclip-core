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
